/*
 *  Created on: Jan 31, 2019
 *      Author: Danylo Ulianych
 *
 * Dynamic Time Warping implementation.
 * Calculates the distance between two time series of (X, Y) coordinates of touch points.
 * Algorithm complexity:
 *    1. Time: O(P*N^2);
 *    2. Memory:
 *        2.1. Flash: O(P*N);
 *        2.2. RAM:
 *            2.2.1) classic: O(N^2);
 *            2.2.2) online: O(P*N),
 *   where P - num. of stored patterns, N - pattern length.
 *   Classic DTW fills the cost distance matrix. While it computes the best match, user events are not captured.
 *   Online DTW stores the best distances so far with each of the stored patterns. After a new event is observed
 *   (or a batch of events), it updates the best distances. Using online DTW in a background thread, you won't
 *   skip user events.
 */

#include "stm32f4xx_hal.h"
#include "dtw.h"
#include "math.h"
#include "char_patterns.h"
#include "arm_math.h"

#define PATTERN_SIZE_EXTENDED (PATTERN_SIZE + 2)

// DTW cost matrix top line
static float32_t m_dist_top[PATTERN_SIZE_EXTENDED];

int32_t int32_mod(int32_t a, int32_t b) {
	int32_t mod = a % b;
	if (mod < 0) {
		mod += b;
	}
	return mod;
}

void DTW_ComputeDistance(const CharPattern_32t* sample, const CharPattern_32t* pattern,
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
			dx = sample->xcoords[i - 1] - pattern->xcoords[j - 1];
			dy = sample->ycoords[i - 1] - pattern->ycoords[j - 1];
			cost = dx * dx + dy * dy;
			insertion = m_dist_top[int32_mod(k + 1, PATTERN_SIZE_EXTENDED)];
			deletion = m_dist_top[int32_mod(k - 1, PATTERN_SIZE_EXTENDED)];
			match = m_dist_top[k];
			m_dist_top[k] = cost + fminf(insertion, fminf(deletion, match));
		}
		m_dist_top[int32_mod(k + 1, PATTERN_SIZE_EXTENDED)] = HUGE_VALF;
	}
	*dist = m_dist_top[k];
}

void DTW_ClassifyChar(const CharPattern* sample, CharPattern_PredictedInfo* resultInfo) {
	uint32_t tick_start = HAL_GetTick();
	uint32_t pattern_id, predicted_id = 0;
	float32_t dist, dist_min = HUGE_VALF;
	CharPattern_32t sample_32t, pattern;
	pattern.size = PATTERN_SIZE;
#ifdef CHAR_PATTERNS_DATATYPE_Q7
	float32_t sample_xy[2][TOTAL_PATTERNS];
	float32_t pattern_xy[2][TOTAL_PATTERNS];
	arm_q7_to_float(sample->xcoords, sample_xy[0], sample->size);
	arm_q7_to_float(sample->ycoords, sample_xy[1], sample->size);
	pattern.xcoords = pattern_xy[0];
	pattern.ycoords = pattern_xy[1];
#endif  /* CHAR_PATTERNS_DATATYPE_Q7 */
	for (pattern_id = 0; pattern_id < TOTAL_PATTERNS; pattern_id++) {
#ifdef CHAR_PATTERNS_DATATYPE_Q7
		arm_q7_to_float(PATTERN_COORDS_X[pattern_id], &pattern.xcoords, pattern.size);
		arm_q7_to_float(PATTERN_COORDS_Y[pattern_id], &pattern.ycoords, pattern.size);
#else
		pattern.xcoords = (float32_t*) PATTERN_COORDS_X[pattern_id];
		pattern.ycoords = (float32_t*) PATTERN_COORDS_Y[pattern_id];
#endif  /* CHAR_PATTERNS_DATATYPE_Q7 */
		DTW_ComputeDistance(sample, &pattern, &dist);
		if (dist < dist_min) {
			dist_min = dist;
			predicted_id = pattern_id;
		}
	}
	resultInfo->predicted_char = PATTERN_LABEL[predicted_id];
	resultInfo->duration = HAL_GetTick() - tick_start;
	resultInfo->distance = dist_min / sample->size;
}
