/*
 * dtw_helper.c
 *
 *  Created on: Feb 25, 2019
 *      Author: dizcza
 */

#include <stdio.h>
#include <math.h>
#include "stm32f429i_discovery_lcd.h"
#include "char_pattern.h"
#include "preprocess.h"

typedef struct Point_u16 {
	uint16_t x, y;
} Point_u16;

void CharPattern_PrintResult(const CharPattern_PredictedInfo* resultInfo) {
	uint8_t message[20];
	uint16_t start_line = 0;
	sprintf((char*) message, "You wrote: %c", (char) resultInfo->predicted_char);
	BSP_LCD_DisplayStringAtLine(start_line++, message);
	sprintf((char*) message, "Inference took %lu ms", resultInfo->duration);
	BSP_LCD_DisplayStringAtLine(start_line++, message);
	sprintf((char*) message, "Confidence: %.5f", resultInfo->confidence);
	BSP_LCD_DisplayStringAtLine(start_line++, message);
}

void CharPattern_Draw(const CharPattern* pattern) {
	uint32_t i;
	BBox screen_box = {0.f, 0.f, BSP_LCD_GetXSize(), BSP_LCD_GetYSize() };
	float xc_screen, yc_screen, xc_pattern, yc_pattern, scale_x, scale_y;
	Preprocess_GetBoxCenter(&screen_box, &xc_screen, &yc_screen);
	BBox box_pattern;
	Preprocess_GetBBox(pattern, &box_pattern);
	Preprocess_GetBoxCenter(&box_pattern, &xc_pattern, &yc_pattern);
	Preprocess_GetBoxScale(&box_pattern, &screen_box, &scale_x, &scale_y);
	float x_float, y_float;
	Point_u16 p_curr, p_prev;
	x_float = fmaxf(xc_screen + (pattern->xcoords[0] - xc_pattern) * scale_x, 0);
	y_float = fmaxf(yc_screen + (pattern->ycoords[0] - yc_pattern) * scale_y, 0);
	p_prev.x = (uint16_t) x_float;
	p_prev.y = (uint16_t) y_float;
	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	for (i = 1; i < pattern->size; i++) {
		x_float = fmaxf(xc_screen + (pattern->xcoords[i] - xc_pattern) * scale_x, 0);
		y_float = fmaxf(yc_screen + (pattern->ycoords[i] - yc_pattern) * scale_y, 0);
		p_curr.x = (uint16_t) x_float;
		p_curr.y = (uint16_t) y_float;
		BSP_LCD_DrawLine(p_prev.x, p_prev.y, p_curr.x, p_curr.y);
		p_prev.x = p_curr.x;
		p_prev.y = p_curr.y;
	}
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
}
