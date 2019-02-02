/*
 * preprocess.c
 *
 *  Created on: Feb 2, 2019
 *      Author: dizcza
 */

#include "preprocess.h"
#include "char_patterns.h"

typedef struct BBox {
	uint16_t xmin, ymin, xmax, ymax;
} BBox;

/* Reduced buffer */
static uint16_t m_buffer_reduced_x[PATTERN_SIZE];
static uint16_t m_buffer_reduced_y[PATTERN_SIZE];

static void GetBBox(uint16_t* bufferX, uint16_t* bufferY, uint32_t size,
		BBox* box) {
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
	box->xmin = xmin;
	box->ymin = ymin;
	box->xmax = xmax;
	box->ymax = ymax;
}

static void ReduceTouches(uint16_t* bufferX, uint16_t* bufferY,
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

static void Normalize(uint16_t* bufferX, uint16_t* bufferY, uint32_t size,
		BBox* box, DTW_Pattern* sample) {
	uint16_t width = box->xmax - box->xmin;
	uint16_t height = box->ymax - box->ymin;
	if (width / height > ASPECT_RATIO_MAX) {
		// height is too small
		height = width;
	} else if (height / width > ASPECT_RATIO_MAX) {
		// width is too small
		width = height;
	}
	const float32_t xc = 0.5f * (box->xmin + box->xmax);
	const float32_t yc = 0.5f * (box->ymin + box->ymax);
	const float32_t x_scale = 1.f / width;
	const float32_t y_scale = 1.f / height;
	uint32_t i;
	for (i = 0; i < size; i++) {
		sample->xcoords[i] = 0.5f + (bufferX[i] - xc) * x_scale;
		sample->ycoords[i] = 0.5f + (bufferY[i] - yc) * y_scale;
	}
	sample->size = size;
}

void DTW_Preprocess(uint16_t* bufferX, uint16_t* bufferY, uint32_t bufferSize,
		DTW_Pattern* sample) {
	if (bufferSize < 2) {
		return;  // not enough points
	}
	uint16_t* buffer_processed_x = bufferX;
	uint16_t* buffer_processed_y = bufferY;
	uint32_t n_touches = bufferSize;
	if (n_touches > PATTERN_SIZE) {
		// reduce to one size
		ReduceTouches(bufferX, bufferY, bufferSize);
		buffer_processed_x = m_buffer_reduced_x;
		buffer_processed_y = m_buffer_reduced_y;
		n_touches = PATTERN_SIZE;
	}
	BBox box;
	GetBBox(buffer_processed_x, buffer_processed_y, n_touches, &box);
	Normalize(buffer_processed_x, buffer_processed_y, n_touches, &box, sample);
}
