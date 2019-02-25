/*
 * dtw_helper.c
 *
 *  Created on: Feb 25, 2019
 *      Author: dizcza
 */

#include "stm32f429i_discovery_lcd.h"
#include "dtw.h"

void DTW_PrintResult(const DTW_ResultInfo* resultInfo) {
	uint8_t message[20];
	uint16_t start_line = 1;
	sprintf((char*) message, "You wrote: %c", (char) resultInfo->predicted_char);
	BSP_LCD_DisplayStringAtLine(0, message);
	sprintf((char*) message, "Duration: %lu ms", resultInfo->duration);
	BSP_LCD_DisplayStringAtLine(start_line++, message);
	sprintf((char*) message, "Distance: %.5f", resultInfo->distance);
	BSP_LCD_DisplayStringAtLine(start_line++, message);
}
