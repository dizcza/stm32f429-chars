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
	TS_CAPTURE_SAVED = 0,  // success
	TS_CAPTURE_FULL_CACHE,
	TS_CAPTURE_NO_TOUCH,
	TS_CAPTURE_DUPLICATE
} TS_Capture_SaveTouchDef;

/* Raw touches */
uint16_t TS_Capture_TouchesX[TS_CAPTURE_CACHE_SIZE];
uint16_t TS_Capture_TouchesY[TS_CAPTURE_CACHE_SIZE];

void TS_Capture_Init();
TS_Capture_SaveTouchDef TS_Capture_SaveTouch(const TS_StateTypeDef *TsState);
void TS_Capture_DrawLastStroke();
void TS_Capture_DrawAllStrokes();
uint32_t TS_Capture_GetNumOfTouches();
void TS_Capture_Reset();

/* Helpers */
void TS_Capture_PrintInfoLCD(uint16_t startLine);

#endif /* TOUCHSCREEN_TS_CAPTURE_H_ */
