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

#include "stm32f429i_discovery_ts.h"
#include "dtw.h"

typedef enum {
	EMPTY = 0,
	DIRTY,  // has some values
	FULL
} TS_CacheState;

int8_t TS_SaveTouch(const TS_StateTypeDef *TsState);
void TS_DrawLastStroke();
void TS_DrawAllStrokes();
void TS_Dump(DTW_Pattern* pattern);
TS_CacheState TS_GetCacheState();
void TS_Reset();

#endif /* TOUCHSCREEN_TS_CAPTURE_H_ */
