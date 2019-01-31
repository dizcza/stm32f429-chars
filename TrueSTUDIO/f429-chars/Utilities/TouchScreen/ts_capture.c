/*
 * ts_capture.c
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 */

#include "ts_capture.h"
#include "stm32f429i_discovery_lcd.h"

/* Raw touches */
static uint16_t m_touches[TS_CAPTURE_BUFFER_SIZE][2];
static int32_t m_touch_id = -1;

int8_t TS_SaveTouch(const TS_StateTypeDef *TsState) {
	if (!TsState->TouchDetected || m_touch_id >= TS_CAPTURE_BUFFER_SIZE) {
		return 1;
	}
	m_touch_id++;
	m_touches[m_touch_id][0] = TsState->Y;
	m_touches[m_touch_id][1] = TsState->X;
	return 0;
}

void DrawStroke(int32_t touch_id) {
	uint16_t* prev = m_touches[touch_id - 1];
	uint16_t* curr = m_touches[touch_id];
	BSP_LCD_DrawLine(prev[1], prev[0], curr[1], curr[0]);
}

void TS_DrawLastStroke() {
	if (m_touch_id > 0) {
		DrawStroke(m_touch_id);
	}
}

void TS_DrawAllStrokes() {
	int32_t touch_id;
	for (touch_id = 1; touch_id <= m_touch_id; touch_id++) {
		DrawStroke(touch_id);
	}
}

int32_t TS_Dump(uint16_t buffer[][2], uint32_t bufferSize) {
	if (m_touch_id < 1) {
		return 0;  // not enough points
	}
	if (m_touch_id < bufferSize) {
		int32_t i;
		for (i = 0; i <= m_touch_id; i++) {
			buffer[i][0] = m_touches[i][0];
			buffer[i][1] = m_touches[i][1];
		}
		return m_touch_id + 1;
	} else {
		const float step = (m_touch_id + 1) * 1.0f / bufferSize;
		float accumulated = 0.f;
		int32_t id_raw, id_processed;
		for (id_processed = 0; id_processed < bufferSize; id_processed++) {
			id_raw = (int32_t) accumulated;
			if (id_raw > m_touch_id) {
				id_raw = m_touch_id;
			}
			buffer[id_processed][0] = m_touches[id_raw][0];
			buffer[id_processed][1] = m_touches[id_raw][1];
			accumulated += step;
		}
	}
	return bufferSize;
}

int8_t TS_IsEmpty() {
	return m_touch_id == -1;
}

void TS_Reset() {
	m_touch_id = -1;
}
