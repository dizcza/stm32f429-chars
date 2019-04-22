/*
 * preprocess.h
 *
 *  Created on: Feb 20, 2019
 *      Author: dizcza
 */

#ifndef PREPROCESS_PREPROCESS_H_
#define PREPROCESS_PREPROCESS_H_

#define PREPROCESS_KEEP_ASPECT_RATIO  1U

#include "Pattern/char_pattern.h"

typedef struct BBox {
	float xmin, ymin, xmax, ymax;
} BBox;

extern const BBox CharPatterns_Box;

void Preprocess_MakePattern(uint16_t* bufferX, uint16_t* bufferY,
		uint32_t bufferSize, CharPattern* sample);

void Preprocess_Normalize(CharPattern* sample);
void Preprocess_NormalizeInverse(const CharPattern* sample, uint16_t* bufferX,
		uint16_t* bufferY, const BBox* boxTo);
void Preprocess_CorrectSlant(CharPattern* sample);
void Preprocess_ToFloat(const uint16_t* bufferX, const uint16_t* bufferY, CharPattern* output);

#endif /* PREPROCESS_PREPROCESS_H_ */
