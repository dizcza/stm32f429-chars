/*
 * test_shear.c
 *
 *  Created on: Feb 20, 2019
 *      Author: dizcza
 */

#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"

#include "tests.h"
#include "preprocess.h"
#include "TouchScreen/ts_capture.h"

void Test_ShearTransformUI() {
	float xcoords[TS_CAPTURE_CACHE_SIZE];
	float ycoords[TS_CAPTURE_CACHE_SIZE];

	TS_Capture_Init();
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	CharPattern pattern;
	pattern.xcoords = xcoords;
	pattern.ycoords = ycoords;
	TS_StateTypeDef ts_state;
	uint32_t tick, last_touch = 0;
	uint8_t message[30];
	sprintf((char*) message, "Test shear transform");
	BSP_LCD_DisplayStringAtLine(0, message);

	while (1) {
		BSP_TS_GetState(&ts_state);
		TS_Capture_SaveTouch(&ts_state);
		tick = HAL_GetTick();
		if (ts_state.TouchDetected) {
			TS_Capture_DrawLastStroke();
			last_touch = tick;
		} else if (tick - last_touch > 1000) {
			uint32_t n_touches = TS_Capture_GetNumOfTouches();
			if (n_touches > 0) {
				pattern.size = n_touches;
				Preprocess_ToFloat(TS_Capture_TouchesX, TS_Capture_TouchesY, &pattern);
				Preprocess_CorrectSlant(&pattern);
				CharPattern_Draw(&pattern);
				HAL_Delay(2000);
				TS_Capture_Reset();
				BSP_LCD_Clear(LCD_COLOR_BLACK);
			}
		}
	}
}
