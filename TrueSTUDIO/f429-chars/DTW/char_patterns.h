
/*
 * char_patterns.h
 *
 *  Created on: Jan 31, 2019
 *      Author: Danylo Ulianych
 */

#ifndef CHAR_PATTERNS_H_
#define CHAR_PATTERNS_H_

// do not modify this
//#define CHAR_PATTERNS_DATATYPE_Q7

#define PATTERN_SIZE      20
#define TOTAL_PATTERNS    141

#include <stdint.h>
#include "arm_math.h"

#ifdef CHAR_PATTERNS_DATATYPE_Q7
#define CHAR_PATTERNS_RESOLUTION  0.0078125f

typedef q7_t float_coord;
#else
#define CHAR_PATTERNS_RESOLUTION  0.0f

typedef float32_t float_coord;
#endif  /* CHAR_PATTERNS_DATATYPE_Q7 */

typedef struct CharPattern {
    float_coord *xcoords, *ycoords;
    uint32_t size;
} CharPattern;

typedef struct CharPattern_PredictedInfo {
    char predicted_char;
    uint32_t duration;
    float32_t distance;
} CharPattern_PredictedInfo;

extern const uint8_t PATTERN_LABEL[TOTAL_PATTERNS];
extern const float_coord PATTERN_COORDS_X[TOTAL_PATTERNS][PATTERN_SIZE];
extern const float_coord PATTERN_COORDS_Y[TOTAL_PATTERNS][PATTERN_SIZE];

#endif /* CHAR_PATTERNS_H_ */
