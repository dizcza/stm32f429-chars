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
#include "TouchScreen/ts_capture.h"
#include "stm32f429i_discovery_lcd.h"

#define __dtw_min(a, b) (a < b ? a : b)

static const int32_t m_alphabet_size = sizeof(ALPHABET) / sizeof(ALPHABET[0]);
static int32_t m_dist_pattern[PATTERN_LENGTH];
static int32_t m_dist_matrix[PATTERN_LENGTH][PATTERN_LENGTH];

static int32_t EuclideanDist(const uint16_t p1[2], const uint16_t p2[2]) {
	int32_t dx = ((int32_t) p1[0]) - p2[0];
	int32_t dy = ((int32_t) p1[1]) - p2[1];
	return dx * dx + dy * dy;
}

int32_t DTW_ComputeDistance(const uint16_t x[][2], int32_t xSize, const uint16_t y[][2],
		int32_t ySize) {
	int32_t i, j;
	int32_t cost, insertion, deletion, match;
	for (i = 0; i < xSize; i++) {
		m_dist_matrix[i][0] = INT32_MAX;
	}
	for (j = 0; j < ySize; j++) {
		m_dist_matrix[0][j] = INT32_MAX;
	}
	m_dist_matrix[0][0] = 0.f;
	for (i = 1; i < xSize; i++) {
		for (j = 1; j < ySize; j++) {
			cost = EuclideanDist(x[i], y[j]);
			insertion = m_dist_matrix[i - 1][j];
			deletion = m_dist_matrix[i][j - 1];
			match = m_dist_matrix[i - 1][j - 1];
			m_dist_matrix[i][j] = cost
					+ __dtw_min(insertion, __dtw_min(deletion, match));
		}
	}
	return m_dist_matrix[xSize - 1][ySize - 1];
}

uint8_t DTW_ClassifyChar(uint16_t buffer[][2], int32_t bufferSize) {
	int32_t pattern_id, predicted_id = 0;
	int32_t dist, dist_min = INT32_MAX;
	for (pattern_id = 0; pattern_id < m_alphabet_size; pattern_id++) {
		dist = DTW_ComputeDistance(buffer, bufferSize, PATTERN_COORDS[pattern_id], PATTERN_LENGTH);
		if (dist < dist_min) {
			dist_min = dist;
			predicted_id = pattern_id;
		}
		m_dist_pattern[pattern_id] = dist;
	}
	return ALPHABET[predicted_id];
}

//void Chars_DrawPatterns() {
//	int32_t pattern_id, i;
//	for (pattern_id = 1; pattern_id < m_alphabet_size; pattern_id++) {
//		const uint16_t** pattern = PATTERN_COORDS[pattern_id];
//		for (i = 1; i < PATTERN_LENGTH; i++) {
//			BSP_LCD_DrawLine(pattern[i-1][1], pattern[i-1][0], pattern[i][1], pattern[i][0]);
//		}
//	}
//}
