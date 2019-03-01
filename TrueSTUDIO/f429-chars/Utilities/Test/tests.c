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

void arm_add_f32_mine(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t blockSize)
{
  uint32_t blkCnt;                               /* loop counter */

/* Run the below code for Cortex-M4 and Cortex-M3 */
  /*loop Unrolling */
  blkCnt = blockSize / 9;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.
   ** a second loop below computes the remaining 1 to 3 samples. */
  while (blkCnt > 0U)
  {
    /* C = A + B */
    /* Add and then store the results in the destination buffer. */

    /* read four inputs from sourceA and four inputs from sourceB */
    *pDst = *pSrcA + *pSrcB;
    *(pDst + 1) = *(pSrcA + 1) + *(pSrcB + 1);
    *(pDst + 2) = *(pSrcA + 2) + *(pSrcB + 2);
    *(pDst + 3) = *(pSrcA + 3) + *(pSrcB + 3);
    *(pDst + 4) = *(pSrcA + 4) + *(pSrcB + 4);
    *(pDst + 5) = *(pSrcA + 5) + *(pSrcB + 5);
    *(pDst + 6) = *(pSrcA + 6) + *(pSrcB + 6);
    *(pDst + 7) = *(pSrcA + 7) + *(pSrcB + 7);
    *(pDst + 8) = *(pSrcA + 8) + *(pSrcB + 8);

    /* update pointers to process next samples */
    pSrcA += 9U;
    pSrcB += 9U;
    pDst += 9U;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.
   ** No loop unrolling is used. */
  blkCnt = blockSize % 9;

  while (blkCnt > 0U)
  {
    /* C = A + B */
    /* Add and then store the results in the destination buffer. */
    *pDst++ = (*pSrcA++) + (*pSrcB++);

    /* Decrement the loop counter */
    blkCnt--;
  }
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')


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
	arm_add_f32_mine(v1, v2, res_arm, TEST_ARRAY_SIZE);
	Test_AllClose((const float32_t*) res_arm, (const float32_t*) res_true, TEST_ARRAY_SIZE);

	// timings
#ifdef TEST_VERBOSE
	uint16_t start_line = 0;
	uint8_t message[30];
	uint32_t tick, dur, repeat=10000;

	q7_t val_q7, val2_q7, val3_q7;
	float val_f32 = -1;
	arm_float_to_q7(&val_f32, &val_q7, 1);
	sprintf((char*) message, "%.2f: "BYTE_TO_BINARY_PATTERN, val_f32, BYTE_TO_BINARY(val_q7));
	BSP_LCD_DisplayStringAtLine(start_line++, message);
	val_f32 = 0.75f;

	arm_float_to_q7(&val_f32, &val2_q7, 1);
	sprintf((char*) message, "%.2f: "BYTE_TO_BINARY_PATTERN, val_f32, BYTE_TO_BINARY(val2_q7));
	BSP_LCD_DisplayStringAtLine(start_line++, message);

	arm_add_q7(&val_q7, &val2_q7, &val3_q7, 1);
	arm_add_q7(&val3_q7, &val2_q7, &val3_q7, 1);
	arm_q7_to_float(&val3_q7, &val_f32, 1);
	sprintf((char*) message, "%.2f: "BYTE_TO_BINARY_PATTERN, val_f32, BYTE_TO_BINARY(val3_q7));
	BSP_LCD_DisplayStringAtLine(start_line++, message);

	q7_t d = 0b10100000;
	arm_q7_to_float(&d, &val_f32, 1);
	sprintf((char*) message, "%.2f: "BYTE_TO_BINARY_PATTERN, val_f32, BYTE_TO_BINARY(d));
	BSP_LCD_DisplayStringAtLine(start_line++, message);


	tick = HAL_GetTick();
	for (trial = 0; trial < repeat; trial++) {
		arm_add_f32_mine(v1, v2, res_arm, TEST_ARRAY_SIZE);
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
