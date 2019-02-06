/*
 * ts_capture.c
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 */

#include "stm32f429i_discovery_lcd.h"
#include "ts_capture.h"
#include "OnlineMean/onlinemean.h"

static int32_t m_finished = -1, m_touch_id = -1;
static uint32_t m_last_touch_tick = 0;

/* Debug information */
static uint32_t m_calls = 0;  // num of save touch calls
static OnlineMean m_touch_duration;

static void DrawStroke(int32_t touch_id) {
	BSP_LCD_DrawLine(TS_Capture_TouchesX[touch_id - 1],
			TS_Capture_TouchesY[touch_id - 1], TS_Capture_TouchesX[touch_id],
			TS_Capture_TouchesY[touch_id]);
}

void TS_Capture_Init() {
	TS_Capture_Reset();
}

int8_t TS_Capture_SaveTouch(const TS_StateTypeDef *TsState) {
	m_calls++;
	if (m_touch_id == TS_CAPTURE_CACHE_SIZE - 1) {
		return 1;  // full cache
	}
	uint32_t tick = HAL_GetTick();
	uint32_t duration = tick - m_last_touch_tick;
	if (!TsState->TouchDetected) {
		if (duration > TS_CAPTURE_NO_TOUCH_TICK) {
			m_finished = m_touch_id;
		}
		return -1;
	}
	m_last_touch_tick = tick;
	m_touch_id++;
	TS_Capture_TouchesX[m_touch_id] = TsState->X;
	TS_Capture_TouchesY[m_touch_id] = TsState->Y;
	if (m_touch_id > 0) {
		OnlineMean_Update(&m_touch_duration, duration);
	}
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

void TS_Capture_PrintInfoLCD(uint16_t startLine) {
	uint32_t n_touches = TS_Capture_GetNumOfTouches();
	uint8_t message[20];
	sprintf((char*) message, "[TS] %lu calls", m_calls);
	BSP_LCD_DisplayStringAtLine(startLine++, message);
	sprintf((char*) message, "[TS] %lu touches", n_touches);
	BSP_LCD_DisplayStringAtLine(startLine++, message);
	float std = OnlineMean_GetStd(&m_touch_duration);
	sprintf((char*) message, "[TS] %ld +- %.1f ms", (int32_t) m_touch_duration.mean, std);
	BSP_LCD_DisplayStringAtLine(startLine++, message);
}

void TS_Capture_Reset() {
	m_touch_id = -1;
	m_finished = -1;
	m_calls = 0;
	m_last_touch_tick = 0;
	OnlineMean_Reset(&m_touch_duration);
}
