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

#include <stdint.h>
#include "arm_math.h"

typedef struct DTW_Pattern {
	float32_t *xcoords, *ycoords;
	uint32_t size;
} DTW_Pattern;

void DTW_ComputeDistance(DTW_Pattern* sample, DTW_Pattern* pattern,
		float32_t* dist);
void DTW_ClassifyChar(DTW_Pattern* sample, uint8_t* predictedChar);

#endif /* DTW_DTW_H_ */
