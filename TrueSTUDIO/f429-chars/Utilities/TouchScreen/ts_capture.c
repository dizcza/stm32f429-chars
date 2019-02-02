/*
 * ts_capture.c
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 */

#include "stm32f429i_discovery_lcd.h"
#include "ts_capture.h"
#include "char_patterns.h"
#include "arm_math.h"

typedef struct TS_BBox {
	uint16_t xmin, ymin, xmax, ymax;
} TS_BBox;

/* Raw touches */
static uint16_t m_touches_x[TS_CAPTURE_CACHE_SIZE];
static uint16_t m_touches_y[TS_CAPTURE_CACHE_SIZE];

/* Reduced temp buffer */
static uint16_t m_touches_reduced_x[PATTERN_SIZE];
static uint16_t m_touches_reduced_y[PATTERN_SIZE];

static int32_t m_finished = -1, m_touch_id = -1;
static uint32_t m_last_touch_tick = 0;

static void GetBBox(uint16_t* bufferX, uint16_t* bufferY, uint32_t size,
		TS_BBox* box) {
	uint16_t xmin = UINT16_MAX, xmax = 0, ymin = UINT16_MAX, ymax = 0;
	uint16_t i, x, y;
	for (i = 0; i < size; i++) {
		x = bufferX[i];
		y = bufferY[i];
		if (x < xmin) {
			xmin = x;
		}
		if (x > xmax) {
			xmax = x;
		}
		if (y < ymin) {
			ymin = y;
		}
		if (y > ymax) {
			ymax = y;
		}
	}
	box->xmin = xmin;
	box->ymin = ymin;
	box->xmax = xmax;
	box->ymax = ymax;
}

static void ReduceTouches() {
	// reduce to one size
	const float step = (m_touch_id + 1) * 1.0f / PATTERN_SIZE;
	float accumulated = 0.f;
	int32_t id_raw, id_processed;
	for (id_processed = 0; id_processed < PATTERN_SIZE; id_processed++) {
		id_raw = (int32_t) accumulated;
		if (id_raw > m_touch_id) {
			id_raw = m_touch_id;
		}
		m_touches_reduced_x[id_processed] = m_touches_x[id_raw];
		m_touches_reduced_y[id_processed] = m_touches_y[id_raw];
		accumulated += step;
	}
}

static void Normalize(uint16_t* bufferX, uint16_t* bufferY, uint32_t size,
		TS_BBox* box, DTW_Pattern* sample) {
	uint16_t width = box->xmax - box->xmin;
	uint16_t height = box->ymax - box->ymin;
	if (width / height > ASPECT_RATIO_MAX) {
		// height is too small
		height = width;
	} else if (height / width > ASPECT_RATIO_MAX) {
		// width is too small
		width = height;
	}
	const float32_t xc = 0.5f * (box->xmin + box->xmax);
	const float32_t yc = 0.5f * (box->ymin + box->ymax);
	const float32_t x_scale = 1.f / width;
	const float32_t y_scale = 1.f / height;
	uint32_t i;
	for (i = 0; i < size; i++) {
		sample->xcoords[i] = 0.5f + (bufferX[i] - xc) * x_scale;
		sample->ycoords[i] = 0.5f + (bufferY[i] - yc) * y_scale;
	}
	sample->size = size;
}

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

void TS_Dump(DTW_Pattern* sample) {
	if (m_touch_id < 1) {
		return;  // not enough points
	}
	uint16_t* buffer_x = m_touches_x;
	uint16_t* buffer_y = m_touches_y;
	uint32_t n_touches = (uint32_t) (m_touch_id + 1);
	if (n_touches > PATTERN_SIZE) {
		// reduce to one size
		ReduceTouches();
		buffer_x = m_touches_reduced_x;
		buffer_y = m_touches_reduced_y;
		n_touches = PATTERN_SIZE;
	}
	TS_BBox box;
	GetBBox(buffer_x, buffer_y, n_touches, &box);
	Normalize(buffer_x, buffer_y, n_touches, &box, sample);
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
