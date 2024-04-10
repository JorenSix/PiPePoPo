/**
 * @file yin_fixed.c
 * @brief This file contains an implementation of a pitch estimation algorithm YIN.
 * 
 * The implementation uses some fixed point operations to speed up the algorithm.
 * 
 **/



#include <stdlib.h>
#include <stdio.h>

#include "yin_fixed.h"
#include "fixed_point.h"

//state information
struct Yin_Fixed {
	
	size_t half_buffer_size;

	float sample_rate;

	float threshold;

	float probability;

	int32_t * yin_buffer_fixed;

	float * yin_buffer_float;
};


Yin_Fixed * yin_fixed_new(float sample_rate, size_t buffer_size) {
	Yin_Fixed * yin = (Yin_Fixed *) malloc(sizeof(Yin_Fixed));
	yin->half_buffer_size = buffer_size / 2;
	yin->sample_rate = sample_rate;
	yin->threshold = 0.15f;
	yin->probability = -1.0;
	yin->yin_buffer_fixed = (int32_t *) malloc(sizeof(int32_t) * yin->half_buffer_size);
	yin->yin_buffer_float = (float   *) malloc(sizeof(float)   * yin->half_buffer_size);
	for (size_t i = 0; i < yin->half_buffer_size; i++) {
		yin->yin_buffer_fixed[i] = 0;
		yin->yin_buffer_float[i] = 0;
	}
	return yin;
}

void yin_fixed_difference(Yin_Fixed * yin,int16_t* buffer){
	size_t index;
	size_t tau;
	int32_t delta;
	for(tau = 0 ; tau < yin->half_buffer_size; tau++){
		for(index = 0; index < yin->half_buffer_size; index++){
			delta = fixed_point_sum(buffer[index], -buffer[index + tau]);
			yin->yin_buffer_fixed[tau] += fixed_point_mul(delta,delta);
		}
		yin->yin_buffer_float[tau]	= fixed_point_to_f(yin->yin_buffer_fixed[tau]);	
	}
}

void yin_fixed_cumulative_mean_normalized_difference(Yin_Fixed * yin){
	size_t tau;
	yin->yin_buffer_float[0] = 1;
	float running_sum = 0;

	for (tau = 1; tau < yin->half_buffer_size; tau++) {
		running_sum += yin->yin_buffer_float[tau];
		//printf("floating fixed %d %f \n" ,tau, yin->yin_buffer_float[tau]);
		yin->yin_buffer_float[tau] = yin->yin_buffer_float[tau] * tau / running_sum;
		//printf("floating fixed %d %f %f\n" ,tau, yin->yin_buffer_float[tau],running_sum);
	}
}

int yin_fixed_absolute_threshold(Yin_Fixed * yin){
	size_t tau;
	// first two positions in yin->yin_buffer_fixed are always 1
	// So start at the third (index 2)
	for (tau = 2; tau < yin->half_buffer_size ; tau++) {
		if (yin->yin_buffer_float[tau] < yin->threshold) {
			while (tau + 1 < yin->half_buffer_size && yin->yin_buffer_float[tau + 1] < yin->yin_buffer_float[tau]) {
				tau++;
			}
			// found tau, exit loop and return
			// store the probability
			// From the YIN paper: The threshold determines the list of
			// candidates admitted to the set, and can be interpreted as the
			// proportion of aperiodic power tolerated
			// within a ëëperiodicíí signal.
			//
			// Since we want the periodicity and and not aperiodicity:
			// periodicity = 1 - aperiodicity
			yin->probability = 1 - yin->yin_buffer_float[tau];
			break;
		}
	}
	// if no pitch found, tau => -1
	if (tau == yin->half_buffer_size || yin->yin_buffer_float[tau] >= yin->threshold) {
		tau = -1;
		yin->probability = 0;
	}
	return tau;
}

float yin_fixed_parabolic_interpolation(Yin_Fixed * yin,size_t tau_estimate) {
	float better_tau;
	size_t x0;
	size_t x2;
	
	if (tau_estimate < 1) {
		x0 = tau_estimate;
	} else {
		x0 = tau_estimate - 1;
	}

	if (tau_estimate + 1 < yin->half_buffer_size) {
		x2 = tau_estimate + 1;
	} else {
		x2 = tau_estimate;
	}

	if (x0 == tau_estimate) {
		if (yin->yin_buffer_fixed[tau_estimate] <= yin->yin_buffer_fixed[x2]) {
			better_tau = tau_estimate;
		} else {
			better_tau = x2;
		}
	} else if (x2 == tau_estimate) {
		if (yin->yin_buffer_fixed[tau_estimate] <= yin->yin_buffer_fixed[x0]) {
			better_tau = tau_estimate;
		} else {
			better_tau = x0;
		}
	} else {
		float s0, s1, s2;
		s0 = fixed_point_to_f(yin->yin_buffer_fixed[x0]);
		s1 = fixed_point_to_f(yin->yin_buffer_fixed[tau_estimate]);
		s2 = fixed_point_to_f(yin->yin_buffer_fixed[x2]);
		// fixed AUBIO implementation, thanks to Karl Helgason:
		// (2.0f * s1 - s2 - s0) was incorrectly multiplied with -1
		better_tau = tau_estimate + (s2 - s0) / (2 * (2 * s1 - s2 - s0));
	}

	return better_tau;
}


float yin_fixed_estimate_pitch(Yin_Fixed * yin,int16_t* buffer){

	for (size_t i = 0; i < yin->half_buffer_size; i++) {
		yin->yin_buffer_fixed[i] = 0;
		yin->yin_buffer_float[i] = 0;
	}

	int tauEstimate = -1;
	float pitchInHertz = -1;
	
	//step 2
	yin_fixed_difference(yin,buffer);
	
	// step 3
	yin_fixed_cumulative_mean_normalized_difference(yin);
	
	//step 4
	tauEstimate = yin_fixed_absolute_threshold(yin);
	
	//step 5
	if(tauEstimate != -1){
		printf("%d \n" , tauEstimate);
		pitchInHertz = yin->sample_rate / yin_fixed_parabolic_interpolation(yin,tauEstimate);
	}	
	return pitchInHertz;
}

float yin_fixed_last_pitch_estimate_probability(Yin_Fixed * yin){
	return yin->probability;
}

void yin_fixed_destroy(Yin_Fixed * yin) {
	free(yin->yin_buffer_fixed);
	free(yin->yin_buffer_float);	
	free(yin);
}
