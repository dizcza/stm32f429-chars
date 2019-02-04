/*
 * dtw.c
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 *
 */

#include "stm32f4xx_hal.h"
#include "dtw.h"
#include "math.h"
#include "char_patterns.h"
#include <stdlib.h>

#define PATTERN_SIZE_EXTENDED (PATTERN_SIZE + 2)

static float32_t m_dist_matrix[PATTERN_SIZE + 1][PATTERN_SIZE + 1];
static float32_t m_dist_top[PATTERN_SIZE_EXTENDED];

int32_t int32_mod(int a, int b) {
	int mod = a % b;
	if (mod < 0) {
		mod += b;
	}
	return mod;
}


void DTW_ComputeDistanceOld(DTW_Pattern* sample, DTW_Pattern* pattern,
		float32_t* dist) {
	assert_param(pattern->size == PATTERN_SIZE);
	assert_param(sample->size <= PATTERN_SIZE);
	int32_t i, j;
	float32_t cost, insertion, deletion, match;
	for (i = 1; i <= sample->size; i++) {
		m_dist_matrix[i][0] = HUGE_VALF;
	}
	for (j = 1; j <= pattern->size; j++) {
		m_dist_matrix[0][j] = HUGE_VALF;
	}
	m_dist_matrix[0][0] = 0.f;
	for (i = 1; i <= sample->size; i++) {
		for (j = 1; j <= pattern->size; j++) {
			float32_t dx = sample->xcoords[i-1] - pattern->xcoords[j-1];
			float32_t dy = sample->ycoords[i-1] - pattern->ycoords[j-1];
			cost = dx * dx + dy * dy;
			insertion = m_dist_matrix[i - 1][j];
			deletion = m_dist_matrix[i][j - 1];
			match = m_dist_matrix[i - 1][j - 1];
			m_dist_matrix[i][j] = cost
					+ fminf(insertion, fminf(deletion, match));
		}
	}
	*dist = m_dist_matrix[sample->size][pattern->size];
}



void DTW_ComputeDistance(DTW_Pattern* sample, DTW_Pattern* pattern,
		float32_t* dist) {
	// note, that we don't constrain sample's size
	assert_param(pattern->size == PATTERN_SIZE);
	int32_t i, j, k;
	float32_t dx, dy, cost, insertion, deletion, match;
	for (j = 0; j < PATTERN_SIZE_EXTENDED; j++) {
		m_dist_top[j] = HUGE_VALF;
	}
	m_dist_top[1] = 0.f;
	for (i = 1; i <= sample->size; i++) {
		for (j = 1; j <= pattern->size; j++) {
			k = int32_mod(j - i + 1, PATTERN_SIZE_EXTENDED);
			dx = sample->xcoords[i-1] - pattern->xcoords[j-1];
			dy = sample->ycoords[i-1] - pattern->ycoords[j-1];
			cost = dx * dx + dy * dy;
			insertion = m_dist_top[int32_mod(k + 1, PATTERN_SIZE_EXTENDED)];
			deletion = m_dist_top[int32_mod(k - 1, PATTERN_SIZE_EXTENDED)];
			match = m_dist_top[k];
			m_dist_top[k] = cost
					+ fminf(insertion, fminf(deletion, match));
			if (fabs(m_dist_top[k] - m_dist_matrix[i][j]) > 1e-3) {
				volatile int temp = 2;
			}
		}
		m_dist_top[int32_mod(k + 1, PATTERN_SIZE_EXTENDED)] = HUGE_VALF;
	}
	*dist = m_dist_top[k];
}


void DTW_Test() {
	const int sample_sz = PATTERN_SIZE;
	float32_t xcoords_sample[sample_sz];
	float32_t ycoords_sample[sample_sz];
	float32_t xcoords_pattern[PATTERN_SIZE];
	float32_t ycoords_pattern[PATTERN_SIZE];
	DTW_Pattern sample = { xcoords_sample, ycoords_sample, sample_sz };
	DTW_Pattern pattern = { xcoords_pattern, ycoords_pattern, PATTERN_SIZE };
	int32_t i;
	srand(113);
	for (i = 0; i < sample_sz; i++) {
		xcoords_sample[i] = rand() % 7;
		ycoords_sample[i] = rand() % 7;
	}
	for (i = 0; i < PATTERN_SIZE; i++) {
		xcoords_pattern[i] = rand() % 7;
		ycoords_pattern[i] = rand() % 7;
	}
	float dist_old, dist;
	DTW_ComputeDistanceOld(&sample, &pattern, &dist_old);
	DTW_ComputeDistance(&sample, &pattern, &dist);
	assert_param(fabsf(dist - dist_old) < 1e-3);
}

void DTW_ClassifyChar(DTW_Pattern* pattern, uint8_t* predictedChar) {
	int32_t pattern_id, predicted_id = 0;
	float32_t dist, dist_min = HUGE_VALF;
	DTW_Pattern stored_pattern;
	stored_pattern.size = PATTERN_SIZE;
	for (pattern_id = 0; pattern_id < ALPHABET_SIZE; pattern_id++) {
		stored_pattern.xcoords = (float32_t*) PATTERN_COORDS_X[pattern_id];
		stored_pattern.ycoords = (float32_t*) PATTERN_COORDS_Y[pattern_id];
		DTW_ComputeDistanceOld(pattern, &stored_pattern, &dist);
		float dist_copy = dist;
		DTW_ComputeDistance(pattern, &stored_pattern, &dist);
		assert_param(fabsf(dist - dist_copy) < 1e-6);
		if (dist < dist_min) {
			dist_min = dist;
			predicted_id = pattern_id;
		}
	}
	*predictedChar = ALPHABET[predicted_id];
}
