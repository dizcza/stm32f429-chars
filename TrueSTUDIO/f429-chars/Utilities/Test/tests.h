/*
 * tests.h
 *
 *  Created on: Feb 17, 2019
 *      Author: dizcza
 */

#ifndef TEST_TESTS_H_
#define TEST_TESTS_H_

#include "arm_math.h"

#define TEST_EPS (1e-6)
#define TEST_VERBOSE  // uncomment to enable verbose tests


void Test_AllClose(const float32_t *arr1, const float32_t *arr2, uint32_t size);
void Test_ArmAdd32();
void Test_Preprocess_CorrectSlant();
void Test_ShearTransformUI();

#endif /* TEST_TESTS_H_ */
