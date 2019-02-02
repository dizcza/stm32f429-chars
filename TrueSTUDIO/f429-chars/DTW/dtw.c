/*
 * dtw.c
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 *
 */

#include "dtw.h"
#include "math.h"
#include "char_patterns.h"

static float32_t m_dist_pattern[PATTERN_SIZE];
static float32_t m_dist_matrix[PATTERN_SIZE][PATTERN_SIZE];

void DTW_ComputeDistance(DTW_Pattern* sample, DTW_Pattern* pattern,
		float32_t* dist) {
	int32_t i, j;
	float32_t cost, insertion, deletion, match;
	for (i = 0; i < sample->size; i++) {
		m_dist_matrix[i][0] = HUGE_VALF;
	}
	for (j = 0; j < pattern->size; j++) {
		m_dist_matrix[0][j] = HUGE_VALF;
	}
	m_dist_matrix[0][0] = 0.f;
	for (i = 1; i < sample->size; i++) {
		for (j = 1; j < pattern->size; j++) {
			float32_t dx = sample->xcoords[i] - pattern->xcoords[j];
			float32_t dy = sample->ycoords[i] - pattern->ycoords[j];
			cost = dx * dx + dy * dy;
			insertion = m_dist_matrix[i - 1][j];
			deletion = m_dist_matrix[i][j - 1];
			match = m_dist_matrix[i - 1][j - 1];
			m_dist_matrix[i][j] = cost
					+ fminf(insertion, fminf(deletion, match));
		}
	}
	*dist = m_dist_matrix[sample->size - 1][pattern->size - 1];
}

void DTW_ClassifyChar(DTW_Pattern* pattern, uint8_t* predictedChar) {
	int32_t pattern_id, predicted_id = 0;
	float32_t dist, dist_min = HUGE_VALF;
	DTW_Pattern stored_pattern;
	stored_pattern.size = PATTERN_SIZE;
	for (pattern_id = 0; pattern_id < ALPHABET_SIZE; pattern_id++) {
		stored_pattern.xcoords = (float32_t*) PATTERN_COORDS_X[pattern_id];
		stored_pattern.ycoords = (float32_t*) PATTERN_COORDS_Y[pattern_id];
		DTW_ComputeDistance(pattern, &stored_pattern, &dist);
		if (dist < dist_min) {
			dist_min = dist;
			predicted_id = pattern_id;
		}
		m_dist_pattern[pattern_id] = dist;
	}
	*predictedChar = ALPHABET[predicted_id];
}
