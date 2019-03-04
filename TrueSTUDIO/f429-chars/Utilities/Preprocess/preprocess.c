/*
 * preprocess.c
 *
 *  Created on: Feb 20, 2019
 *      Author: dizcza
 */

#include "preprocess.h"
#include "char_patterns.h"

#define COTANG_50  0.8391f

/* Reduced buffer */
static uint16_t m_buffer_reduced_x[PATTERN_SIZE];
static uint16_t m_buffer_reduced_y[PATTERN_SIZE];

const BBox CharPatterns_Box = { -1.f, -1.f, 1.f - CHAR_PATTERNS_RESOLUTION, 1.f - CHAR_PATTERNS_RESOLUTION };

static void GetBBox(const uint16_t* bufferX, const uint16_t* bufferY,
		uint32_t size, BBox* box) {
	uint16_t xmin = UINT16_MAX, xmax = 0, ymin = UINT16_MAX, ymax = 0;
	uint16_t i, x, y;
	for (i = 0; i < size; i++) {
		x = bufferX[i];
		y = bufferY[i];
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
	box->xmin = (float32_t) xmin;
	box->ymin = (float32_t) ymin;
	box->xmax = (float32_t) xmax;
	box->ymax = (float32_t) ymax;
}

static void GetBoxCenter(const BBox* box, float32_t* xc, float32_t* yc) {
	*xc = 0.5f * (box->xmin + box->xmax);
	*yc = 0.5f * (box->ymin + box->ymax);
}

static void GetBoxScale(const BBox *boxFrom, const BBox *boxTo,
		float32_t *scaleX, float32_t *scaleY) {
	*scaleX = (boxTo->xmax - boxTo->xmin) / (boxFrom->xmax - boxFrom->xmin);
	*scaleY = (boxTo->ymax - boxTo->ymin) / (boxFrom->ymax - boxFrom->ymin);
#if PREPROCESS_KEEP_ASPECT_RATIO
	const float32_t scale = fminf(*scaleX, *scaleY);
	*scaleX = scale;
	*scaleY = scale;
#endif  /* PREPROCESS_KEEP_ASPECT_RATIO */
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

void Preprocess_Normalize(const uint16_t* bufferX, const uint16_t* bufferY,
		CharPattern* sample) {
	BBox boxSample;
	GetBBox(bufferX, bufferY, sample->size, &boxSample);
	float32_t xc, yc, xc_normalized, yc_normalized, scale_x, scale_y;
	GetBoxCenter(&boxSample, &xc, &yc);
	GetBoxCenter(&CharPatterns_Box, &xc_normalized, &yc_normalized);
	GetBoxScale(&boxSample, &CharPatterns_Box, &scale_x, &scale_y);
	uint32_t i;
	float32_t x_float, y_float;
	for (i = 0; i < sample->size; i++) {
		x_float = xc_normalized + (bufferX[i] - xc) * scale_x;
		y_float = yc_normalized + (bufferY[i] - yc) * scale_y;
#ifdef CHAR_PATTERNS_DATATYPE_Q7
		sample->xcoords[i] = __SSAT((q31_t) (x_float * 128.0f), 8);
		sample->ycoords[i] = __SSAT((q31_t) (y_float * 128.0f), 8);
#else
		sample->xcoords[i] = x_float;
		sample->ycoords[i] = y_float;
#endif  /* CHAR_PATTERNS_DATATYPE_Q7 */
	}
}

void Preprocess_NormalizeInverse(const CharPattern* sample, uint16_t* bufferX,
		uint16_t* bufferY, const BBox* boxTo) {
	float32_t xc, yc, xc_normalized, yc_normalized, scale_x, scale_y;
	GetBoxCenter(boxTo, &xc, &yc);
	GetBoxCenter(&CharPatterns_Box, &xc_normalized, &yc_normalized);
	GetBoxScale(&CharPatterns_Box, boxTo, &scale_x, &scale_y);
	uint32_t i;
	float32_t x_float, y_float;
	for (i = 0; i < sample->size; i++) {
#ifdef CHAR_PATTERNS_DATATYPE_Q7
		x_float = ((float32_t) sample->xcoords[i] / 128.0f);
		y_float = ((float32_t) sample->ycoords[i] / 128.0f);
#else
		x_float = sample->xcoords[i];
		y_float = sample->ycoords[i];
#endif  /* CHAR_PATTERNS_DATATYPE_Q7 */
		x_float = xc + (x_float - xc_normalized) * scale_x;
		y_float = yc + (y_float - yc_normalized) * scale_y;
		bufferX[i] = (uint16_t) x_float;
		bufferY[i] = (uint16_t) y_float;
	}
}

void Preprocess_CorrectSlant(uint16_t* bufferX, uint16_t* bufferY,
		uint32_t size) {
	int32_t dx, dy, dx_slant = 0, dy_slant = 0;
	uint32_t i;
	for (i = 1; i < size; i++) {
		dx = ((int32_t) bufferX[i]) - bufferX[i - 1];
		dy = ((int32_t) bufferY[i]) - bufferY[i - 1];
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

void Preprocess_MakePattern(uint16_t* bufferX, uint16_t* bufferY,
		uint32_t bufferSize, CharPattern* sample) {
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
	sample->size = n_touches;
	Preprocess_CorrectSlant(buffer_processed_x, buffer_processed_y, n_touches);
	Preprocess_Normalize(buffer_processed_x, buffer_processed_y, sample);
}
