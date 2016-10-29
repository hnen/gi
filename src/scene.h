
#ifndef __SCENE_H
#define __SCENE_H

#include "common.h"
#include "vec.h"

typedef enum {SPHERE, BOX} XXobjtype;

typedef struct {
    vec3 col_albd;
    vec3 col_emit;
} XXmat;

typedef struct {
    XXobjtype objtype;
    union {
        struct {
            vec3 p;
            xxfloat r;
        } sphere;
        struct {
            vec3 p;
            vec3 a0, a1, a2;
        } box;
    };
    int inv;
    XXmat mat;
} XXsceneobj;

typedef struct {
    int obj_count;
    XXsceneobj * objs;
} XXscene;

static inline XXscene scene_alloc(int objcount) {
    XXscene s = {
        objcount, (XXsceneobj*)xxalloc(objcount * sizeof(XXsceneobj))
    };
    return s;
}

static inline void scene_free(XXscene scene) {
    xxfree(scene.objs);
}

static inline XXsceneobj _sceneobj_sphere(vec3 p, float r, XXmat mat) {
    XXsceneobj ret = {
        .objtype = SPHERE,
        .mat = mat,
        .sphere = {
            .p = p, .r = r
        },
        .inv = 0
    };
    return ret;
}

static inline XXsceneobj _sceneobj_box(vec3 p, vec3 a0, vec3 a1, vec3 a2, XXmat mat) {
    XXsceneobj ret = {
        .objtype = BOX,
        .mat = mat,
        .box = {
            .p = p, .a0 = a0, .a1 = a1, .a2 = a2
        },
        .inv = 0
    };
    return ret;
}

static inline XXsceneobj _sceneobj_box_inv(vec3 p, vec3 a0, vec3 a1, vec3 a2, XXmat mat) {
    XXsceneobj ret = _sceneobj_box(p, a0, a1, a2, mat);
    ret.inv = 1;
    return ret;
}

#endif

