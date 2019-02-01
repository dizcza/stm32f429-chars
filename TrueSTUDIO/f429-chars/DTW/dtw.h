/*
 * dtw.h
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 */

#ifndef DTW_DTW_H_
#define DTW_DTW_H_

#include <stdint.h>

typedef struct DTW_Pattern {
	uint16_t *xcoords, *ycoords;
	uint32_t size;
} DTW_Pattern;

int32_t DTW_ComputeDistance(DTW_Pattern* x, DTW_Pattern* y);
uint8_t DTW_ClassifyChar(DTW_Pattern* pattern);

#endif /* DTW_DTW_H_ */
