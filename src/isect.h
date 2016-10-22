
#ifndef __ISECT_H
#define __ISECT_H

#include "vec.h"

int isect_obj(XXobj * obj, vec3 r_p, vec3 r_d, vec3 * out_ipos, vec3 * out_inorm);
int isect_tri(vec3 t_p0, vec3 t_p1, vec3 t_p2, vec3 r_p, vec3 r_d, float * out_t);

int isect_sphere(vec3 s_p, float s_r, vec3 r_p, vec3 r_d, vec3 * out_ipos0, vec3 * out_inorm0, vec3 * out_ipos1, vec3 * out_inorm1);

#endif
