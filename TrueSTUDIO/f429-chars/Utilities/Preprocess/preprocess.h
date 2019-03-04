/*
 * preprocess.h
 *
 *  Created on: Feb 20, 2019
 *      Author: dizcza
 */

#ifndef PREPROCESS_PREPROCESS_H_
#define PREPROCESS_PREPROCESS_H_

#define PREPROCESS_KEEP_ASPECT_RATIO  1U

#include "dtw.h"

typedef struct BBox {
	float32_t xmin, ymin, xmax, ymax;
} BBox;

extern const BBox CharPatterns_Box;

void Preprocess_MakePattern(uint16_t* bufferX, uint16_t* bufferY,
		uint32_t bufferSize, CharPattern* sample);

void Preprocess_Normalize(const uint16_t* bufferX, const uint16_t* bufferY,
		CharPattern* sample);
void Preprocess_NormalizeInverse(const CharPattern* sample, uint16_t* bufferX,
		uint16_t* bufferY, const BBox* boxTo);
void Preprocess_CorrectSlant(uint16_t* bufferX, uint16_t* bufferY,
		uint32_t size);

#endif /* PREPROCESS_PREPROCESS_H_ */
