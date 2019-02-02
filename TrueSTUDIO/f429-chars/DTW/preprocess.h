/*
 * preprocess.h
 *
 *  Created on: Feb 2, 2019
 *      Author: dizcza
 */

#ifndef PREPROCESS_H_
#define PREPROCESS_H_

#include "dtw.h"

void DTW_Preprocess(uint16_t* bufferX, uint16_t* bufferY, uint32_t bufferSize, DTW_Pattern* sample);

#endif /* PREPROCESS_H_ */
