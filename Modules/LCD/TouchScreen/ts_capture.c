/*
 * ts_capture.c
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 */

#include <stdio.h>
#include "stm32f429i_discovery_lcd.h"
#include "ts_capture.h"

static int32_t m_finished = -1;  // previous stroke last point
static int32_t m_touch_id = -1;  // current touch id
static uint32_t m_last_touch_tick = 0;

uint16_t TS_Capture_TouchesX[TS_CAPTURE_CACHE_SIZE];
uint16_t TS_Capture_TouchesY[TS_CAPTURE_CACHE_SIZE];

/* Debug information */
static uint32_t m_calls = 0;  // num of save touch calls

static void DrawStroke(int32_t touch_id) {
	BSP_LCD_DrawLine(TS_Capture_TouchesX[touch_id - 1],
			TS_Capture_TouchesY[touch_id - 1], TS_Capture_TouchesX[touch_id],
			TS_Capture_TouchesY[touch_id]);
}

static uint8_t IsDuplicate(const TS_StateTypeDef *TsState) {
	return (m_touch_id >= 0) && (TS_Capture_TouchesX[m_touch_id] == TsState->X)
			&& (TS_Capture_TouchesY[m_touch_id] == TsState->Y);
}

void TS_Capture_Init() {
	TS_Capture_Reset();
}

TS_Capture_SaveTouchDef TS_Capture_SaveTouch(const TS_StateTypeDef *TsState) {
	m_calls++;
	if (m_touch_id == TS_CAPTURE_CACHE_SIZE - 1) {
		return TS_CAPTURE_FULL_CACHE;
	}
	uint32_t tick = HAL_GetTick();
	uint32_t duration = tick - m_last_touch_tick;
	if (!TsState->TouchDetected) {
		if (duration > TS_CAPTURE_NO_TOUCH_TICK) {
			m_finished = m_touch_id;
		}
		return TS_CAPTURE_NO_TOUCH;
	}
	m_last_touch_tick = tick;
	if (IsDuplicate(TsState)) {
		return TS_CAPTURE_DUPLICATE;
	}
	m_touch_id++;
	TS_Capture_TouchesX[m_touch_id] = TsState->X;
	TS_Capture_TouchesY[m_touch_id] = TsState->Y;
	return TS_CAPTURE_SAVED;
}

void TS_Capture_DrawLastStroke() {
	if (m_touch_id > 0 && m_touch_id != m_finished + 1) {
		// avoid connecting two subsequent strokes
		DrawStroke(m_touch_id);
	}
}

void TS_Capture_DrawAllStrokes() {
	int32_t touch_id;
	for (touch_id = 1; touch_id <= m_touch_id; touch_id++) {
		DrawStroke(touch_id);
	}
}

uint32_t TS_Capture_GetNumOfTouches() {
	return (uint32_t) (m_touch_id + 1);
}

void TS_Capture_Reset() {
	m_touch_id = -1;
	m_finished = -1;
	m_calls = 0;
	m_last_touch_tick = 0;
}
