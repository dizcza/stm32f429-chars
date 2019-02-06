/*
 * Welford's Online algorithm to estimate population mean and variance.
 *
 *  Created on: Feb 6, 2019
 *      Author: Danylo Ulianych
 */

#include <math.h>
#include "onlinemean.h"

void OnlineMean_Init(OnlineMean *oMean) {
	OnlineMean_Reset(oMean);
}

void OnlineMean_Update(OnlineMean *oMean, float newValue) {
	oMean->count++;
	if (oMean->count > 1) {
		float delta = newValue - oMean->mean;
		oMean->mean += delta / oMean->count;
		oMean->varsum += delta * (newValue - oMean->mean);
	} else {
		oMean->mean = newValue;
	}
}

float OnlineMean_GetMean(OnlineMean *oMean) {
	return oMean->mean;
}

float OnlineMean_GetStd(OnlineMean *oMean) {
#if UNBIASED_ESTIMATOR
	return sqrt(oMean->varsum / (oMean->count - 1));
#else
	return sqrt(oMean->varsum / oMean->count);
#endif  /* UNBIASED_ESTIMATOR */
}

void OnlineMean_Reset(OnlineMean *oMean) {
	oMean->count = 0;
	oMean->mean = 0.f;
	oMean->varsum = 0.f;
}
