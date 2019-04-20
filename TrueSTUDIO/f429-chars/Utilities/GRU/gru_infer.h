/*
 * gru_infer.h
 *
 *  Created on: Apr 18, 2019
 *      Author: dizcza
 */

#ifndef GRU_GRU_INFER_H_
#define GRU_GRU_INFER_H_

#include <char_pattern.h>
#include "grunet.h"

ai_error GRU_Init();
ai_error GRU_Infer(const CharPattern* sample, CharPattern_PredictedInfo* resultInfo);
void GRU_LogError(ai_error err, char* message);

#endif /* GRU_GRU_INFER_H_ */
