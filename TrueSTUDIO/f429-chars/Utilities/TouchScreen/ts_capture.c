/*
 * ts_capture.c
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 */

#include "stm32f429i_discovery_lcd.h"
#include "ts_capture.h"

static int32_t m_finished = -1, m_touch_id = -1;
static uint32_t m_last_touch_tick = 0;

static void DrawStroke(int32_t touch_id) {
	BSP_LCD_DrawLine(TS_Capture_TouchesX[touch_id - 1],
			TS_Capture_TouchesY[touch_id - 1], TS_Capture_TouchesX[touch_id],
			TS_Capture_TouchesY[touch_id]);
}

int8_t TS_Capture_SaveTouch(const TS_StateTypeDef *TsState) {
	if (m_touch_id == TS_CAPTURE_CACHE_SIZE - 1) {
		return 1;  // full cache
	}
	uint32_t tick = HAL_GetTick();
	if (!TsState->TouchDetected) {
		if (tick - m_last_touch_tick > TS_CAPTURE_NO_TOUCH_TICK) {
			m_finished = m_touch_id;
		}
		return -1;
	}
	m_last_touch_tick = tick;
	m_touch_id++;
	TS_Capture_TouchesX[m_touch_id] = TsState->X;
	TS_Capture_TouchesY[m_touch_id] = TsState->Y;
	return 0;
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
}
