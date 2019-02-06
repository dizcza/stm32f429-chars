/*
 * test_onlinemean.c
 *
 *  Created on: Feb 6, 2019
 *      Author: dizcza
 */


#include <math.h>
#include "stm32f4xx_hal.h"
#include "test_onlinemean.h"
#include "onlinemean.h"

#define ONLINEMEAN_TEST_SIZE 10
#define ONLINEMEAN_ESP (1e-6)


void OnlineMean_Test() {
	OnlineMean oMean;
	OnlineMean_Init(&oMean);
	uint16_t i;
	uint16_t array[ONLINEMEAN_TEST_SIZE];
	float sum = 0, var = 0;
	for (i = 0; i < ONLINEMEAN_TEST_SIZE; i++) {
		array[i] = i;
		sum += array[i];
		OnlineMean_Update(&oMean, array[i]);
	}
	float mean = sum / ONLINEMEAN_TEST_SIZE;
	for (i = 0; i < ONLINEMEAN_TEST_SIZE; i++) {
		float delta = array[i] - mean;
		var += delta * delta;
	}
#if UNBIASED_ESTIMATOR
	float std = sqrt(var / (ONLINEMEAN_TEST_SIZE - 1));
#else
	float std = sqrt(var / ONLINEMEAN_TEST_SIZE);
#endif  /* UNBIASED_ESTIMATOR */
	float std_online = OnlineMean_GetStd(&oMean);
	assert_param(fabs(mean - oMean.mean) < ONLINEMEAN_ESP);
	assert_param(fabs(std - std_online) < ONLINEMEAN_ESP);
}
