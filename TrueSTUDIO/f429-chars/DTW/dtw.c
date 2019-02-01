/*
 * dtw.c
 *
 * VERSION 2.0 Andrew Slater, 20/8/1999
 * Latest changes 3/2006 by John Coleman
 *
 * DESCRIPTION
 * Compute a distance matrix on 2 multi-parameter vectors from 2 utterances,
 * and perform dynamic time warping on the distance matrix
 * where a, b ... n are parameters (e.g. f0, tongue-tip x co-ordinate)
 * 1 ... n is a time series
 * X and Y are 2 utterances
 *
 * Distance is calculated as:
 * Dist[x1][y1] = (X1a - Y1a)^2 + (X1b - Y1b)^2 + ... + (X1n - Y1n)^2, etc.
 */

#include <stdint.h>
#include "dtw.h"
#include "char_patterns.h"

#define __dtw_min(a, b) (a < b ? a : b)

static int32_t m_dist_pattern[PATTERN_SIZE];
static int32_t m_dist_matrix[PATTERN_SIZE][PATTERN_SIZE];

int32_t DTW_ComputeDistance(DTW_Pattern* x, DTW_Pattern* y) {
	int32_t i, j;
	int32_t cost, insertion, deletion, match;
	for (i = 0; i < x->size; i++) {
		m_dist_matrix[i][0] = INT32_MAX;
	}
	for (j = 0; j < y->size; j++) {
		m_dist_matrix[0][j] = INT32_MAX;
	}
	m_dist_matrix[0][0] = 0.f;
	for (i = 1; i < x->size; i++) {
		for (j = 1; j < y->size; j++) {
			int32_t dx = ((int32_t) x->xcoords[i]) - y->xcoords[j];
			int32_t dy = ((int32_t) x->ycoords[i]) - y->ycoords[j];
			cost = dx * dx + dy * dy;
			insertion = m_dist_matrix[i - 1][j];
			deletion = m_dist_matrix[i][j - 1];
			match = m_dist_matrix[i - 1][j - 1];
			m_dist_matrix[i][j] = cost
					+ __dtw_min(insertion, __dtw_min(deletion, match));
		}
	}
	return m_dist_matrix[x->size - 1][y->size - 1];
}

uint8_t DTW_ClassifyChar(DTW_Pattern* pattern) {
	int32_t pattern_id, predicted_id = 0;
	int32_t dist, dist_min = INT32_MAX;
	DTW_Pattern stored_pattern;
	stored_pattern.size = PATTERN_SIZE;
	for (pattern_id = 0; pattern_id < ALPHABET_SIZE; pattern_id++) {
		stored_pattern.xcoords = (uint16_t*) PATTERN_COORDS_X[pattern_id];
		stored_pattern.ycoords = (uint16_t*) PATTERN_COORDS_Y[pattern_id];
		dist = DTW_ComputeDistance(pattern, &stored_pattern);
		if (dist < dist_min) {
			dist_min = dist;
			predicted_id = pattern_id;
		}
		m_dist_pattern[pattern_id] = dist;
	}
	return ALPHABET[predicted_id];
}
