#ifndef __VEC_H
#define __VEC_H

typedef struct {
  float x, y, z;
} vec3;

typedef struct {
  float x, y;
} vec2;

extern vec3 vec3_add(vec3 a, vec3 b);
extern vec3 vec3_sub(vec3 a, vec3 b);
extern vec3 vec3_mul(vec3 a, float x);
extern float vec3_dot(vec3 a, vec3 b);

extern vec2 vec2_add(vec2 a, vec2 b);
extern vec2 vec2_sub(vec2 a, vec2 b);
extern vec2 vec2_mul(vec2 a, float x);
extern float vec2_dot(vec2 a, vec2 b);

#endif
