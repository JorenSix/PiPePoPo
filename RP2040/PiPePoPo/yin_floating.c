#include <stdlib.h>
#include <stdio.h>
#include "yin_floating.h"

//state information
struct Yin_Floating {
	size_t half_buffer_size;

	float sample_rate;

	float threshold;

	float probability;

	float * yin_buffer;
};

Yin_Floating * yin_floating_new(float sample_rate, size_t buffer_size) {
    Yin_Floating * yin = malloc(sizeof(Yin_Floating));
    yin->half_buffer_size = buffer_size / 2;
    yin->sample_rate = sample_rate;
    yin->threshold = 0.15;
    yin->probability = -1.0;
    yin->yin_buffer = malloc(sizeof(float) * yin->half_buffer_size);
    for (size_t i = 0; i < yin->half_buffer_size; i++) {
        yin->yin_buffer[i] = 0;
    }
    return yin;
}

void yin_floating_difference(Yin_Floating * yin_floating,float* buffer){
	size_t index;
	size_t tau;
	float delta;
	for(tau = 0 ; tau < yin_floating->half_buffer_size; tau++){
		for(index = 0; index < yin_floating->half_buffer_size; index++){
			delta = buffer[index] - buffer[index + tau];
			yin_floating->yin_buffer[tau] += delta * delta;
		}
		
	}
}

void yin_floating_cumulative_mean_normalized_difference(Yin_Floating * yin_floating){
	size_t tau;
	yin_floating->yin_buffer[0] = 1;
	float running_sum = 0;
	for (tau = 1; tau < yin_floating->half_buffer_size; tau++) {
		running_sum += yin_floating->yin_buffer[tau];
		yin_floating->yin_buffer[tau] *= tau / running_sum;

		//printf("floating %d %f %f\n" ,tau, yin_floating->yin_buffer[tau],running_sum);
	}
	
}

int yin_floating_absolute_threshold(Yin_Floating * yin_floating){
	size_t tau;
	// first two positions in yin_floating->yin_buffer are always 1
	// So start at the third (index 2)
	for (tau = 2; tau < yin_floating->half_buffer_size ; tau++) {
		if (yin_floating->yin_buffer[tau] < yin_floating->threshold) {
			while (tau + 1 < yin_floating->half_buffer_size && yin_floating->yin_buffer[tau + 1] < yin_floating->yin_buffer[tau]) {
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
			yin_floating->probability = 1 - yin_floating->yin_buffer[tau];
			break;
		}
	}
	// if no pitch found, tau => -1
	if (tau == yin_floating->half_buffer_size || yin_floating->yin_buffer[tau] >= yin_floating->threshold) {
		tau = -1;
		yin_floating->probability = 0;
	}
	return tau;
}

float yin_floating_parabolic_interpolation(Yin_Floating * yin_floating,size_t tau_estimate) {
	float better_tau;
	size_t x0;
	size_t x2;
	
	if (tau_estimate < 1) {
		x0 = tau_estimate;
	} 
	else {
		x0 = tau_estimate - 1;
	}
	if (tau_estimate + 1 < yin_floating->half_buffer_size) {
		x2 = tau_estimate + 1;
	} 
	else {
		x2 = tau_estimate;
	}
	if (x0 == tau_estimate) {
		if (yin_floating->yin_buffer[tau_estimate] <= yin_floating->yin_buffer[x2]) {
			better_tau = tau_estimate;
		} 
		else {
			better_tau = x2;
		}
	} 
	else if (x2 == tau_estimate) {
		if (yin_floating->yin_buffer[tau_estimate] <= yin_floating->yin_buffer[x0]) {
			better_tau = tau_estimate;
		} 
		else {
			better_tau = x0;
		}
	} 
	else {
		float s0, s1, s2;
		s0 = yin_floating->yin_buffer[x0];
		s1 = yin_floating->yin_buffer[tau_estimate];
		s2 = yin_floating->yin_buffer[x2];
		// fixed AUBIO implementation, thanks to Karl Helgason:
		// (2.0f * s1 - s2 - s0) was incorrectly multiplied with -1
		better_tau = tau_estimate + (s2 - s0) / (2 * (2 * s1 - s2 - s0));
	}
	return better_tau;
}


float yin_floating_estimate_pitch(Yin_Floating * yin_floating,float* buffer){

	int tauEstimate = -1;
	float pitchInHertz = -1;
	
	//step 2
	yin_floating_difference(yin_floating,buffer);
	
	// step 3
	yin_floating_cumulative_mean_normalized_difference(yin_floating);
	
	//step 4
	tauEstimate = yin_floating_absolute_threshold(yin_floating);
	
	//step 5
	if(tauEstimate != -1){
		printf("%d \n" , tauEstimate);
		pitchInHertz = yin_floating->sample_rate / yin_floating_parabolic_interpolation(yin_floating,tauEstimate);
	}
	
	return pitchInHertz;
}


float yin_floating_last_pitch_estimate_probability(Yin_Floating * yin_floating){
	return yin_floating->probability;
}

void yin_floating_destroy(Yin_Floating * yin) {
	free(yin->yin_buffer);
	free(yin);
}
