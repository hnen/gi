
#define EPSILON 0.00001f

#include <float.h>

#include "common.h"
#include "obj.h"
#include "scene.h"
#include "isect.h"
#include "timer.h"

int isect_scene(const XXscene * scene, vec3 r_p, vec3 r_d, XXsceneobj ** out_obj, vec3 * out_ipos, vec3 * out_inorm) {
    //timer_push("isect_scene");
    xxfloat min_t = FLT_MAX;
    int min_i = -1;
    vec3 min_norm;
    for(int i = 0; i < scene->obj_count; i++) {
        XXsceneobj * obj = &scene->objs[i];
        xxfloat t;
        if (obj->objtype == SPHERE) {
            if (isect_sphere(obj->sphere.p, obj->sphere.r, r_p, r_d, &t)) {
                if (t < min_t) {
                    min_norm = vec3_sub(vec3_add(r_p, vec3_mul(r_d, t)), scene->objs[i].sphere.p);
                    min_t = t;
                    min_i = i;
                }
            }
        } else if (obj->objtype == BOX) {
            vec3 norm;
            if (isect_box(obj->box.p, obj->box.a0, obj->box.a1, obj->box.a2, r_p, r_d, obj->inv, &t, &norm)) {
                if (t < min_t) {
                    min_norm = norm;
                    min_t = t;
                    min_i = i;
                }
            }
        } else if (obj->objtype == AAB) {
            vec3 norm;
            if (isect_aab(obj->box.p, obj->aab.x, obj->aab.y, obj->aab.z, r_p, r_d, obj->inv, &t, &norm)) {
                if (t < min_t) {
                    min_norm = norm;
                    min_t = t;
                    min_i = i;
                }
            }
        } else {
            printf("UNKNOWN OBJECT #%d\n", i);
            XX_TODO("Unimplemented object type");
        }
    }
    //timer_pop();

    if (min_i >= 0) {
        *out_ipos = vec3_add(r_p, vec3_mul(r_d, min_t)); 
        *out_inorm = min_norm;
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

#define t_plane(p_p, p_n, r_p, r_d) (vec3_dot(vec3_sub(p_p, r_p), p_n) / vec3_dot(r_d, p_n))

#define t_plane_s(p_p_x, r_p_x, r_d_x) ((p_p_x - r_p_x) / r_d_x)

int isect_aab(vec3 b_p, float a_x, float a_y, float a_z, vec3 r_p, vec3 r_d, int inv, xxfloat * out_t, vec3 * out_inorm) {
    vec3 norm;

    float t_a00 = t_plane_s(b_p.x + a_x, r_p.x, r_d.x);
    float t_a01 = t_plane_s(b_p.x - a_x, r_p.x, r_d.x);

    float tmin0 = fminf(t_a00, t_a01);
    float tmax0 = fmaxf(t_a00, t_a01);

    float t_a10 = t_plane_s(b_p.y + a_y, r_p.y, r_d.y);
    float t_a11 = t_plane_s(b_p.y - a_y, r_p.y, r_d.y);

    float tmin1 = fmaxf(tmin0, fminf(t_a10, t_a11));
    float tmax1 = fminf(tmax0, fmaxf(t_a10, t_a11));

    if (tmin1 > tmax1) return 0;
    
    float t_a20 = t_plane_s(b_p.z + a_z, r_p.z, r_d.z);
    float t_a21 = t_plane_s(b_p.z - a_z, r_p.z, r_d.z);

    float tmin2 = fmaxf(tmin1, fminf(t_a20, t_a21));
    float tmax2 = fminf(tmax1, fmaxf(t_a20, t_a21));

    if (tmin2 > tmax2) return 0;

    if (!inv) {
        if (tmin2 > tmin1) {
            *out_inorm = _vec3(0, 0, t_a20 < t_a21 ? 1 : -1);
        } else if (tmin1 > tmin0) {
            *out_inorm = _vec3(0, t_a10 < t_a11 ? 1 : -1, 0);
        } else {
            *out_inorm = _vec3(t_a00 < t_a01 ? 1 : -1, 0, 0);
        }
        *out_t = tmin2;
    } else {
        if (tmax2 < tmax1) {
            *out_inorm = _vec3(0, 0, t_a20 < t_a21 ? 1 : -1);
        } else if (tmax1 < tmax0) {
            *out_inorm = _vec3(0, t_a10 < t_a11 ? 1 : -1, 0);
        } else {
            *out_inorm = _vec3(t_a00 < t_a01 ? 1 : -1, 0, 0);
        }
        *out_t = tmax2;
    }

    vec3_normalize(out_inorm);


    return 1;
}

int isect_box(vec3 b_p, vec3 b_a0, vec3 b_a1, vec3 b_a2, vec3 r_p, vec3 r_d, int inv, xxfloat * out_t, vec3 * out_inorm) {
    vec3 norm;

    float t_a00 = t_plane(vec3_add(b_p, b_a0), b_a0, r_p, r_d);
    float t_a01 = t_plane(vec3_sub(b_p, b_a0), b_a0, r_p, r_d);

    float tmin0 = fminf(t_a00, t_a01);
    float tmax0 = fmaxf(t_a00, t_a01);

    float t_a10 = t_plane(vec3_add(b_p, b_a1), b_a1, r_p, r_d);
    float t_a11 = t_plane(vec3_sub(b_p, b_a1), b_a1, r_p, r_d);

    float tmin1 = fmaxf(tmin0, fminf(t_a10, t_a11));
    float tmax1 = fminf(tmax0, fmaxf(t_a10, t_a11));

    if (tmin1 > tmax1) return 0;
    
    float t_a20 = t_plane(vec3_add(b_p, b_a2), b_a2, r_p, r_d);
    float t_a21 = t_plane(vec3_sub(b_p, b_a2), b_a2, r_p, r_d);

    float tmin2 = fmaxf(tmin1, fminf(t_a20, t_a21));
    float tmax2 = fminf(tmax1, fmaxf(t_a20, t_a21));

    if (tmin2 > tmax2) return 0;


    if (!inv) {
        if (tmin2 > tmin1) {
            *out_inorm = vec3_mul(b_a2, t_a20 < t_a21 ? 1 : -1);
        } else if (tmin1 > tmin0) {
            *out_inorm = vec3_mul(b_a1, t_a10 < t_a11 ? 1 : -1);
        } else {
            *out_inorm = vec3_mul(b_a0, t_a00 < t_a01 ? 1 : -1);
        }
        *out_t = tmin2;
    } else {
        if (tmax2 < tmax1) {
            *out_inorm = vec3_mul(b_a2, t_a20 < t_a21 ? 1 : -1);
        } else if (tmax1 < tmax0) {
            *out_inorm = vec3_mul(b_a1, t_a10 < t_a11 ? 1 : -1);
        } else {
            *out_inorm = vec3_mul(b_a0, t_a00 < t_a01 ? 1 : -1);
        }
        *out_t = tmax2;
    }

    vec3_normalize(out_inorm);

    return 1;
}





