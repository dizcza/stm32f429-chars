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

#ifndef DTW_DTW_H_
#define DTW_DTW_H_

#define USE_EUCLIDEAN

#include <stdint.h>
#include "arm_math.h"
#include "char_patterns.h"

#ifdef CHAR_PATTERNS_DATATYPE_Q7
typedef struct CharPattern_32t {
	float32_t *xcoords, *ycoords;
	uint32_t size;
} CharPattern_32t;
#else
typedef CharPattern CharPattern_32t;
#endif  /* CHAR_PATTERNS_DATATYPE_Q7 */


void DTW_ComputeDistance(const CharPattern_32t* sample, const CharPattern_32t* pattern,
		float32_t* dist);
void Euclidean_ComputeDistance(const CharPattern_32t* sample, const CharPattern_32t* pattern,
		float32_t* dist);
void DTW_ClassifyChar(const CharPattern* sample, CharPattern_PredictedInfo* resultInfo);

void DTW_PrintResult(const CharPattern_PredictedInfo* resultInfo);
void DTW_DrawPattern(const CharPattern* pattern);

#endif /* DTW_DTW_H_ */
