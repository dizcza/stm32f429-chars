/*
 * ts_capture.c
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 */

#include "stm32f429i_discovery_lcd.h"
#include "ts_capture.h"
#include "char_patterns.h"

/* Raw touches */
static uint16_t m_touches_x[TS_CAPTURE_CACHE_SIZE];
static uint16_t m_touches_y[TS_CAPTURE_CACHE_SIZE];
static int32_t m_finished = -1, m_touch_id = -1;
static uint32_t m_last_touch_tick = 0;

int8_t TS_SaveTouch(const TS_StateTypeDef *TsState) {
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
	m_touches_x[m_touch_id] = TsState->X;
	m_touches_y[m_touch_id] = TsState->Y;
	return 0;
}

void DrawStroke(int32_t touch_id) {
	BSP_LCD_DrawLine(m_touches_x[touch_id - 1], m_touches_y[touch_id - 1],
			m_touches_x[touch_id], m_touches_y[touch_id]);
}

void TS_DrawLastStroke() {
	if (m_touch_id > 0 && m_touch_id != m_finished + 1) {
		// avoid connecting two subsequent strokes
		DrawStroke(m_touch_id);
	}
}

void TS_DrawAllStrokes() {
	int32_t touch_id;
	for (touch_id = 1; touch_id <= m_touch_id; touch_id++) {
		DrawStroke(touch_id);
	}
}

void TS_Dump(DTW_Pattern* pattern) {
	if (m_touch_id < 1) {
		return;  // not enough points
	}
	if (m_touch_id < PATTERN_SIZE) {
		int32_t i;
		for (i = 0; i <= m_touch_id; i++) {
			pattern->xcoords[i] = m_touches_x[i];
			pattern->ycoords[i] = m_touches_y[i];
		}
	} else {
		const float step = (m_touch_id + 1) * 1.0f / PATTERN_SIZE;
		float accumulated = 0.f;
		int32_t id_raw, id_processed;
		for (id_processed = 0; id_processed < PATTERN_SIZE; id_processed++) {
			id_raw = (int32_t) accumulated;
			if (id_raw > m_touch_id) {
				id_raw = m_touch_id;
			}
			pattern->xcoords[id_processed] = m_touches_x[id_raw];
			pattern->ycoords[id_processed] = m_touches_y[id_raw];
			accumulated += step;
		}
	}
}

TS_CacheState TS_GetCacheState() {
	switch (m_touch_id) {
	case -1:
		return EMPTY;

	case TS_CAPTURE_CACHE_SIZE - 1:
		return FULL;

	default:
		return DIRTY;
	}
}

void TS_Reset() {
	m_touch_id = -1;
}
