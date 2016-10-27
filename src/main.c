#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#include "glfw.h"
#include "cl.h"
#include "gl.h"
#include "obj.h"
#include "isect.h"
#include "scene.h"

#define W 1024
#define H ((W)*9/16)


#define SAMPLES_PER_PIXEL 1
#define OBJ_COUNT 12
#define OBJ_SIZE_MIN 0.2f
#define OBJ_SIZE_MAX 2.0f
#define RAY_DEPTH 5

#define EMIT_INITIAL_RAYS 100000
#define MAX_EMITTED_HITS (EMIT_INITIAL_RAYS * RAY_DEPTH)

const char* vertex_shader =
"#version 400\n"
"in vec3 vp;"
"out vec2 uv;"
"void main() {"
"  gl_Position = vec4(vp, 1.0);"
"  uv = (vp.xy + vec2(1.0f, 1.0f)) * 0.5f;"
"}";

const char* fragment_shader =
"#version 400\n"
"in vec2 uv;"
"out vec4 frag_colour;"
"uniform sampler2D tex;"
"void main() {"
"  frag_colour = texture(tex, uv);"
"}";

struct XXshaderprogram g_program;
GLuint g_tex;
XXobj g_obj;

static void createTextureData(char * out_data, int w, int h);
static void renderScene(const XXscene * scene, char * out_data, int w, int h, xxfloat t);
static inline unsigned int evalpixel(vec3 r_p, vec3 r_d, vec3 cam_d, const XXscene * scene);
static vec3 computeradiance(vec3 r_p, vec3 r_d, const XXscene * scene, int depth, int * out_noisect);
static inline float isect_scene_len(const XXscene * scene, vec3 r_p, vec3 r_d);
static inline vec3 genhemisray(vec3 nitnrm);
static inline vec3 pickspherepos();
static void allocrandomscene(int obj_count, XXscene * out_scene);
static void emitlightrays(const XXscene * scene, vec3 cam_p, vec3 cam_d, unsigned int * out_data, int w, int h);
static void emitray(vec3 r_p, vec3 r_d, XXsceneobj * emitter, vec3 cam_p, vec3 cam_d, unsigned int * out_data, int w, int h);

XXscene g_scene;

static void init() {
    obj_parse("./data/kuutiot.obj", &g_obj);
    obj_printf(&g_obj);

    srand(time(0));
    allocrandomscene(OBJ_COUNT, &g_scene);

    XX_E(gl_createShaderProgram(vertex_shader, fragment_shader, &g_program));
    gl_createTexture(GL_CLAMP_TO_EDGE, GL_LINEAR, &g_tex);
}

static void release() {
    scene_free(g_scene);
    XX_E(gl_releaseShaderProgram(g_program));
    gl_deleteTexture(g_tex);
}

static void run() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static const GLfloat vertex_data[] = {
        1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    GLuint vao, vbo;
    gl_createVAO(&vao);
    gl_createVBO(&vbo, vertex_data, sizeof(vertex_data));

    void * data = xxalloc(W*H*4);
    renderScene(&g_scene, data, W, H, glfwGetTime());
    gl_setTextureData(g_tex, W, H, data);
    xxfree(data);

    GL_E(glUseProgram(g_program.program));
    GL_E(glUniform1i(glGetUniformLocation(g_program.program, "tex"), 0));

    gl_renderVBO(vbo, g_program.program, 2);

    gl_releaseVBO(vbo);
    gl_releaseVAO(vao);

}


static void renderScene(const XXscene * scene, char * out_data, int w, int h, xxfloat t) {

    vec3 cam_p = {sinf(t) * 10.0f, 1.7f, cosf(t) * 10.0f};
    vec3 cam_dst = {0, 0.0f, 0};
    vec3 cam_d = vec3_sub(cam_dst, cam_p);
    vec3_normalize(&cam_d);

    vec3 wld_u = {0, 1, 0};
    vec3 cam_l = vec3_cross(wld_u, cam_d);
    vec3 cam_u = vec3_cross(cam_d, cam_l);

    vec3_normalize(&cam_d);
    vec3_normalize(&cam_l);
    vec3_normalize(&cam_u);

    unsigned int * buf = (unsigned int*)out_data;

    int nodecount = 0;
    printf("emitting rays..\n");
    emitlightrays(scene, cam_p, cam_d, buf, w, h);

    /*
    for(int y = 0; y < h; y++) {
        printf("%.2f%%%...\n", (float)(y*100)/h);
        for(int x = 0; x < w; x++) { 
            vec3 i = vec3_mul(cam_l, (xxfloat)(x-w/2) * 0.003f);
            vec3 j = vec3_mul(cam_u, (xxfloat)(y-h/2) * 0.003f);
            vec3 r_d = vec3_add(cam_d, vec3_add(i, j));
            vec3_normalize(&r_d);

            *(buf++) = evalpixel(cam_p, r_d, cam_d, &g_scene, emitnodesarr, nodecount);
        }
    }
    */
}

static float * alloc_object_ray_emit_factors_cumulative(const XXscene * scene);

static void emitlightrays(const XXscene * scene, vec3 cam_p, vec3 cam_d, unsigned int * out_data, int w, int h) {
    float * emit_f = alloc_object_ray_emit_factors_cumulative(scene);
    int obj_i = 0;
    double ity_i = 0.0f;
    double ity_d = (double)emit_f[scene->obj_count-1] / EMIT_INITIAL_RAYS;
    for(int i = 0; i < EMIT_INITIAL_RAYS; i++) {
        ity_i += ity_d;
        /*
        while(emit_f[obj_i] < ity_i) {
            obj_i++;
        }
        */
        obj_i = rand()%scene->obj_count;
        XXsceneobj * obj = &scene->objs[obj_i];
        vec3 r_p = vec3_add(vec3_mul(pickspherepos(), obj->r), obj->p);
        vec3 r_d = vec3_sub(r_p, obj->p);
        vec3_normalize(&r_d);
        r_d = genhemisray(r_d);
        emitray(r_p, r_d, obj, cam_p, cam_d, out_data, w, h);
    }
    xxfree(emit_f);
}

static float * alloc_object_ray_emit_factors_cumulative(const XXscene * scene) {
    float * emitter_intensities = (float*)xxalloc(scene->obj_count*sizeof(float));
    float prev_ity = 0.0f;
    for(int i = 0; i < scene->obj_count; i++) {
        XXsceneobj * obj = &scene->objs[i];
        float ity = vec3_dot(obj->mat.col_emit, _vec3(1,1,1)) / 3.0f;
        ity *= obj->r * obj->r;
        emitter_intensities[i] = prev_ity + ity;
        prev_ity = emitter_intensities[i];
    }
    return emitter_intensities;
}


static void emitray(vec3 r_p, vec3 r_d, XXsceneobj * emitter, vec3 cam_p, vec3 cam_d, unsigned int * out_data, int w, int h) {
    vec3 raycol = emitter->mat.col_emit;
    //out_emitnodes[(*nodecount)++] = _emitted_hit(r_p, raycol);
    for (int i = 0; i < RAY_DEPTH; i++) {
        vec3 hitpos, hitnrm;
        XXsceneobj * hitobj = 0;
        if (isect_scene(&g_scene, r_p, r_d, &hitobj, &hitpos, &hitnrm)) {
            vec3 obj_albd = hitobj->mat.col_albd;
            raycol = _vec3(raycol.x * obj_albd.x, raycol.y * obj_albd.y, raycol.z * obj_albd.z);
            raycol = vec3_add(raycol, hitobj->mat.col_emit);
            r_p = hitpos;
            r_d = genhemisray(hitnrm);
            //out_emitnodes[(*nodecount)++] = _emitted_hit(r_p, raycol);
        } else {
            break;
        }
    }
}

static inline unsigned int evalpixel(vec3 r_p, vec3 r_d, vec3 cam_d, const XXscene * scene) { 
    vec3 pos, nrm;
    vec3 c = _vec3(0,0,0);
    int isec = 0;
    for (int i = 0; i < SAMPLES_PER_PIXEL && !isec; i++) {
        c = vec3_add(c, computeradiance(r_p, r_d, scene, 0, &isec));
    }
    c = vec3_mul(c, 1.0f / SAMPLES_PER_PIXEL);
    xxfloat f = (c.x+c.y+c.z) / 3.0f;
    unsigned char f_r = (unsigned char)(clamp(c.x) * 255);
    unsigned char f_g = (unsigned char)(clamp(c.y) * 255);
    unsigned char f_b = (unsigned char)(clamp(c.z) * 255);
    return f_r | (f_g << 8) | (f_b << 16);
}

static vec3 computeradiance(vec3 r_p, vec3 r_d, const XXscene * scene, int depth, int * out_noisect) {
    if (depth > RAY_DEPTH) {
        return _vec3(0,0,0);
    }
    vec3 hitpos, hitnrm;
    XXsceneobj * hitobj = 0;
    if (isect_scene(scene, r_p, r_d, &hitobj, &hitpos, &hitnrm)) {
        vec3 emit = hitobj->mat.col_emit;
        vec3 albd = hitobj->mat.col_albd;
        vec3 rad = _vec3(0,0,0);
        vec3 nr = genhemisray(hitnrm);
        rad = computeradiance(hitpos, nr, scene, depth+1, out_noisect);
        rad = _vec3(rad.x * albd.x, rad.y * albd.y, rad.z * albd.z);
        return vec3_add(emit, rad);
    }
    if (depth == 0) {
        *out_noisect = 1;
    }
    return _vec3(0,0,0);
}

static inline float isect_scene_len(const XXscene * scene, vec3 r_p, vec3 r_d) {
    vec3 hitpos, hitnrm;
    XXsceneobj * hitobj = 0;
    if (isect_scene(scene, r_p, r_d, &hitobj, &hitpos, &hitnrm)) {
        return vec3_length(vec3_sub(hitpos, r_p));
    }
    return FLT_MAX;
}

static inline vec3 pickspherepos() {
    xxfloat y = (randf() - 0.5f) * 2.0f;
    xxfloat a = 2.0f*M_PI*randf();
    xxfloat r2 = sqrtf(1 - y*y);
    return _vec3(sinf(a) * r2, y, cosf(a) * r2);
}
static inline vec3 genhemisray(vec3 hitnrm) {
    xxfloat r1 = 2*M_PI*randf(), r2 = randf(), r2s = sqrtf(r2);
    vec3 j = hitnrm;
    vec3 i = vec3_cross(fabsf(j.x) > 0.1f ? _vec3(0,1,0) : _vec3(1,0,0), j);
    vec3_normalize(&i);
    vec3 k = vec3_cross(i, j);
    vec3 nr = vec3_add(vec3_mul(j, sqrtf(1-r2)),
            vec3_add(vec3_mul(i, cosf(r1)*r2s),
                vec3_mul(k, sinf(r1)*r2s)));
    vec3_normalize(&nr);
    return nr;
}

static void allocrandomscene(int obj_count, XXscene * out_scene) {
    *out_scene = scene_alloc(obj_count);
    XXmat objmat = { .col_albd={1,1,1}, .col_emit={0,0,0} };
    XXmat litmat = { .col_albd={0,0,0}, .col_emit={1,1,1} };
    for (int i = 0; i < obj_count-1; i++) {
        vec3 obj_col = {1 - randf()*0.5f,1 - randf()*0.5f,1 - randf()*0.5f};
        vec3 emit = vec3_mul(obj_col, i <= 4 ? 2.75f : 0.01f);
        XXsceneobj obj = {
            .p={(randf() - 0.5f) * 7.5f, (randf() - 0.5f) * 7.5f, (randf() - 0.5f) * 7.5f},
            .r= OBJ_SIZE_MIN + randf() * (OBJ_SIZE_MAX - OBJ_SIZE_MIN),
            .mat = { .col_albd=obj_col, .col_emit=emit }
        };
        out_scene->objs[i] = obj;
    }
    /*
    double R0 = 200;
    XXsceneobj obj = {
        .p = {0, R0 + 100, 0},
        .r= R0,
        .mat = litmat
    };
    out_scene->objs[obj_count-1] = obj;
    */

    double R = 10000;
    int j = obj_count;
    XXsceneobj obj0 = {
        .p = {0, -R - 3.25, 0},
        .r = R,
        .mat = { .col_albd={ 0.9f, 0.9f, 0.9f }, .col_emit={0,0,0} }
    };
    out_scene->objs[obj_count-1] = obj0;

    /*
       XXsceneobj obj1 = {
       .p = {0, 0, R + 15.0},
       .r = R,
       .mat = objmat
       };
       out_scene->objs[j+1] = obj1;

       XXsceneobj obj2 = {
       .p = {0, 0,-R - 15.0},
       .r = R,
       .mat = objmat
       };
       out_scene->objs[j+2] = obj2;

       XXsceneobj obj3 = {
       .p = {-R - 15.0, 0, 0},
       .r = R,
       .mat = objmat
       };
       out_scene->objs[j+3] = obj3;

       XXsceneobj obj4 = {
       .p = {R + 15.0, 0, 0},
       .r = R,
       .mat = objmat
       };
       out_scene->objs[j+4] = obj4;
     */

}

int main() {

    //cl_run();

    GLFWwindow * window;
    glfw_init(&window, W, H);
    init();

    glfw_run(window, &run);

    release();
    glfwTerminate();

    return 0;
}








