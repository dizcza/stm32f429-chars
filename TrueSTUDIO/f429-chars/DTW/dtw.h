/*
 * dtw.h
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 *
 * Dynamic Time Warping implementation.
 * Calculates the distance between two time series of X- and Y-coordinates of touch points.
 * Algorithm complexity:
 *    - time: O(P*N^2);
 *    - memory: O(N^2 + P*N),
 *   where P - num. of stored patterns, N - pattern length.
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
