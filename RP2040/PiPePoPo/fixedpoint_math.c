#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#include "yin_floating.h"
#include "yin_fixed.h"

void generate_sine_wave(float* buffer, int buffer_length, float frequency, float sample_rate) {
    float amplitude = 0.5f;
    float angular_frequency = 2.0f * M_PI * frequency / sample_rate;
    for (int i = 0; i < buffer_length; i++) {
        buffer[i] = amplitude * sin(angular_frequency * i);
    }
}

void test_pitch_detection_floating(int buffer_length, float sample_rate, float frequency) {
    float * buffer = malloc(sizeof(float) * buffer_length);
    Yin_Floating * yin = yin_floating_new(sample_rate, buffer_length);
    generate_sine_wave(buffer, buffer_length, frequency, sample_rate);
    float pitch = yin_floating_estimate_pitch(yin, buffer);

    printf("Detected pitch floating: %.2f\n", pitch);
}


void test_pitch_detection_fixed(int buffer_length, float sample_rate, float frequency) {

    float * buffer = malloc(sizeof(float) * buffer_length);
    generate_sine_wave(buffer, buffer_length, frequency, sample_rate);

    //convert to fixed point
    int16_t * bufferFixed = malloc(sizeof(int16_t) * buffer_length);
    for (int i = 0; i < buffer_length; i++) {
        bufferFixed[i] = (int16_t) (buffer[i] * INT16_MAX);
    }

    Yin_Fixed * yin = yin_fixed_new(sample_rate, buffer_length);
    float pitch = yin_fixed_estimate_pitch(yin, bufferFixed);

    printf("Detected pitch fixed: %.2f\n", pitch);
}


/*

int main(void) {
    int buffer_length = 2048;
    float sample_rate = 8000.0f;
    float frequency = 471.0f;
    test_pitch_detection_floating(buffer_length, sample_rate, frequency);
    test_pitch_detection_fixed(buffer_length, sample_rate, frequency);
    return 0;
}

*/