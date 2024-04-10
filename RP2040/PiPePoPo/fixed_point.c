#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "fixed_point.h"


fixed_point fixed_point_mul(fixed_point a, fixed_point b) {
    int32_t temp = (int32_t)a * (int32_t)b;
    fixed_point result = (fixed_point)(temp >> 15);
    return result;
}

fixed_point fixed_point_div(fixed_point a, fixed_point b) {
    int32_t temp = (int32_t)a << 15;
    fixed_point result = (fixed_point)(temp / b);
    return result;
}

fixed_point fixed_point_sum(fixed_point a, fixed_point b) {
    return a + b;
}

float fixed_point_to_f(fixed_point value) {
    return (float) value / (float) INT16_MAX;
}

fixed_point fixed_point_from_f(float value) {
    return (fixed_point)(value * INT16_MAX);
}


float random_float(void){
   float zero_hundedk = (float) (rand() % 100000);
   return (zero_hundedk - 50000.0f)/50000.0f;
}






void test_fixed_point_mul(void) {

    for (int i = 0; i < 1000; i++) {
        float float_a = random_float();
        float float_b = random_float();

        fixed_point a = fixed_point_from_f(float_a);
        fixed_point b = fixed_point_from_f(float_b);

        fixed_point result = fixed_point_mul(a, b);

        float float_result = fixed_point_to_f(result);
        float float_expected_result = float_a * float_b;
        float diff = fabs(float_result - float_expected_result);

        assert(diff < 0.001f);
    }
}

void test_fixed_point_div(void) {

    for (int i = 0; i < 1000; i++) {
        float float_a = random_float();
        float float_b = random_float();

        fixed_point a = fixed_point_from_f(float_a);
        fixed_point b = fixed_point_from_f(float_b);

        fixed_point result = fixed_point_div(a, b);

        float float_result = fixed_point_to_f(result);
        float float_expected_result = float_a / float_b;
        float diff = fabs(float_result - float_expected_result);


        float allowed_dif = float_expected_result * 0.1f > 0.1f ? float_expected_result * 0.1f : 0.2f;

        //fprintf(stderr, "float_a: %f, float_b: %f, float_result: %f, float_expected_result: %f, diff: %f, allowed diff %f\n", float_a, float_b, float_result, float_expected_result, diff,allowed_dif);


        assert(diff < allowed_dif);
    }
}

void test_fixed_point_sum(void) {

    for (int i = 0; i < 1000; i++) {
        float float_a = random_float();
        float float_b = random_float();

        fixed_point a = fixed_point_from_f(float_a);
        fixed_point b = fixed_point_from_f(float_b);

        fixed_point result = fixed_point_sum(a, b);

        float float_result = fixed_point_to_f(result);
        float float_expected_result = float_a + float_b;
        float diff = fabs(float_result - float_expected_result);

        assert(diff < 0.001f);
    }
}

void test_fixed_point_math(void) {
    srand(0);
    test_fixed_point_mul();
    test_fixed_point_div();
    test_fixed_point_sum();
}
/*
int main(void) {
    test_fixed_point_math();
    printf("All tests passed\n");
    return 0;
}
*/
