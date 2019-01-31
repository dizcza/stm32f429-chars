/*
 * char_patterns.c
 *
 *  Created on: Jan 31, 2019
 *      Author: dizcza
 */

#include "stm32f429i_discovery_lcd.h"
#include "char_patterns.h"

//static const int32_t m_alphabet_size2 = sizeof(ALPHABET) / sizeof(ALPHABET[0]);


void Chars_DrawPatterns() {
	int32_t pattern_id;
	for (pattern_id = 1; pattern_id < 30; pattern_id++) {
		const uint16_t** curr = (const uint16_t**) PATTERN_COORDS[pattern_id];
		const uint16_t** prev =
				(const uint16_t**) PATTERN_COORDS[pattern_id - 1];
//		BSP_LCD_DrawLine((uint16_t) prev[1], (uint16_t) prev[0], (uint16_t) curr[1], (uint16_t) curr[0]);
	}
}
