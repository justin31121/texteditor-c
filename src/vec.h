#ifndef VEC_H
#define VEC_H

#include <stdbool.h>

typedef struct{
  float x, y;
}Vec;

Vec vec(float a, float b);
Vec vec_s(float a);

bool vec_eq(Vec a, Vec b);

Vec vec_add(Vec a, Vec b);
Vec vec_sub(Vec a, Vec b);
Vec vec_mul(Vec a, Vec b);
Vec vec_div(Vec a, Vec b);

Vec vec_min(Vec a, Vec b);
Vec vec_max(Vec a, Vec b);
Vec vec_dist(Vec min, Vec max);

Vec vec_scale(Vec a, float b);
Vec vec_off(Vec a, float b);

#endif //VEC_H
