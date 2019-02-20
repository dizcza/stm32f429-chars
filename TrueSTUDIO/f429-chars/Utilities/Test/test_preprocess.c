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

static void AssertVerticalLine(uint16_t *xs, uint16_t *ys, uint32_t size) {
	uint32_t i;
	int32_t dy;
	const uint16_t x0 = xs[0];
	const int32_t dy0 = ((int32_t) ys[1]) - ys[0];
	assert_param(dy0 != 0);
	for (i = 1; i < size; i++) {
		assert_param(xs[i] == x0);
		dy = ((int32_t) ys[i]) - ys[i - 1];
		assert_param(dy == dy0);
	}
}

static void AssertHorizontalLine(uint16_t *xs, uint16_t *ys, uint32_t size) {
	uint32_t i;
	int32_t dx;
	const uint16_t y0 = ys[0];
	const int32_t dx0 = ((int32_t) xs[1]) - xs[0];
	assert_param(dx0 != 0);
	for (i = 1; i < size; i++) {
		assert_param(ys[i] == y0);
		dx = ((int32_t) xs[i]) - xs[i - 1];
		assert_param(dx == dx0);
	}
}

void Test_Preprocess_CorrectSlant() {
	uint16_t xs[TEST_PREPROCESS_BUFFER_SIZE];
	uint16_t ys[TEST_PREPROCESS_BUFFER_SIZE];
	uint16_t i;

	uint8_t flip_x, flip_y;
	for (flip_x = 0; flip_x < 2; flip_x++) {
		for (flip_y = 0; flip_y < 2; flip_y++) {
			for (i = 0; i < TEST_PREPROCESS_BUFFER_SIZE; i++) {
				xs[i] = i;
				ys[i] = i;
				if (flip_x) {
					xs[i] = TEST_PREPROCESS_BUFFER_SIZE - xs[i];
				}
				if (flip_y) {
					ys[i] = TEST_PREPROCESS_BUFFER_SIZE - ys[i];
				}
			}
			Preprocess_CorrectSlant(xs, ys, TEST_PREPROCESS_BUFFER_SIZE);
			AssertVerticalLine(xs, ys, TEST_PREPROCESS_BUFFER_SIZE);
		}
	}

	for (i = 0; i < TEST_PREPROCESS_BUFFER_SIZE; i++) {
		xs[i] = i;
		ys[i] = 10;
	}
	Preprocess_CorrectSlant(xs, ys, TEST_PREPROCESS_BUFFER_SIZE);
	AssertHorizontalLine(xs, ys, TEST_PREPROCESS_BUFFER_SIZE);
}
