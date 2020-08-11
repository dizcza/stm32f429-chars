/*
 * tests.h
 *
 *  Created on: Feb 17, 2019
 *      Author: dizcza
 */

#ifndef TEST_TESTS_H_
#define TEST_TESTS_H_

#define TEST_EPS (1e-5)

#include <math.h>
#include "stm32f4xx_hal.h"
#include "gru_infer.h"

__STATIC_INLINE void assert_isclose(float a, float b) {
	assert_param(fabsf(a - b) < TEST_EPS);
}

void Test_Preprocess_CorrectSlant();
void Test_Preprocess_Normalize();
void Test_ShearTransformUI();
void Test_GRU(ai_handle network);

#endif /* TEST_TESTS_H_ */
