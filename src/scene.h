
#ifndef __SCENE_H
#define __SCENE_H

#include "common.h"
#include "vec.h"

typedef struct {
    vec3 col_albd;
    vec3 col_emit;
} XXmat;

typedef struct {
    vec3 p;
    xxfloat r;
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

#endif

