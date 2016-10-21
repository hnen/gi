#ifndef __VEC_H
#define __VEC_H

typedef struct {
  float x, y, z;
} vec3;

typedef struct {
  float x, y;
} vec2;

inline vec3 vec3_add(vec3 a, vec3 b) { vec3 r = {a.x+b.x,a.y+b.y,a.z+b.z}; return r; }
inline vec3 vec3_sub(vec3 a, vec3 b) { vec3 r = {a.x-b.x,a.y-b.y,a.z-b.z}; return r; }
inline vec3 vec3_mul(vec3 a, float x) { vec3 r = {a.x*x,a.y*x,a.z*x}; return r; }
inline float vec3_dot(vec3 a, vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }

inline vec2 vec2_add(vec2 a, vec2 b) { vec2 r = {a.x+b.x,a.y+b.y}; return r; }
inline vec2 vec2_sub(vec2 a, vec2 b) { vec2 r = {a.x-b.x,a.y-b.y}; return r; }
inline vec2 vec2_mul(vec2 a, float x) { vec2 r = {a.x*x,a.y*x}; return r; }
inline float vec2_dot(vec2 a, vec2 b) { return a.x*b.x + a.y*b.y; }

#endif
