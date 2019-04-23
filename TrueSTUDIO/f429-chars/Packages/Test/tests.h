/*
 * tests.h
 *
 *  Created on: Feb 17, 2019
 *      Author: dizcza
 */

#ifndef TEST_TESTS_H_
#define TEST_TESTS_H_

#define TEST_EPS (1e-5)

#include "math.h"

__STATIC_INLINE void assert_close(float a, float b) {
	assert_param(fabsf(a - b) < TEST_EPS);
}

void Test_ArmAdd32();
void Test_Preprocess_CorrectSlant();
void Test_Preprocess_Normalize();
void Test_ShearTransformUI();

#endif /* TEST_TESTS_H_ */
