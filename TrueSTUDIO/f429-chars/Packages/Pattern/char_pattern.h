
/*
 * char_patterns.h
 *
 *  Created on: Jan 31, 2019
 *      Author: Danylo Ulianych
 */

#ifndef CHAR_PATTERN_H_
#define CHAR_PATTERN_H_

#include <stdint.h>
#include "grunet.h"

#define PATTERN_SIZE (AI_GRUNET_IN_1_SIZE / 2)


typedef struct CharPattern {
    float *xcoords, *ycoords;
    uint32_t size;
} CharPattern;

typedef struct CharPattern_PredictedInfo {
    char predicted_char;
    uint32_t duration;
    float confidence;
} CharPattern_PredictedInfo;

void CharPattern_PrintResult(const CharPattern_PredictedInfo* resultInfo);
void CharPattern_Draw(const CharPattern* pattern);

#endif /* CHAR_PATTERN_H_ */
