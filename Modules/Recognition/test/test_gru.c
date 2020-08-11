/*
 * test_gru.c
 *
 *  Created on: Aug 11, 2020
 *      Author: dizcza
 */


#include <stdlib.h>
#include "tests_recognition.h"
#include "grunet.h"


void Test_GRU(ai_handle network) {

	ai_buffer ai_input[AI_GRUNET_IN_NUM] = AI_GRUNET_IN;
	ai_buffer ai_output[AI_GRUNET_OUT_NUM] = AI_GRUNET_OUT;

	ai_float input_data[AI_GRUNET_IN_1_SIZE];
	ai_float output_data[AI_GRUNET_OUT_1_SIZE];

	// fill the input buffer with random data
	int i;
	for (i = 0; i < AI_GRUNET_IN_1_SIZE; i++) {
		input_data[i] = rand() * 1.f / RAND_MAX;
	}

	ai_input[0].n_batches = 1;
	ai_input[0].data = AI_HANDLE_PTR((const void*) input_data);
	ai_output[0].n_batches = 1;
	ai_output[0].data = AI_HANDLE_PTR((void*) output_data);

	ai_i32 n_batches_run;
	n_batches_run = ai_grunet_run(network, &ai_input[0], &ai_output[0]);
	assert_param(n_batches_run == 1);

	ai_error err = ai_grunet_get_error(network);
	assert_param(err.type == AI_ERROR_NONE);
	assert_param(err.code == AI_ERROR_CODE_NONE);

	float* proba_softmax = (float*) ai_output[0].data;
	float sum_proba = 0.f;
	for (i = 0; i < AI_GRUNET_OUT_1_SIZE; i++) {
		sum_proba += proba_softmax[i];
	}

	// the output of a softmax layer must sum up to 1.0
	assert_isclose(sum_proba, 1.0f);
}
