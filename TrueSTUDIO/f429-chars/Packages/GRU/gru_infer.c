/*
 * gru_infer.c
 *
 *  Created on: Apr 18, 2019
 *      Author: dizcza
 */

#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "cmsis_gcc.h"
#include "stm32f429i_discovery_lcd.h"
#include "gru_infer.h"
#include "grunet_data.h"

#define GRU_SINGLE_PASS_BATCH_SIZE 1

static ai_float m_input_data[AI_GRUNET_IN_1_SIZE];
static ai_float m_output_data[AI_GRUNET_OUT_1_SIZE];

ai_handle m_network = AI_HANDLE_NULL;

/* Global buffer to handle the activations data buffer - R/W data */
AI_ALIGNED(4)
static ai_u8 m_activations[AI_GRUNET_DATA_ACTIVATIONS_SIZE];

ai_error GRU_Init() {
	ai_error err;

	err = ai_grunet_create(&m_network, AI_GRUNET_DATA_CONFIG);
	if (err.type != AI_ERROR_NONE) {
		GRU_LogError(err, "ai_grunet_create");
		ai_grunet_destroy(m_network);
		return err;
	}

	ai_buffer params_buffer = AI_GRUNET_DATA_WEIGHTS(ai_grunet_data_weights_get());
	ai_buffer activations_buffer = AI_GRUNET_DATA_ACTIVATIONS(m_activations);
	const ai_network_params params = {params_buffer, activations_buffer};
	if (!ai_grunet_init(m_network, &params)) {
		err = ai_grunet_get_error(m_network);
		GRU_LogError(err, "ai_grunet_init");
		ai_grunet_destroy(m_network);
	}

	return err;
}

void GRU_LogError(ai_error err, char* title) {
	uint16_t start_line = 0U;
	uint8_t message[32];
	if (title) {
		sprintf((char*) message, "E: AI error (%s) - ", title);
	}
	else {
		sprintf((char*) message, "E: AI error - ");
	}
	BSP_LCD_DisplayStringAtLine(start_line++, message);
	printf("%s", message);
	sprintf((char*) message, "type=%d code=%d", err.type, err.code);
	BSP_LCD_DisplayStringAtLine(start_line++, message);
	printf("%s\r\n", message);
}

ai_error GRU_Infer(const CharPattern* sample, CharPattern_PredictedInfo* resultInfo) {
	assert_param(sample->size == PATTERN_SIZE);
	assert_param(2 * PATTERN_SIZE == AI_GRUNET_IN_1_SIZE);
	uint32_t i;
	for (i = 0; i < sample->size; i++) {
		m_input_data[2 * i] = sample->xcoords[i];
		m_input_data[2 * i + 1] = sample->ycoords[i];
	}

	ai_buffer ai_input;
	ai_buffer ai_output;

	ai_input.n_batches = GRU_SINGLE_PASS_BATCH_SIZE;
	ai_input.data = AI_HANDLE_PTR(m_input_data);
	ai_output.n_batches = GRU_SINGLE_PASS_BATCH_SIZE;
	ai_output.data = AI_HANDLE_PTR(m_output_data);

	ai_grunet_run(m_network, &ai_input, &ai_output);
	ai_error err = ai_grunet_get_error(m_network);
	if (err.type != AI_ERROR_NONE) {
		GRU_LogError(err, "ai_grunet_run");
		return err;
	}

	float* output = (float*) ai_output.data;
	uint32_t argmax = 0;
	for (i = 1; i < AI_GRUNET_OUT_1_SIZE; i++) {
		if (output[i] > output[argmax]) {
			argmax = i;
		}
	}
	resultInfo->predicted_char = (char) ('a' + argmax);

	return err;
}

__STATIC_INLINE const char* GRU_aiBufferFormatToStr(uint32_t val)
{
    if (val == AI_BUFFER_FORMAT_NONE)
        return "AI_BUFFER_FORMAT_NONE";
    else if (val == AI_BUFFER_FORMAT_FLOAT)
        return "AI_BUFFER_FORMAT_FLOAT";
    else if (val == AI_BUFFER_FORMAT_U8)
        return "AI_BUFFER_FORMAT_U8";
    else if (val == AI_BUFFER_FORMAT_Q15)
        return "AI_BUFFER_FORMAT_Q15";
    else if (val == AI_BUFFER_FORMAT_Q7)
        return "AI_BUFFER_FORMAT_Q7";
    else
        return "UNKNOWN";
}

static ai_u32 GRU_aiBufferSize(const ai_buffer* buffer)
{
    return buffer->height * buffer->width * buffer->channels;
}

static void GRU_aiPrintLayoutBuffer(const char *msg,
        const ai_buffer* buffer)
{
    printf("%s HWC layout:%d,%d,%ld (s:%ld f:%s)\r\n",
      msg, buffer->height, buffer->width, buffer->channels,
      GRU_aiBufferSize(buffer),
      GRU_aiBufferFormatToStr(buffer->format));
}

static void GRU_aiPrintNetworkInfo(const ai_network_report* report)
{
  printf("Network configuration...\r\n");
  printf(" Model name         : %s\r\n", report->model_name);
  printf(" Model signature    : %s\r\n", report->model_signature);
  printf(" Model datetime     : %s\r\n", report->model_datetime);
  printf(" Compile datetime   : %s\r\n", report->compile_datetime);
  printf(" Runtime revision   : %s (%d.%d.%d)\r\n", report->runtime_revision,
    report->runtime_version.major,
    report->runtime_version.minor,
    report->runtime_version.micro);
  printf(" Tool revision      : %s (%d.%d.%d)\r\n", report->tool_revision,
    report->tool_version.major,
    report->tool_version.minor,
    report->tool_version.micro);
  printf("Network info...\r\n");
  printf("  signature         : 0x%lx\r\n", report->signature);
  printf("  nodes             : %ld\r\n", report->n_nodes);
  printf("  complexity        : %ld MACC\r\n", report->n_macc);
  printf("  activation        : %ld bytes\r\n", GRU_aiBufferSize(&report->activations));
  printf("  weights           : %ld bytes\r\n", GRU_aiBufferSize(&report->weights));
  printf("  inputs/outputs    : %u/%u\r\n", report->n_inputs, report->n_outputs);
  GRU_aiPrintLayoutBuffer("  IN tensor format  :", &report->inputs);
  GRU_aiPrintLayoutBuffer("  OUT tensor format :", &report->outputs);
}



void GRU_LogNetworkInfo() {
	ai_network_report report;
    if (ai_grunet_get_info(m_network, &report)) {
        GRU_aiPrintNetworkInfo(&report);
    } else {
        ai_error err = ai_grunet_get_error(m_network);
        GRU_LogError(err, "ai_grunet_get_info");
    }
}
