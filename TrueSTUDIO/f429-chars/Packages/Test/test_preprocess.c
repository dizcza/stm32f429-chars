/*
 * test_preprocess.c
 *
 *  Created on: Feb 20, 2019
 *      Author: dizcza
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "Test/tests.h"
#include "Preprocess/preprocess.h"

#define TEST_PREPROCESS_BUFFER_SIZE  20
#define TEST_PREPROCESS_EPS (1e-6)

static void AssertVerticalLine(float *xs, float *ys, uint32_t size) {
	uint32_t i;
	float dy;
	const float x0 = xs[0];
	const float dy0 = ys[1] - ys[0];
	assert_param(fabsf(dy0) < TEST_PREPROCESS_EPS);
	for (i = 1; i < size; i++) {
		assert_param(fabsf(xs[i] - x0) < TEST_PREPROCESS_EPS);
		dy = ys[i] - ys[i - 1];
		assert_param(fabsf(dy - dy0) < TEST_PREPROCESS_EPS);
	}
}

static void AssertHorizontalLine(float *xs, float *ys, uint32_t size) {
	uint32_t i;
	float dx;
	const float y0 = ys[0];
	const float dx0 = xs[1] - xs[0];
	assert_param(fabsf(dx0) < TEST_PREPROCESS_EPS);
	for (i = 1; i < size; i++) {
		assert_param(fabsf(ys[i] - y0) < TEST_PREPROCESS_EPS);
		dx = xs[i] - xs[i - 1];
		assert_param(fabsf(dx - dx0) < TEST_PREPROCESS_EPS);
	}
}

void Test_Preprocess_CorrectSlant() {
	float xs[TEST_PREPROCESS_BUFFER_SIZE];
	float ys[TEST_PREPROCESS_BUFFER_SIZE];
	CharPattern pattern = {xs, ys, TEST_PREPROCESS_BUFFER_SIZE};
	uint16_t i;

	uint8_t flip_x, flip_y;
	for (flip_x = 0; flip_x < 2; flip_x++) {
		for (flip_y = 0; flip_y < 2; flip_y++) {
			for (i = 0; i < TEST_PREPROCESS_BUFFER_SIZE; i++) {
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
