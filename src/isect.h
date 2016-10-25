
#ifndef __ISECT_H
#define __ISECT_H

#include "vec.h"
#include "scene.h"

int isect_scene(const XXscene * scene, vec3 r_p, vec3 r_d, XXsceneobj ** out_obj, vec3 * out_ipos, vec3 * out_inorm);
int isect_obj(const XXobj * obj, vec3 r_p, vec3 r_d, vec3 * out_ipos, vec3 * out_inorm);

int isect_tri(vec3 t_p0, vec3 t_p1, vec3 t_p2, vec3 r_p, vec3 r_d, xxfloat * out_t);
int isect_sphere(vec3 s_p, xxfloat s_r, vec3 r_p, vec3 r_d, xxfloat * out_t);

#endif
