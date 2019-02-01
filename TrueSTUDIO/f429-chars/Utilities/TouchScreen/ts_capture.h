/*
 * ts_capture.h
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 */

#ifndef TOUCHSCREEN_TS_CAPTURE_H_
#define TOUCHSCREEN_TS_CAPTURE_H_

#define TS_CAPTURE_BUFFER_SIZE 1000

#include "stm32f429i_discovery_ts.h"
#include "dtw.h"


int8_t TS_SaveTouch(const TS_StateTypeDef *TsState);
void TS_DrawLastStroke();
void TS_DrawAllStrokes();
void TS_Dump(DTW_Pattern* pattern);
int8_t TS_IsEmpty();
void TS_Reset();

#endif /* TOUCHSCREEN_TS_CAPTURE_H_ */
