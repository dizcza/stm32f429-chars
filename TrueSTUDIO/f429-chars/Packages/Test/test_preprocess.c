/*
 * test_preprocess.c
 *
 *  Created on: Feb 20, 2019
 *      Author: dizcza
 */

#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "Test/tests.h"
#include "Preprocess/preprocess.h"

#define TEST_PREPROCESS_BUFFER_SIZE  20

static void AssertVerticalLine(float *xs, float *ys, uint32_t size) {
	uint32_t i;
	float dy;
	const float x0 = xs[0];
	const float dy0 = ys[1] - ys[0];
	assert_param(fabsf(dy0) > TEST_EPS);  // dy0 != 0
	for (i = 1; i < size; i++) {
		assert_close(xs[i], x0);
		dy = ys[i] - ys[i - 1];
		assert_close(dy, dy0);
	}
}

static void AssertHorizontalLine(float *xs, float *ys, uint32_t size) {
	uint32_t i;
	float dx;
	const float y0 = ys[0];
	const float dx0 = xs[1] - xs[0];
	assert_param(fabsf(dx0) > TEST_EPS);  // dx0 != 0
	for (i = 1; i < size; i++) {
		assert_close(ys[i], y0);
		dx = xs[i] - xs[i - 1];
		assert_close(dx, dx0);
	}
}

void Test_Preprocess_CorrectSlant() {
	float xs[TEST_PREPROCESS_BUFFER_SIZE];
	float ys[TEST_PREPROCESS_BUFFER_SIZE];
	CharPattern pattern = {xs, ys, TEST_PREPROCESS_BUFFER_SIZE};
	uint32_t i;

	uint8_t flip_x, flip_y;
	for (flip_x = 0; flip_x < 2; flip_x++) {
		for (flip_y = 0; flip_y < 2; flip_y++) {
			for (i = 0; i < pattern.size; i++) {
				pattern.xcoords[i] = (float) i;
				pattern.ycoords[i] = (float) i;
				if (flip_x) {
					pattern.xcoords[i] = TEST_PREPROCESS_BUFFER_SIZE - pattern.xcoords[i];
				}
				if (flip_y) {
					pattern.ycoords[i] = TEST_PREPROCESS_BUFFER_SIZE - pattern.ycoords[i];
				}
			}
			Preprocess_CorrectSlant(&pattern);
			AssertVerticalLine(pattern.xcoords, pattern.ycoords, TEST_PREPROCESS_BUFFER_SIZE);
		}
	}

	for (i = 0; i < TEST_PREPROCESS_BUFFER_SIZE; i++) {
		pattern.xcoords[i] = (float) i;
		pattern.ycoords[i] = 10.f;
	}
	Preprocess_CorrectSlant(&pattern);
	AssertHorizontalLine(pattern.xcoords, pattern.ycoords, TEST_PREPROCESS_BUFFER_SIZE);
}

void Test_Preprocess_Normalize() {
	const uint32_t width_test = 240;
	const uint32_t height_test = 320;
	float xs[TEST_PREPROCESS_BUFFER_SIZE];
	float ys[TEST_PREPROCESS_BUFFER_SIZE];
	CharPattern pattern = {xs, ys, TEST_PREPROCESS_BUFFER_SIZE};
	uint32_t i;
	for (i = 0; i < pattern.size; i++) {
		pattern.xcoords[i] = rand() % width_test;
		pattern.ycoords[i] = rand() % height_test;
	}
	Preprocess_Normalize(&pattern);
	BBox box;
	Preprocess_GetBBox(&pattern, &box);
#ifdef PREPROCESS_KEEP_ASPECT_RATIO
	float scale_x = (box.xmax - box.xmin) / (Preprocess_NormalizedBox.xmax - Preprocess_NormalizedBox.xmin);
	float scale_y = (box.ymax - box.ymin) / (Preprocess_NormalizedBox.ymax - Preprocess_NormalizedBox.ymin);
	if (scale_x > scale_y) {
		assert_close(box.xmin, Preprocess_NormalizedBox.xmin);
		assert_close(box.xmax, Preprocess_NormalizedBox.xmax);
		assert_param(box.ymin - Preprocess_NormalizedBox.ymin >= TEST_EPS);
		assert_param(box.ymax - Preprocess_NormalizedBox.ymax <= TEST_EPS);
	} else {
		assert_close(box.ymin, Preprocess_NormalizedBox.ymin);
		assert_close(box.ymax, Preprocess_NormalizedBox.ymax);
		assert_param(box.xmin - Preprocess_NormalizedBox.xmin >= TEST_EPS);
		assert_param(box.xmax - Preprocess_NormalizedBox.xmax <= TEST_EPS);
	}
#else
	assert_close(box.xmin, Preprocess_NormalizedBox.xmin);
	assert_close(box.xmax, Preprocess_NormalizedBox.xmax);
	assert_close(box.ymin, Preprocess_NormalizedBox.ymin);
	assert_close(box.ymax, Preprocess_NormalizedBox.ymax);
#endif  /* PREPROCESS_KEEP_ASPECT_RATIO */
}
