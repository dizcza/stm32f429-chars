/*
 * preprocess.c
 *
 *  Created on: Feb 20, 2019
 *      Author: dizcza
 */


#include "preprocess.h"
#include "char_patterns.h"

#define COTANG_50  0.8391f

typedef struct BBox {
	uint16_t xmin, ymin, xmax, ymax;
} BBox;

typedef struct BBoxFloat {
	float32_t xmin, ymin, xmax, ymax;
} BBoxFloat;

/* Reduced buffer */
static uint16_t m_buffer_reduced_x[PATTERN_SIZE];
static uint16_t m_buffer_reduced_y[PATTERN_SIZE];
static BBoxFloat m_normalized_box;

static void GetBBox(const uint16_t* bufferX, const uint16_t* bufferY, uint32_t size,
		BBox* box) {
	uint16_t xmin = UINT16_MAX, xmax = 0, ymin = UINT16_MAX, ymax = 0;
	uint16_t i, x, y;
	for (i = 0; i < size; i++) {
		x = *bufferX++;
		y = *bufferY++;
		if (x < xmin) {
			xmin = x;
		}
		if (x > xmax) {
			xmax = x;
		}
		if (y < ymin) {
			ymin = y;
		}
		if (y > ymax) {
			ymax = y;
		}
	}
	box->xmin = xmin;
	box->ymin = ymin;
	box->xmax = xmax;
	box->ymax = ymax;
}

static void ReduceTouches(const uint16_t* bufferX, const uint16_t* bufferY,
		uint32_t bufferSize) {
	// reduce to one size
	const float step = bufferSize * 1.0f / PATTERN_SIZE;
	float accumulated = 0.f;
	int32_t id_raw, id_reduced;
	for (id_reduced = 0; id_reduced < PATTERN_SIZE; id_reduced++) {
		id_raw = (int32_t) accumulated;
		if (id_raw > bufferSize - 1) {
			id_raw = bufferSize - 1;
		}
		m_buffer_reduced_x[id_reduced] = bufferX[id_raw];
		m_buffer_reduced_y[id_reduced] = bufferY[id_raw];
		accumulated += step;
	}
}

static void Normalize(const uint16_t* bufferX, const uint16_t* bufferY, uint32_t size,
		const BBox* box, CharPattern* sample) {
	const float32_t xc = 0.5f * (box->xmin + box->xmax);
	const float32_t yc = 0.5f * (box->ymin + box->ymax);
	const float32_t xc_normalized = 0.5f * (m_normalized_box.xmax + m_normalized_box.xmin);
	const float32_t yc_normalized = 0.5f * (m_normalized_box.ymax + m_normalized_box.ymin);
	float32_t x_scale = (m_normalized_box.xmax - m_normalized_box.xmin) / (box->xmax - box->xmin);
	float32_t y_scale = (m_normalized_box.ymax - m_normalized_box.ymin) / (box->ymax - box->ymin);
#if PREPROCESS_KEEP_ASPECT_RATIO
	const float32_t scale = fminf(x_scale, y_scale);
	x_scale = scale;
	y_scale = scale;
#endif  /* PREPROCESS_KEEP_ASPECT_RATIO */

	uint32_t i;
	float32_t x_float, y_float;
	for (i = 0; i < size; i++) {
		x_float = xc_normalized + (*bufferX++ - xc) * x_scale;
		y_float = yc_normalized + (*bufferY++ - yc) * y_scale;
#ifdef CHAR_PATTERNS_DATATYPE_Q7
		*sample->xcoords++ = __SSAT((q31_t) (x_float * 128.0f), 8);
		*sample->ycoords++ = __SSAT((q31_t) (y_float * 128.0f), 8);
#else
		*sample->xcoords++ = x_float;
		*sample->ycoords++ = y_float;
#endif  /* CHAR_PATTERNS_DATATYPE_Q7 */
	}
	sample->size = size;
}


void Preprocess_Init() {
	m_normalized_box.xmin = -1.f;
	m_normalized_box.xmax = 1.f - CHAR_PATTERNS_RESOLUTION;
	m_normalized_box.ymin = -1.f;
	m_normalized_box.ymax = 1.f - CHAR_PATTERNS_RESOLUTION;
}

void Preprocess_CorrectSlant(uint16_t* bufferX, uint16_t* bufferY, uint32_t size) {
	int32_t dx, dy, dx_slant = 0, dy_slant = 0;
	uint32_t i;
	for (i = 1; i < size; i++) {
		dx = ((int32_t) bufferX[i]) - bufferX[i-1];
		dy = ((int32_t) bufferY[i]) - bufferY[i-1];
		if (fabsf(dy * 1.f / dx) > COTANG_50) {
			if (dy < 0) {
				dy *= -1;
				dx *= -1;
			}
			dx_slant += dx;
			dy_slant += dy;
		}
	}
	if (dy_slant == 0) {
		// don't correct horizontal slam
		return;
	}
	const float shear = dx_slant * (-1.f) / dy_slant;
	float x_shifted;
	for (i = 0; i < size; i++) {
		x_shifted = bufferX[i] + bufferY[i] * shear;
		x_shifted = fmaxf(x_shifted, 0);
		bufferX[i] = (uint16_t) x_shifted;
	}
}

void Preprocess_MakePattern(uint16_t* bufferX, uint16_t* bufferY, uint32_t bufferSize,
		CharPattern* sample) {
	if (bufferSize < 2) {
		return;  // not enough points
	}
	uint16_t* buffer_processed_x = bufferX;
	uint16_t* buffer_processed_y = bufferY;
	uint32_t n_touches = bufferSize;
	if (n_touches > PATTERN_SIZE) {
		// reduce to one size
		// todo inplace ReduceTouches
		ReduceTouches(bufferX, bufferY, bufferSize);
		buffer_processed_x = m_buffer_reduced_x;
		buffer_processed_y = m_buffer_reduced_y;
		n_touches = PATTERN_SIZE;
	}
	Preprocess_CorrectSlant(buffer_processed_x, buffer_processed_y, n_touches);
	BBox box;
	GetBBox(buffer_processed_x, buffer_processed_y, n_touches, &box);
	Normalize(buffer_processed_x, buffer_processed_y, n_touches, &box, sample);
}
