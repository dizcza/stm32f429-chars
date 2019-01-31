/*
 * dtw.h
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 */

#ifndef DTW_DTW_H_
#define DTW_DTW_H_

#define PATTERN_LENGTH 90

int32_t DTW_ComputeDistance(const uint16_t x[][2], int32_t xSize, const uint16_t y[][2],
		int32_t ySize);
uint8_t DTW_ClassifyChar(uint16_t buffer[][2], int32_t bufferSize);

#endif /* DTW_DTW_H_ */
