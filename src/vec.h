#ifndef __VEC_H
#define __VEC_H

#include <math.h>

typedef struct {
  float x, y, z;
} vec3;

typedef struct {
  float x, y;
} vec2;

static inline vec3 _vec3(float x, float y, float z) { vec3 r = {x, y, z}; return r; }
static inline vec3 vec3_add(vec3 a, vec3 b) { vec3 r = {a.x+b.x,a.y+b.y,a.z+b.z}; return r; }
static inline vec3 vec3_sub(vec3 a, vec3 b) { vec3 r = {a.x-b.x,a.y-b.y,a.z-b.z}; return r; }
static inline vec3 vec3_mul(vec3 a, float x) { vec3 r = {a.x*x,a.y*x,a.z*x}; return r; }
static inline float vec3_length(vec3 a) { return sqrtf(a.x * a.x + a.y * a.y + a.z *a.z); }
static inline vec3 vec3_normalized(vec3 a) { return vec3_mul(a, 1.0f / vec3_length(a)); }
static inline void vec3_normalize(vec3 * a) { float len = vec3_length(*a); a->x /= len; a->y /= len; a->z /= len; }
static inline float vec3_dot(vec3 a, vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static inline vec3 vec3_cross(vec3 a, vec3 b) { vec3 r = { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y-a.y*b.x }; return r; }

static inline vec2 vec2_add(vec2 a, vec2 b) { vec2 r = {a.x+b.x,a.y+b.y}; return r; }
static inline vec2 vec2_sub(vec2 a, vec2 b) { vec2 r = {a.x-b.x,a.y-b.y}; return r; }
static inline vec2 vec2_mul(vec2 a, float x) { vec2 r = {a.x*x,a.y*x}; return r; }
static inline float vec2_dot(vec2 a, vec2 b) { return a.x*b.x + a.y*b.y; }

#endif
