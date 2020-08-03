/*
 * preprocess.h
 *
 *  Created on: Feb 20, 2019
 *      Author: dizcza
 */

#ifndef PREPROCESS_PREPROCESS_H_
#define PREPROCESS_PREPROCESS_H_

#define PREPROCESS_KEEP_ASPECT_RATIO  1U

#include "char_pattern.h"

typedef struct BBox {
	float xmin, ymin, xmax, ymax;
} BBox;

extern const BBox Preprocess_NormalizedBox;

void Preprocess_GetBBox(const CharPattern* pattern, BBox* box);
void Preprocess_GetBoxScale(const BBox *boxFrom, const BBox *boxTo,
		float *scaleX, float *scaleY);
void Preprocess_GetBoxCenter(const BBox* box, float* xc, float* yc);
void Preprocess_Normalize(CharPattern* sample);
void Preprocess_NormalizeInverse(const CharPattern* sample, uint16_t* bufferX,
		uint16_t* bufferY, const BBox* boxTo);
void Preprocess_CorrectSlant(CharPattern* sample);
void Preprocess_ToFloat(const uint16_t* bufferX, const uint16_t* bufferY, CharPattern* output);

void Preprocess_MakePattern(const uint16_t* bufferX, const uint16_t* bufferY,
		uint32_t bufferSize, CharPattern* sample);

#endif /* PREPROCESS_PREPROCESS_H_ */
