/*
 * preprocess.c
 *
 *  Created on: Feb 20, 2019
 *      Author: dizcza
 */

#include "math.h"
#include "preprocess.h"

#define COTANG_50  0.8391f

/* Reduced buffer */
static uint16_t m_buffer_reduced_x[PATTERN_SIZE];
static uint16_t m_buffer_reduced_y[PATTERN_SIZE];

const BBox Preprocess_NormalizedBox = { -1.f, -1.f, 1.f, 1.f };

void Preprocess_GetBBox(const CharPattern* pattern, BBox* box) {
	float xmin = INFINITY, xmax = -INFINITY, ymin = INFINITY, ymax = -INFINITY;
	float x, y;
	uint32_t i;
	for (i = 0; i < pattern->size; i++) {
		x = pattern->xcoords[i];
		y = pattern->ycoords[i];
		xmin = fminf(x, xmin);
		xmax = fmaxf(x, xmax);
		ymin = fminf(y, ymin);
		ymax = fmaxf(y, ymax);
	}
	box->xmin = xmin;
	box->ymin = ymin;
	box->xmax = xmax;
	box->ymax = ymax;
}

void Preprocess_GetBoxCenter(const BBox* box, float* xc, float* yc) {
	*xc = 0.5f * (box->xmin + box->xmax);
	*yc = 0.5f * (box->ymin + box->ymax);
}

void Preprocess_GetBoxScale(const BBox *boxFrom, const BBox *boxTo,
		float *scaleX, float *scaleY) {
	*scaleX = (boxTo->xmax - boxTo->xmin) / (boxFrom->xmax - boxFrom->xmin);
	*scaleY = (boxTo->ymax - boxTo->ymin) / (boxFrom->ymax - boxFrom->ymin);
#if PREPROCESS_KEEP_ASPECT_RATIO
	const float scale = fminf(*scaleX, *scaleY);
	*scaleX = scale;
	*scaleY = scale;
#endif  /* PREPROCESS_KEEP_ASPECT_RATIO */
}

static void MakeBufferFixedSize(const uint16_t* bufferX, const uint16_t* bufferY,
		uint32_t bufferSize) {
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

void Preprocess_Normalize(CharPattern* sample) {
	BBox boxSample;
	Preprocess_GetBBox(sample, &boxSample);
	float xc, yc, xc_normalized, yc_normalized, scale_x, scale_y;
	Preprocess_GetBoxCenter(&boxSample, &xc, &yc);
	Preprocess_GetBoxCenter(&Preprocess_NormalizedBox, &xc_normalized, &yc_normalized);
	Preprocess_GetBoxScale(&boxSample, &Preprocess_NormalizedBox, &scale_x, &scale_y);
	uint32_t i;
	for (i = 0; i < sample->size; i++) {
		sample->xcoords[i] = xc_normalized + (sample->xcoords[i] - xc) * scale_x;
		sample->ycoords[i] = yc_normalized + (sample->ycoords[i] - yc) * scale_y;
	}
}

void Preprocess_NormalizeInverse(const CharPattern* sample, uint16_t* bufferX,
		uint16_t* bufferY, const BBox* boxTo) {
	float xc_screen, yc_screen, xc_pattern, yc_pattern, scale_x, scale_y;
	Preprocess_GetBoxCenter(boxTo, &xc_screen, &yc_screen);
	BBox box_pattern;
	Preprocess_GetBBox(sample, &box_pattern);
	Preprocess_GetBoxCenter(&box_pattern, &xc_pattern, &yc_pattern);
	Preprocess_GetBoxScale(&box_pattern, boxTo, &scale_x, &scale_y);
	uint32_t i;
	float x_float, y_float;
	for (i = 0; i < sample->size; i++) {
		x_float = fmaxf(xc_screen + (sample->xcoords[i] - xc_pattern) * scale_x, 0);
		y_float = fmaxf(yc_screen + (sample->ycoords[i] - yc_pattern) * scale_y, 0);
		bufferX[i] = (uint16_t) x_float;
		bufferY[i] = (uint16_t) y_float;
	}
}

void Preprocess_CorrectSlant(CharPattern* pattern) {
	float dx, dy, dx_slant = 0.f, dy_slant = 0.f;
	uint32_t i;
	for (i = 1; i < pattern->size; i++) {
		dx = pattern->xcoords[i] - pattern->xcoords[i - 1];
		dy = pattern->ycoords[i] - pattern->ycoords[i - 1];
		if (fabsf(dy / dx) > COTANG_50) {
			if (dy < 0) {
				dy *= -1;
				dx *= -1;
			}
			dx_slant += dx;
			dy_slant += dy;
		}
	}
	if (fabsf(dy_slant) < 1e-5) {
		// don't correct horizontal slam
		return;
	}
	const float shear = dx_slant * (-1.f) / dy_slant;
	for (i = 0; i < pattern->size; i++) {
		pattern->xcoords[i] = pattern->xcoords[i] + pattern->ycoords[i] * shear;
	}
}

void Preprocess_ToFloat(const uint16_t* bufferX, const uint16_t* bufferY, CharPattern* output) {
	uint32_t i = 0;
	for (i = 0; i < output->size; i++) {
		output->xcoords[i] = (float) bufferX[i];
		output->ycoords[i] = (float) bufferY[i];
	}
}

void Preprocess_MakePattern(const uint16_t* bufferX, const uint16_t* bufferY,
		uint32_t bufferSize, CharPattern* sample) {
	if (bufferSize < 2) {
		return;  // not enough points
	}
	// reduce to one size
	MakeBufferFixedSize(bufferX, bufferY, bufferSize);
	sample->size = PATTERN_SIZE;
	Preprocess_ToFloat(m_buffer_reduced_x, m_buffer_reduced_y, sample);
	Preprocess_CorrectSlant(sample);
	Preprocess_Normalize(sample);
}
