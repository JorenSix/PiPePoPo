#include <stddef.h>
#include <stdint.h>

#ifndef FIXED_POINT_H
#define FIXED_POINT_H

typedef int32_t fixed_point;

fixed_point fixed_point_mul(fixed_point a, fixed_point b);

fixed_point fixed_point_div(fixed_point a, fixed_point b);

fixed_point fixed_point_sum(fixed_point a, fixed_point b);

float fixed_point_to_f(fixed_point value);

fixed_point fixed_point_from_f(float value);

#endif // FIXED_POINT_H
