/*
 * ts_capture.h
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 */

#ifndef TOUCHSCREEN_TS_CAPTURE_H_
#define TOUCHSCREEN_TS_CAPTURE_H_

#define TS_CAPTURE_CACHE_SIZE     1000
#define TS_CAPTURE_NO_TOUCH_TICK  20

#include <stdint.h>
#include "stm32f429i_discovery_ts.h"

typedef enum {
	EMPTY = 0,
	DIRTY,  // has some values
	FULL
} TS_Capture_CacheState;

/* Raw touches */
uint16_t TS_Capture_TouchesX[TS_CAPTURE_CACHE_SIZE];
uint16_t TS_Capture_TouchesY[TS_CAPTURE_CACHE_SIZE];


int8_t TS_Capture_SaveTouch(const TS_StateTypeDef *TsState);
void TS_Capture_DrawLastStroke();
void TS_Capture_DrawAllStrokes();
uint32_t TS_Capture_GetNumOfTouches();
TS_Capture_CacheState TS_Capture_GetCacheState();
void TS_Capture_Reset();

#endif /* TOUCHSCREEN_TS_CAPTURE_H_ */
