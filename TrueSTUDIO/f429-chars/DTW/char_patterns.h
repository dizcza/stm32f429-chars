/*
 * char_patterns.h
 *
 *  Created on: Jan 31, 2019
 *      Author: Danylo Ulianych
 */

#ifndef CHAR_PATTERNS_H_
#define CHAR_PATTERNS_H_

#define PATTERN_SIZE      50
#define TOTAL_PATTERNS    131

#include <stdint.h>
#include "arm_math.h"

extern const uint8_t PATTERN_LABEL[TOTAL_PATTERNS];

extern const float32_t PATTERN_COORDS_X[TOTAL_PATTERNS][PATTERN_SIZE];
extern const float32_t PATTERN_COORDS_Y[TOTAL_PATTERNS][PATTERN_SIZE];

#endif /* CHAR_PATTERNS_H_ */