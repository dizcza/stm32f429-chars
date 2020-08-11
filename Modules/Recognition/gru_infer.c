/*
 * gru_infer.c
 *
 *  Created on: Apr 18, 2019
 *      Author: dizcza
 */

#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stm32f429i_discovery_lcd.h"
#include "gru_infer.h"
#include "grunet_data.h"
#include "Log/lcd_log.h"

#define GRU_SINGLE_PASS_BATCH_SIZE 1

static ai_float m_input_data[AI_GRUNET_IN_1_SIZE];
static ai_float m_output_data[AI_GRUNET_OUT_1_SIZE];

static ai_handle m_network = AI_HANDLE_NULL;

static ai_buffer ai_input[AI_GRUNET_IN_NUM] = AI_GRUNET_IN;
static ai_buffer ai_output[AI_GRUNET_OUT_NUM] = AI_GRUNET_OUT;


/* Global buffer to handle the activations data buffer - R/W data */
AI_ALIGNED(4)
static ai_u8 m_activations[AI_GRUNET_DATA_ACTIVATIONS_SIZE];

ai_error GRU_Init() {
	ai_error err;

    /* 1 - Specific AI data structure to provide the references of the
     * activation/working memory chunk and the weights/bias parameters */
    const ai_network_params params = {
            AI_GRUNET_DATA_WEIGHTS(ai_grunet_data_weights_get()),
            AI_GRUNET_DATA_ACTIVATIONS(m_activations)
    };

	err = ai_grunet_create(&m_network, AI_GRUNET_DATA_CONFIG);
	if (err.type != AI_ERROR_NONE) {
		GRU_LogError(err, "ai_grunet_create");
		ai_grunet_destroy(m_network);
		return err;
	}

	if (!ai_grunet_init(m_network, &params)) {
		err = ai_grunet_get_error(m_network);
		GRU_LogError(err, "ai_grunet_init");
		ai_grunet_destroy(m_network);
		m_network = AI_HANDLE_PTR(NULL);
	}

	return err;
}

ai_handle GRU_NetwokHandle() {
	return m_network;
}

ai_error GRU_GetError() {
	return ai_grunet_get_error(m_network);
}

void GRU_LogError(ai_error err, char* title) {
	if (title) {
		LCD_ErrLog("AI error (%s) - type=%d code=%d\n", title, err.type, err.code);
	} else {
		LCD_ErrLog("AI error - type=%d code=%d\n", err.type, err.code);
	}
}

ai_error GRU_Infer(const CharPattern* sample,
		CharPattern_PredictedInfo* resultInfo) {
	assert_param(2 * sample->size == AI_GRUNET_IN_1_SIZE);
	uint32_t i, start;
	start = HAL_GetTick();
	for (i = 0; i < sample->size; i++) {
		m_input_data[2 * i] = sample->xcoords[i];
		m_input_data[2 * i + 1] = sample->ycoords[i];
	}

	ai_input[0].n_batches = GRU_SINGLE_PASS_BATCH_SIZE;
	ai_input[0].data = AI_HANDLE_PTR((const void*) m_input_data);
	ai_output[0].n_batches = GRU_SINGLE_PASS_BATCH_SIZE;
	ai_output[0].data = AI_HANDLE_PTR((void*) m_output_data);

	ai_grunet_run(m_network, &ai_input[0], &ai_output[0]);
	ai_error err = ai_grunet_get_error(m_network);
	if (err.type != AI_ERROR_NONE) {
		GRU_LogError(err, "ai_grunet_run");
		return err;
	}

	float* proba_softmax = (float*) ai_output[0].data;
	uint32_t argmax = 0;
	for (i = 1; i < AI_GRUNET_OUT_1_SIZE; i++) {
		if (proba_softmax[i] > proba_softmax[argmax]) {
			argmax = i;
		}
	}
	resultInfo->predicted_char = (char) ('a' + argmax);
	resultInfo->confidence = proba_softmax[argmax];
	resultInfo->duration = HAL_GetTick() - start;

	return err;
}

static const char* BufferFormatToStr(uint32_t val) {
	switch (val) {
	case AI_BUFFER_FORMAT_NONE:
		return "AI_BUFFER_FORMAT_NONE";
	case AI_BUFFER_FORMAT_FLOAT:
		return "AI_BUFFER_FORMAT_FLOAT";
	case AI_BUFFER_FORMAT_U8:
		return "AI_BUFFER_FORMAT_U8";
	case AI_BUFFER_FORMAT_Q15:
		return "AI_BUFFER_FORMAT_Q15";
	case AI_BUFFER_FORMAT_Q7:
		return "AI_BUFFER_FORMAT_Q7";
	default:
		return "UNKNOWN";
	}
}

static ai_u32 GetBufferSize(const ai_buffer* buffer) {
	return buffer->height * buffer->width * buffer->channels;
}

static void PrintLayoutBuffer(const char *msg, const ai_buffer* buffer) {
	printf("%s HWC layout: %d,%d,%ld (s:%ld f:%s)\n", msg, buffer->height,
			buffer->width, buffer->channels, GetBufferSize(buffer),
			BufferFormatToStr(buffer->format));
}

void GRU_LogNetworkInfo() {
	ai_network_report report;
	if (ai_grunet_get_info(m_network, &report)) {
		BSP_LCD_SelectLayer(LCD_BACKGROUND_LAYER);
		LCD_LineColor = LCD_COLOR_GREEN;
		printf(">>> Network configuration\n");
		printf(" Model name: %s\n", report.model_name);
		printf(" Model datetime: %s\n", report.model_datetime);
		printf(">>> Network info\n");
		printf("  nodes: %ld\n", report.n_nodes);
		printf("  complexity: %ld MACC\n", report.n_macc);
		printf("  activation: %ld bytes\n", GetBufferSize(&report.activations));
		printf("  params: %ld bytes\n", GetBufferSize(&report.params));
		printf("  inputs/outputs: %u/%u\n", report.n_inputs, report.n_outputs);
		PrintLayoutBuffer("  IN tensor format:", report.inputs);
		PrintLayoutBuffer("  OUT tensor format:", report.outputs);
		LCD_LineColor = LCD_COLOR_DARKBLUE;
		BSP_LCD_SelectLayer(LCD_FOREGROUND_LAYER);
	} else {
		ai_error err = ai_grunet_get_error(m_network);
		GRU_LogError(err, "ai_grunet_get_info");
	}
}
