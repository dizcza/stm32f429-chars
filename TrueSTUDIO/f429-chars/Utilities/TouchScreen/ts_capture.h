/*
 * ts_capture.h
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 */

#ifndef TOUCHSCREEN_TS_CAPTURE_H_
#define TOUCHSCREEN_TS_CAPTURE_H_

#include "stm32f429i_discovery_ts.h"

#define TS_CAPTURE_BUFFER_SIZE 1000

int8_t TS_SaveTouch(const TS_StateTypeDef *TsState);
void TS_DrawLastStroke();
void TS_DrawAllStrokes();
int32_t TS_Dump(uint16_t buffer[][2], uint32_t bufferSize);
int8_t TS_IsEmpty();
void TS_Reset();

#endif /* TOUCHSCREEN_TS_CAPTURE_H_ */
