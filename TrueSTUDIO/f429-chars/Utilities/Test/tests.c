/*
 * tests.c
 *
 *  Created on: Feb 17, 2019
 *      Author: dizcza
 */


#include "Test/tests.h"
#include "stm32f4xx_hal.h"
#include "stm32f429i_discovery_lcd.h"

#define TEST_ARRAY_SIZE 100


void Test_AllClose(const float32_t *arr1, const float32_t *arr2, uint32_t size) {
	uint32_t i = 0;
	for (i = 0; i < size; i++) {
		assert_param(fabsf(arr1[i] - arr2[i]) < TEST_EPS);
	}
}

void Test_ArmAdd32() {
	// initialize arrays
	float32_t v1[TEST_ARRAY_SIZE];
	float32_t v2[TEST_ARRAY_SIZE];
	float32_t res_arm[TEST_ARRAY_SIZE];
	float32_t res_true[TEST_ARRAY_SIZE];
	uint32_t i, trial;
	for (i = 0; i < TEST_ARRAY_SIZE; i++) {
		v1[i] = (float32_t) i;
		v2[i] = sqrt(i);
		res_true[i] = v1[i] + v2[i];
	}

	// test correctness
	arm_add_f32(v1, v2, res_arm, TEST_ARRAY_SIZE);
	Test_AllClose((const float32_t*) res_arm, (const float32_t*) res_true, TEST_ARRAY_SIZE);

	// timings
#ifdef TEST_VERBOSE
	uint16_t start_line = 0;
	uint8_t message[30];
	uint32_t tick, dur, repeat=10000;

	tick = HAL_GetTick();
	for (trial = 0; trial < repeat; trial++) {
		arm_add_f32(v1, v2, res_arm, TEST_ARRAY_SIZE);
	}
	dur = HAL_GetTick() - tick;
	sprintf((char*) message, "arm_math %lu", dur);
	BSP_LCD_DisplayStringAtLine(start_line++, message);

	tick = HAL_GetTick();
	for (trial = 0; trial < repeat; trial++) {
		float32_t* v1_ptr = v1;
		float32_t* v2_ptr = v2;
		float32_t* res_ptr = res_true;
		for (i = 0; i < TEST_ARRAY_SIZE; i++) {
			*(res_ptr++) = *(v1_ptr++) + *(v2_ptr++);
		}
	}
	dur = HAL_GetTick() - tick;
	sprintf((char*) message, "Pointers %lu", dur);
	BSP_LCD_DisplayStringAtLine(start_line++, message);

	tick = HAL_GetTick();
	for (trial = 0; trial < repeat; trial++) {
		for (i = 0; i < TEST_ARRAY_SIZE; i++) {
			res_true[i] = v1[i] + v2[i];
		}
	}
	dur = HAL_GetTick() - tick;
	sprintf((char*) message, "Indexing %lu", dur);
	BSP_LCD_DisplayStringAtLine(start_line++, message);
#endif  /* TEST_VERBOSE */
}
