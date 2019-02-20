/*
 * preprocess.h
 *
 *  Created on: Feb 20, 2019
 *      Author: dizcza
 */

#ifndef PREPROCESS_PREPROCESS_H_
#define PREPROCESS_PREPROCESS_H_

#include "dtw.h"

void Preprocess_MakePattern(uint16_t* bufferX, uint16_t* bufferY, uint32_t bufferSize, DTW_Pattern* sample);
void Preprocess_CorrectSlant(uint16_t* bufferX, uint16_t* bufferY, uint32_t size);

#endif /* PREPROCESS_PREPROCESS_H_ */
