/**
  ******************************************************************************
  * @file    grunet.c
  * @author  AST Embedded Analytics Research Platform
  * @date    Sun Oct 30 22:26:01 2022
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */


#include "grunet.h"

#include "ai_platform_interface.h"
#include "ai_math_helpers.h"

#include "core_common.h"
#include "core_convert.h"

#include "layers.h"



/*
#define AI_TOOLS_VERSION_MAJOR 6
#define AI_TOOLS_VERSION_MINOR 0
#define AI_TOOLS_VERSION_MICRO 0
#define AI_TOOLS_VERSION_EXTRA "RC6"

*/

/*
#define AI_TOOLS_API_VERSION_MAJOR 1
#define AI_TOOLS_API_VERSION_MINOR 4
#define AI_TOOLS_API_VERSION_MICRO 0
*/

#undef AI_NET_OBJ_INSTANCE
#define AI_NET_OBJ_INSTANCE g_grunet
 
#undef AI_GRUNET_MODEL_SIGNATURE
#define AI_GRUNET_MODEL_SIGNATURE     "d7c563c864c72046058d029f0fb7aa1d"

#ifndef AI_TOOLS_REVISION_ID
#define AI_TOOLS_REVISION_ID     ""
#endif

#undef AI_TOOLS_DATE_TIME
#define AI_TOOLS_DATE_TIME   "Sun Oct 30 22:26:01 2022"

#undef AI_TOOLS_COMPILE_TIME
#define AI_TOOLS_COMPILE_TIME    __DATE__ " " __TIME__

#undef AI_GRUNET_N_BATCHES
#define AI_GRUNET_N_BATCHES         (1)

/**  Forward network declaration section  *************************************/
AI_STATIC ai_network AI_NET_OBJ_INSTANCE;


/**  Forward network array declarations  **************************************/
AI_STATIC ai_array input_0_output_array;   /* Array #0 */
AI_STATIC ai_array gru_1_output_array;   /* Array #1 */
AI_STATIC ai_array dense_1_output_array;   /* Array #2 */
AI_STATIC ai_array dense_1_nl_output_array;   /* Array #3 */
AI_STATIC ai_array gru_1_kernel_array;   /* Array #4 */
AI_STATIC ai_array gru_1_recurrent_array;   /* Array #5 */
AI_STATIC ai_array gru_1_bias_array;   /* Array #6 */
AI_STATIC ai_array dense_1_weights_array;   /* Array #7 */
AI_STATIC ai_array dense_1_bias_array;   /* Array #8 */
AI_STATIC ai_array gru_1_scratch0_array;   /* Array #9 */


/**  Forward network tensor declarations  *************************************/
AI_STATIC ai_tensor input_0_output;   /* Tensor #0 */
AI_STATIC ai_tensor gru_1_output;   /* Tensor #1 */
AI_STATIC ai_tensor dense_1_output;   /* Tensor #2 */
AI_STATIC ai_tensor dense_1_nl_output;   /* Tensor #3 */
AI_STATIC ai_tensor gru_1_kernel;   /* Tensor #4 */
AI_STATIC ai_tensor gru_1_recurrent;   /* Tensor #5 */
AI_STATIC ai_tensor gru_1_bias;   /* Tensor #6 */
AI_STATIC ai_tensor dense_1_weights;   /* Tensor #7 */
AI_STATIC ai_tensor dense_1_bias;   /* Tensor #8 */
AI_STATIC ai_tensor gru_1_scratch0;   /* Tensor #9 */


/**  Forward network tensor chain declarations  *******************************/
AI_STATIC_CONST ai_tensor_chain gru_1_chain;   /* Chain #0 */
AI_STATIC_CONST ai_tensor_chain dense_1_chain;   /* Chain #1 */
AI_STATIC_CONST ai_tensor_chain dense_1_nl_chain;   /* Chain #2 */


/**  Forward network layer declarations  **************************************/
AI_STATIC ai_layer_gru gru_1_layer; /* Layer #0 */
AI_STATIC ai_layer_dense dense_1_layer; /* Layer #1 */
AI_STATIC ai_layer_nl dense_1_nl_layer; /* Layer #2 */




/**  Array declarations section  **********************************************/
/* Array#0 */
AI_ARRAY_OBJ_DECLARE(
  input_0_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 60, AI_STATIC)

/* Array#1 */
AI_ARRAY_OBJ_DECLARE(
  gru_1_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 100, AI_STATIC)

/* Array#2 */
AI_ARRAY_OBJ_DECLARE(
  dense_1_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 26, AI_STATIC)

/* Array#3 */
AI_ARRAY_OBJ_DECLARE(
  dense_1_nl_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 26, AI_STATIC)

/* Array#4 */
AI_ARRAY_OBJ_DECLARE(
  gru_1_kernel_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 600, AI_STATIC)

/* Array#5 */
AI_ARRAY_OBJ_DECLARE(
  gru_1_recurrent_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 30000, AI_STATIC)

/* Array#6 */
AI_ARRAY_OBJ_DECLARE(
  gru_1_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 300, AI_STATIC)

/* Array#7 */
AI_ARRAY_OBJ_DECLARE(
  dense_1_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 2600, AI_STATIC)

/* Array#8 */
AI_ARRAY_OBJ_DECLARE(
  dense_1_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 26, AI_STATIC)

/* Array#9 */
AI_ARRAY_OBJ_DECLARE(
  gru_1_scratch0_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 600, AI_STATIC)

/**  Tensor declarations section  *********************************************/
/* Tensor #0 */
AI_TENSOR_OBJ_DECLARE(
  input_0_output, AI_STATIC,
  0, 0x0,
  AI_SHAPE_INIT(4, 1, 2, 1, 30), AI_STRIDE_INIT(4, 4, 4, 8, 8),
  1, &input_0_output_array, NULL)

/* Tensor #1 */
AI_TENSOR_OBJ_DECLARE(
  gru_1_output, AI_STATIC,
  1, 0x0,
  AI_SHAPE_INIT(4, 1, 100, 1, 1), AI_STRIDE_INIT(4, 4, 4, 400, 400),
  1, &gru_1_output_array, NULL)

/* Tensor #2 */
AI_TENSOR_OBJ_DECLARE(
  dense_1_output, AI_STATIC,
  2, 0x0,
  AI_SHAPE_INIT(4, 1, 26, 1, 1), AI_STRIDE_INIT(4, 4, 4, 104, 104),
  1, &dense_1_output_array, NULL)

/* Tensor #3 */
AI_TENSOR_OBJ_DECLARE(
  dense_1_nl_output, AI_STATIC,
  3, 0x0,
  AI_SHAPE_INIT(4, 1, 26, 1, 1), AI_STRIDE_INIT(4, 4, 4, 104, 104),
  1, &dense_1_nl_output_array, NULL)

/* Tensor #4 */
AI_TENSOR_OBJ_DECLARE(
  gru_1_kernel, AI_STATIC,
  4, 0x0,
  AI_SHAPE_INIT(4, 2, 1, 1, 300), AI_STRIDE_INIT(4, 4, 8, 8, 8),
  1, &gru_1_kernel_array, NULL)

/* Tensor #5 */
AI_TENSOR_OBJ_DECLARE(
  gru_1_recurrent, AI_STATIC,
  5, 0x0,
  AI_SHAPE_INIT(4, 100, 1, 1, 300), AI_STRIDE_INIT(4, 4, 400, 400, 400),
  1, &gru_1_recurrent_array, NULL)

/* Tensor #6 */
AI_TENSOR_OBJ_DECLARE(
  gru_1_bias, AI_STATIC,
  6, 0x0,
  AI_SHAPE_INIT(4, 1, 300, 1, 1), AI_STRIDE_INIT(4, 4, 4, 1200, 1200),
  1, &gru_1_bias_array, NULL)

/* Tensor #7 */
AI_TENSOR_OBJ_DECLARE(
  dense_1_weights, AI_STATIC,
  7, 0x0,
  AI_SHAPE_INIT(4, 100, 26, 1, 1), AI_STRIDE_INIT(4, 4, 400, 10400, 10400),
  1, &dense_1_weights_array, NULL)

/* Tensor #8 */
AI_TENSOR_OBJ_DECLARE(
  dense_1_bias, AI_STATIC,
  8, 0x0,
  AI_SHAPE_INIT(4, 1, 26, 1, 1), AI_STRIDE_INIT(4, 4, 4, 104, 104),
  1, &dense_1_bias_array, NULL)

/* Tensor #9 */
AI_TENSOR_OBJ_DECLARE(
  gru_1_scratch0, AI_STATIC,
  9, 0x0,
  AI_SHAPE_INIT(4, 1, 600, 1, 1), AI_STRIDE_INIT(4, 4, 4, 2400, 2400),
  1, &gru_1_scratch0_array, NULL)



/**  Layer declarations section  **********************************************/


AI_TENSOR_CHAIN_OBJ_DECLARE(
  gru_1_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &input_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gru_1_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 5, &gru_1_kernel, &gru_1_recurrent, &gru_1_bias, NULL, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gru_1_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  gru_1_layer, 0,
  GRU_TYPE, 0x0, NULL,
  gru, forward_gru,
  &gru_1_chain,
  &AI_NET_OBJ_INSTANCE, &dense_1_layer, AI_STATIC, 
  .n_units = 100, 
  .activation_nl = nl_func_tanh_array_f32, 
  .go_backwards = false, 
  .reverse_seq = false, 
  .reset_after = false, 
  .recurrent_nl = nl_func_hard_sigmoid_array_f32, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  dense_1_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gru_1_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_1_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &dense_1_weights, &dense_1_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  dense_1_layer, 1,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense,
  &dense_1_chain,
  &AI_NET_OBJ_INSTANCE, &dense_1_nl_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  dense_1_nl_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_1_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_1_nl_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  dense_1_nl_layer, 1,
  NL_TYPE, 0x0, NULL,
  nl, forward_sm,
  &dense_1_nl_chain,
  &AI_NET_OBJ_INSTANCE, &dense_1_nl_layer, AI_STATIC, 
  .nl_params = NULL, 
)


AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_OBJ_INIT(AI_BUFFER_FORMAT_U8,
                     1, 1, 134104, 1,
                     NULL),
  AI_BUFFER_OBJ_INIT(AI_BUFFER_FORMAT_U8,
                     1, 1, 2800, 1,
                     NULL),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_GRUNET_IN_NUM, &input_0_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_GRUNET_OUT_NUM, &dense_1_nl_output),
  &gru_1_layer, 0, NULL)



AI_DECLARE_STATIC
ai_bool grunet_configure_activations(
  ai_network* net_ctx, const ai_buffer* activation_buffer)
{
  AI_ASSERT(net_ctx &&  activation_buffer && activation_buffer->data)

  ai_ptr activations = AI_PTR(AI_PTR_ALIGN(activation_buffer->data, 4));
  AI_ASSERT(activations)
  AI_UNUSED(net_ctx)

  {
    /* Updating activations (byte) offsets */
    input_0_output_array.data = AI_PTR(NULL);
    input_0_output_array.data_start = AI_PTR(NULL);
    gru_1_output_array.data = AI_PTR(activations + 2400);
    gru_1_output_array.data_start = AI_PTR(activations + 2400);
    dense_1_output_array.data = AI_PTR(activations + 0);
    dense_1_output_array.data_start = AI_PTR(activations + 0);
    dense_1_nl_output_array.data = AI_PTR(NULL);
    dense_1_nl_output_array.data_start = AI_PTR(NULL);
    gru_1_scratch0_array.data = AI_PTR(activations + 0);
    gru_1_scratch0_array.data_start = AI_PTR(activations + 0);
    
  }
  return true;
}



AI_DECLARE_STATIC
ai_bool grunet_configure_weights(
  ai_network* net_ctx, const ai_buffer* weights_buffer)
{
  AI_ASSERT(net_ctx &&  weights_buffer && weights_buffer->data)

  ai_ptr weights = AI_PTR(weights_buffer->data);
  AI_ASSERT(weights)
  AI_UNUSED(net_ctx)

  {
    /* Updating weights (byte) offsets */
    
    gru_1_kernel_array.format |= AI_FMT_FLAG_CONST;
    gru_1_kernel_array.data = AI_PTR(weights + 0);
    gru_1_kernel_array.data_start = AI_PTR(weights + 0);
    gru_1_recurrent_array.format |= AI_FMT_FLAG_CONST;
    gru_1_recurrent_array.data = AI_PTR(weights + 2400);
    gru_1_recurrent_array.data_start = AI_PTR(weights + 2400);
    gru_1_bias_array.format |= AI_FMT_FLAG_CONST;
    gru_1_bias_array.data = AI_PTR(weights + 122400);
    gru_1_bias_array.data_start = AI_PTR(weights + 122400);
    dense_1_weights_array.format |= AI_FMT_FLAG_CONST;
    dense_1_weights_array.data = AI_PTR(weights + 123600);
    dense_1_weights_array.data_start = AI_PTR(weights + 123600);
    dense_1_bias_array.format |= AI_FMT_FLAG_CONST;
    dense_1_bias_array.data = AI_PTR(weights + 134000);
    dense_1_bias_array.data_start = AI_PTR(weights + 134000);
  }

  return true;
}


/**  PUBLIC APIs SECTION  *****************************************************/

AI_API_ENTRY
ai_bool ai_grunet_get_info(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if ( report && net_ctx )
  {
    ai_network_report r = {
      .model_name        = AI_GRUNET_MODEL_NAME,
      .model_signature   = AI_GRUNET_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = AI_STRUCT_INIT,

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 927016,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .activations       = AI_STRUCT_INIT,
      .params            = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0x0,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }

  return false;
}

AI_API_ENTRY
ai_error ai_grunet_get_error(ai_handle network)
{
  return ai_platform_network_get_error(network);
}

AI_API_ENTRY
ai_error ai_grunet_create(
  ai_handle* network, const ai_buffer* network_config)
{
  return ai_platform_network_create(
    network, network_config, 
    &AI_NET_OBJ_INSTANCE,
    AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR, AI_TOOLS_API_VERSION_MICRO);
}

AI_API_ENTRY
ai_handle ai_grunet_destroy(ai_handle network)
{
  return ai_platform_network_destroy(network);
}

AI_API_ENTRY
ai_bool ai_grunet_init(
  ai_handle network, const ai_network_params* params)
{
  ai_network* net_ctx = ai_platform_network_init(network, params);
  if ( !net_ctx ) return false;

  ai_bool ok = true;
  ok &= grunet_configure_weights(net_ctx, &params->params);
  ok &= grunet_configure_activations(net_ctx, &params->activations);

  ok &= ai_platform_network_post_init(network);

  return ok;
}


AI_API_ENTRY
ai_i32 ai_grunet_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output)
{
  return ai_platform_network_process(network, input, output);
}

AI_API_ENTRY
ai_i32 ai_grunet_forward(ai_handle network, const ai_buffer* input)
{
  return ai_platform_network_process(network, input, NULL);
}



#undef AI_GRUNET_MODEL_SIGNATURE
#undef AI_NET_OBJ_INSTANCE
#undef AI_TOOLS_DATE_TIME
#undef AI_TOOLS_COMPILE_TIME

