/*
 * dtw_test.c
 *
 *  Created on: Feb 4, 2019
 *      Author: dizcza
 */

#include <stdlib.h>
#include "stm32f4xx_hal.h"

#include "char_patterns.h"
#include "dtw_test.h"
#include "dtw.h"

void DTW_ComputeDistanceMatrix(DTW_Pattern* sample, DTW_Pattern* pattern,
		float32_t* dist) {
	assert_param(pattern->size == PATTERN_SIZE);
	assert_param(sample->size <= PATTERN_SIZE);
	int32_t i, j;
	float32_t cost, insertion, deletion, match;
	float32_t dist_matrix[PATTERN_SIZE + 1][PATTERN_SIZE + 1];

	for (i = 1; i <= sample->size; i++) {
		dist_matrix[i][0] = HUGE_VALF;
	}
	for (j = 1; j <= pattern->size; j++) {
		dist_matrix[0][j] = HUGE_VALF;
	}
	dist_matrix[0][0] = 0.f;

	for (i = 1; i <= sample->size; i++) {
		for (j = 1; j <= pattern->size; j++) {
			float32_t dx = sample->xcoords[i - 1] - pattern->xcoords[j - 1];
			float32_t dy = sample->ycoords[i - 1] - pattern->ycoords[j - 1];
			cost = dx * dx + dy * dy;
			insertion = dist_matrix[i - 1][j];
			deletion = dist_matrix[i][j - 1];
			match = dist_matrix[i - 1][j - 1];
			dist_matrix[i][j] = cost
					+ fminf(insertion, fminf(deletion, match));
		}
	}
	*dist = dist_matrix[sample->size][pattern->size];
}

void DTW_Test() {
	float32_t xcoords_sample[PATTERN_SIZE];
	float32_t ycoords_sample[PATTERN_SIZE];
	float32_t xcoords_pattern[PATTERN_SIZE];
	float32_t ycoords_pattern[PATTERN_SIZE];
	DTW_Pattern sample = { xcoords_sample, ycoords_sample, PATTERN_SIZE };
	DTW_Pattern pattern = { xcoords_pattern, ycoords_pattern, PATTERN_SIZE };
	int32_t test_id;
	for (test_id = 0; test_id < 10; test_id++) {
		int32_t i;
		for (i = 0; i < PATTERN_SIZE; i++) {
			xcoords_sample[i] = rand() % 10;
			ycoords_sample[i] = rand() % 10;
			xcoords_pattern[i] = rand() % 10;
			ycoords_pattern[i] = rand() % 10;
		}
		float dist_old, dist;
		DTW_ComputeDistanceMatrix(&sample, &pattern, &dist_old);
		DTW_ComputeDistance(&sample, &pattern, &dist);
		assert_param(fabsf(dist - dist_old) < DTW_TEST_EPS);
	}
}
