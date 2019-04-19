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

#include "Test/tests.h"
#include "Preprocess/preprocess.h"
#include "TouchScreen/ts_capture.h"

void Test_ShearTransformUI() {
	TS_Capture_Init();
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	TS_StateTypeDef ts_state;
	uint32_t tick, last_touch = 0;
	uint8_t message[30];
	sprintf((char*) message, "Test shear transform");
	BSP_LCD_DisplayStringAtLine(0, message);

	while (1) {
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 1) {
			break;
		}
		BSP_TS_GetState(&ts_state);
		TS_Capture_SaveTouch(&ts_state);
		tick = HAL_GetTick();
		if (ts_state.TouchDetected) {
			TS_Capture_DrawLastStroke();
			last_touch = tick;
		} else if (tick - last_touch > 1000) {
			uint32_t n_touches = TS_Capture_GetNumOfTouches();
			if (n_touches > 0) {
				Preprocess_CorrectSlant(TS_Capture_TouchesX, TS_Capture_TouchesY,
						n_touches);
				BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
				TS_Capture_DrawAllStrokes();
				BSP_LCD_SetTextColor(LCD_COLOR_GREEN);

				HAL_Delay(2000);
				TS_Capture_Reset();
				BSP_LCD_Clear(LCD_COLOR_BLACK);
			}
		}
	}

	TS_Capture_Reset();
	BSP_LCD_Clear(LCD_COLOR_BLACK);
}
