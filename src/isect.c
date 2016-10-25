
#define EPSILON 0.00001f

#include <float.h>

#include "obj.h"
#include "isect.h"

int isect_scene(const XXscene * scene, vec3 r_p, vec3 r_d, XXsceneobj ** out_obj, vec3 * out_ipos, vec3 * out_inorm) {
    xxfloat min_t = FLT_MAX;
    int min_i = -1;
    for(int i = 0; i < scene->obj_count; i++) {
        XXsceneobj * obj = &scene->objs[i];
        xxfloat t;
        if (isect_sphere(obj->p, obj->r, r_p, r_d, &t)) {
            if (t < min_t) {
                min_t = t;
                min_i = i;
            }
        }
    }
    if (min_i >= 0) {
        *out_ipos = vec3_add(r_p, vec3_mul(r_d, min_t)); 
        *out_inorm = vec3_sub(*out_ipos, scene->objs[min_i].p);
        *out_obj = &scene->objs[min_i];
        vec3_normalize(out_inorm);
        return 1;
    } else {
        return 0;
    }
    
}

int isect_obj(const XXobj * obj, vec3 r_p, vec3 r_d, vec3 * out_ipos, vec3 * out_inorm) {
    xxfloat min_t = FLT_MAX;
    int min_i = -1;
    for(int i = 0; i < obj->f_count; i++) {
        xxfloat t;
        vec3 p0, p1, p2;
        int i0, i1, i2;
        i0 = obj->f_ind[i*3+0];
        i1 = obj->f_ind[i*3+1];
        i2 = obj->f_ind[i*3+2];
        p0 = obj->v_pos[i0];
        p1 = obj->v_pos[i1];
        p2 = obj->v_pos[i2]; 
        if (isect_tri(p0, p1, p2, r_p, r_d, &t)) {
            if (t < min_t) {
                min_t = t;
                min_i = i;
            }
        }
    }
    if (min_i >= 0) {
        *out_ipos = vec3_add(r_p, vec3_mul(r_d, min_t)); 

        vec3 p0, p1, p2;
        int i0, i1, i2;
        i0 = obj->f_ind[min_i*3+0];
        i1 = obj->f_ind[min_i*3+1];
        i2 = obj->f_ind[min_i*3+2];
        p0 = obj->v_pos[i0];
        p1 = obj->v_pos[i1];
        p2 = obj->v_pos[i2]; 
        *out_inorm = vec3_cross(vec3_sub(p1, p0), vec3_sub(p2, p0));
        vec3_normalize(out_inorm);

        //*out_inorm = obj->f_nrm[min_i];
        return 1;
    } else {
        return 0;
    }
}

/* möller-trumbore */
int isect_tri(vec3 t_p0, vec3 t_p1, vec3 t_p2, vec3 r_p, vec3 r_d, xxfloat * out_t) {
    vec3 e0, e1;
    vec3 P, Q, T;
    xxfloat det, inv_det, u, v;
    xxfloat t;

    e0 = vec3_sub(t_p1, t_p0);
    e1 = vec3_sub(t_p2, t_p0);

    P = vec3_cross(r_d, e1);
    det = vec3_dot(e0, P);

    if (det > -EPSILON && det < EPSILON) return 0;
    inv_det = 1.0f / det;

    T = vec3_sub(r_p, t_p0);
    u = vec3_dot(T, P) * inv_det;
    if (u < 0.0f || u > 1.0f) return 0;

    Q = vec3_cross(T, e0);
    v = vec3_dot(r_d, Q) * inv_det;
    if (v < 0.0f || u+v > 1.0f) return 0;

    t = vec3_dot(e1, Q) * inv_det;
    if (t > EPSILON) {
        *out_t = t;
        //*out_ipos = vec3_add(r_p, vec3_mul(r_d, t));
        //*out_inorm = vec3_cross(e0, e1);
        //vec3_normalize(out_inorm);
        return 1;
    } else {
        return 0;
    }
}

int isect_sphere(vec3 s_p, xxfloat s_r, vec3 r_p, vec3 r_d, xxfloat * out_t) {

    xxfloat a = vec3_dot(r_d, r_d);
    vec3 oc = vec3_sub(r_p, s_p);
    xxfloat b = 2.0f * vec3_dot(r_d, oc);
    xxfloat c = vec3_dot(oc, oc) - s_r * s_r;

    xxfloat det = b*b - 4*a*c;
    if (det >= 0) {
        xxfloat sdet = sqrtf(det);
        xxfloat t0 = (-b - sqrtf(det)) / (2.0f * a);
        if (t0 > 0) {
            *out_t = t0;
            /*
            xxfloat t1 = (-b + sqrtf(det)) / (2.0f * a);
            vec3 p0 = vec3_add(r_p, vec3_mul(r_d, t0));
            vec3 p1 = vec3_add(r_p, vec3_mul(r_d, t1));
            vec3 n0 = vec3_sub(p0, s_p);
            vec3 n1 = vec3_sub(p1, s_p);
            vec3_normalize(&n0);
            vec3_normalize(&n1);
            *out_ipos0 = p0;
            *out_ipos1 = p1;
            *out_inorm0 = n0;
            *out_inorm1 = n1;
            */
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}


