#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <string.h>
#include <math.h>

#include "glfw.h"
#include "cl.h"
#include "gl.h"
#include "obj.h"
#include "isect.h"
#include "scene.h"
#include "timer.h"

#define WINDOW_W 1280
#define WINDOW_H ((WINDOW_W)*9/16)

#define SCR_W 320
#define SCR_H ((SCR_W)*9/16)

#define EPSILON 0.0001f

#define SAMPLES_PER_PIXEL 1
#define OBJ_COUNT 12
#define OBJ_SIZE_MIN 0.2f
#define OBJ_SIZE_MAX 2.0f
#define RAY_DEPTH 2

#define CAM_FOV (M_PI * 45.0f / 180.0f)
#define CAM_NEAR 0.1f

#define EMIT_INITIAL_RAYS 50000

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

typedef struct {
    unsigned int * pixels;
    float * sample_counts;
    int w, h;
} XXrenderbuffer;

static XXrenderbuffer renderbuffer_alloc(int w, int h) {
    XXrenderbuffer ret = {
        .pixels = (unsigned int*)xxalloc(w*h*4),
        .sample_counts = (float*)xxalloc(w*h*sizeof(float)),
        .w = w, .h = h
    };
    return ret;
}
static void renderbuffer_free(XXrenderbuffer buf) {
    xxfree(buf.pixels);
    xxfree(buf.sample_counts);
}
static void renderbuffer_clear(XXrenderbuffer * buf) {
    memset(buf->pixels, 0, buf->w * buf->h * 4);
    memset(buf->sample_counts, 0, buf->w * buf->h * sizeof(float));
    for (int i = 0; i < buf->w*buf->h; i++) {
        buf->sample_counts[i] = 0.0f;
    }
}

struct XXshaderprogram g_program;
GLuint g_tex;
XXobj g_obj;

static void render_scene(XXscene * scene, XXrenderbuffer * out_rndbuf, xxfloat t);
static inline float isect_scene_len(const XXscene * scene, vec3 r_p, vec3 r_d);
static inline vec3 genhemisray(vec3 nitnrm);
static inline vec3 pickspherepos();
static void alloc_scene(int obj_count, XXscene * out_scene);
static void cast_rays_from_emitters(const XXscene * scene, vec3 cam_p, vec3 cam_d, XXrenderbuffer * out_rndbuf);
static void emit_rnd_ray_from_object(const XXscene * scene, int obj_i, vec3 cam_p, vec3 cam_d, XXrenderbuffer * out_rndbuf);
static void emitray(vec3 r_p, vec3 r_d, const XXscene * scene, const XXsceneobj * emitter, vec3 cam_p, vec3 cam_d, XXrenderbuffer * out_rndbuf);
static int is_occluded(const XXscene * scene, vec3 cam_pos, vec3 test_pos);
static int project_to_camera(vec3 cam_p, vec3 cam_d, vec3 wld_u, vec3 p, float * out_x, float * out_y, float aspect);
static void cast_hit_to_screen(vec3 hitpos, vec3 hitcol, vec3 cam_p, vec3 cam_d, const XXscene * scene, XXrenderbuffer * out_rndbuf);
static void add_sample_to_pixels(float fx, float fy, vec3 col, XXrenderbuffer * out_rndbuf);
static inline void add_sample_to_pixel(int i_x, int i_y, float w, vec3 col, XXrenderbuffer * out_rndbuf);

// obsolete unidirectional pathtracing funcs
static inline unsigned int cast_ray_from_camera(vec3 r_p, vec3 r_d, float x_var, float y_var, vec3 cam_d, const XXscene * scene);
static vec3 computeradiance(vec3 r_p, vec3 r_d, const XXscene * scene, int depth);

XXscene g_scene;
XXrenderbuffer g_rndbuf;

static void init() {
    obj_parse("./data/kuutiot.obj", &g_obj);
    obj_printf(&g_obj);

    g_rndbuf = renderbuffer_alloc(SCR_W, SCR_H);

    srand(time(0));
    alloc_scene(OBJ_COUNT, &g_scene);

    XX_E(gl_createShaderProgram(vertex_shader, fragment_shader, &g_program));
    gl_createTexture(GL_CLAMP_TO_EDGE, GL_LINEAR, &g_tex);

    timer_reset();
}

static void release() {
    scene_free(g_scene);
    renderbuffer_free(g_rndbuf);
    XX_E(gl_releaseShaderProgram(g_program));
    gl_deleteTexture(g_tex);
}

double prev_time = 0;
double avg_frametime = 0;
int framecount = 0;

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

    timer_push("frame");
    render_scene(&g_scene, &g_rndbuf, glfwGetTime());
    timer_pop();

    framecount++;

    timer_frame();

    if (glfwGetTime() - prev_time > 1) {
        timer_purge();
        printf("fps: %.2f\n", (double)framecount / (glfwGetTime() - prev_time));
        timer_dump();
        timer_reset();
        framecount = 0;
        prev_time = glfwGetTime();
    }

    gl_setTextureData(g_tex, SCR_W, SCR_H, g_rndbuf.pixels);

    GL_E(glUseProgram(g_program.program));
    GL_E(glUniform1i(glGetUniformLocation(g_program.program, "tex"), 0));

    gl_renderVBO(vbo, g_program.program, 2);

    gl_releaseVBO(vbo);
    gl_releaseVAO(vao);

}

static void scene_gather_emitters(XXscene * scene, float min_emission) {
    scene->emitter_objs_count = 0;
    for(int i = 0; i < scene->obj_count; i++) {
        XXsceneobj * obj = &scene->objs[i];
        vec3 col_emit = obj->mat.col_emit;
        float emission = vec3_dot(col_emit, _vec3(1,1,1)) / 3.0f;
        if (emission > min_emission) {
            scene->emitter_objs[scene->emitter_objs_count++] = i;
        }
    }
}


static void render_scene(XXscene * scene, XXrenderbuffer * out_rndbuf, xxfloat t) {

    scene_gather_emitters(scene, 0.01f);

    //scene->objs[0].p = _vec3(sinf(t) * 4.0f, 3.0f + sinf(t*0.2f) * 2.0f, cosf(t*1.5f)*4.0f);

    float cam_r = 10.0f + sinf(t * 0.6f) * 4.0f;
    vec3 cam_p = {sinf(t * 0.2f) * cam_r, 7.0f, cosf(t * 0.2f) * cam_r};
    vec3 cam_dst = {0, 0.0f, 0};
    vec3 cam_d = vec3_sub(cam_dst, cam_p);
    vec3_normalize(&cam_d);

    vec3 wld_u = {0, 1, 0};
    vec3 cam_l = vec3_cross(wld_u, cam_d);
    vec3 cam_u = vec3_cross(cam_d, cam_l);

    vec3_normalize(&cam_d);
    vec3_normalize(&cam_l);
    vec3_normalize(&cam_u);

    renderbuffer_clear(out_rndbuf);
    //cast_rays_from_emitters(scene, cam_p, cam_d, out_rndbuf);

    float tanh = tanf(CAM_FOV * 0.5f);
    float tanw = (float)SCR_W / SCR_H * tanf(CAM_FOV * 0.5f);

    int w = out_rndbuf->w;
    int h = out_rndbuf->h;
    unsigned int * buf = out_rndbuf->pixels;

    timer_push("render");
    for(int y = 0; y < h; y++) {
        //printf("%.2f%%%...\n", (float)(y*100)/h);
        vec3 j = vec3_mul(cam_u, tanh * 2.0f * (xxfloat)(y-h/2) / h );
        for(int x = 0; x < w; x++) { 
            vec3 i = vec3_mul(cam_l, tanw * 2.0f * (xxfloat)(x-w/2) / w );
            vec3 r_d = vec3_add(cam_d, vec3_add(i, j));

            *(buf++) = cast_ray_from_camera(cam_p, r_d, tanw/w, tanh/h, cam_d, &g_scene);
        }
    }
    timer_pop();
}

static float * alloc_object_ray_emit_factors_cumulative(const XXscene * scene);

static void cast_rays_from_emitters(const XXscene * scene, vec3 cam_p, vec3 cam_d, XXrenderbuffer * out_rndbuf) {
    float * emit_f = alloc_object_ray_emit_factors_cumulative(scene);
    int obj_i = 0;
    double ity_i = 0.0f;
    double ity_d = (double)emit_f[scene->obj_count-1] / EMIT_INITIAL_RAYS;
    for(int i = 0; i < EMIT_INITIAL_RAYS; i++) {
        ity_i += ity_d;
        while(obj_i < scene->obj_count && emit_f[obj_i] < ity_i) {
            obj_i++;
        }
        if (obj_i < scene->obj_count) {
            vec3 emit_col = scene->objs[obj_i].mat.col_emit;
            //printf("emit ray from obj %d, (emit %.5f, emit_f %.5f, ity_i %f, ity_d %f)\n", obj_i, vec3_dot(emit_col, _vec3(1,1,1))/3, emit_f[obj_i], ity_i, ity_d);
            //obj_i = rand()%scene->obj_count;
            emit_rnd_ray_from_object(scene, obj_i, cam_p, cam_d, out_rndbuf);
        }
    }
    xxfree(emit_f);
}

static void emit_rnd_ray_from_object(const XXscene * scene, int obj_i, vec3 cam_p, vec3 cam_d, XXrenderbuffer * out_rndbuf) {
    XXsceneobj * obj = &scene->objs[obj_i];
    if (obj->objtype == SPHERE) {
        vec3 r_p = vec3_add(vec3_mul(pickspherepos(), obj->sphere.r), obj->sphere.p);
        vec3 r_d = vec3_sub(r_p, obj->sphere.p);
        vec3_normalize(&r_d);
        r_d = genhemisray(r_d);
        emitray(r_p, r_d, scene, obj, cam_p, cam_d, out_rndbuf);
    } else {
        printf("objtype %d\n", obj->objtype);
        XX_TODO("unimplemented object type");
    }
}

static float * alloc_object_ray_emit_factors_cumulative(const XXscene * scene) {
    float * emitter_intensities = (float*)xxalloc(scene->obj_count*sizeof(float));
    float prev_ity = 0.0f;
    for(int i = 0; i < scene->obj_count; i++) {
        XXsceneobj * obj = &scene->objs[i];
        if (obj->objtype == SPHERE) {
            float ity = vec3_dot(obj->mat.col_emit, _vec3(1,1,1)) / 3.0f;
            ity *= obj->sphere.r * obj->sphere.r;
            emitter_intensities[i] = prev_ity + ity;
        } else {
            emitter_intensities[i] = prev_ity;
        }
        prev_ity = emitter_intensities[i];
    }
    return emitter_intensities;
}


static void emitray(vec3 r_p, vec3 r_d, const XXscene * scene, const XXsceneobj * emitter, vec3 cam_p, vec3 cam_d, XXrenderbuffer * out_rndbuf) {
    vec3 raycol = emitter->mat.col_emit;
    cast_hit_to_screen(r_p, raycol, cam_p, cam_d, scene, out_rndbuf);
    for (int i = 0; i < RAY_DEPTH; i++) {
        vec3 hitpos, hitnrm;
        XXsceneobj * hitobj = 0;
        if (isect_scene(scene, r_p, r_d, &hitobj, &hitpos, &hitnrm)) {
            vec3 obj_albd = hitobj->mat.col_albd;
            raycol = _vec3(raycol.x * obj_albd.x, raycol.y * obj_albd.y, raycol.z * obj_albd.z);
            raycol = vec3_add(raycol, hitobj->mat.col_emit);
            r_p = hitpos;
            r_d = genhemisray(hitnrm);
            cast_hit_to_screen(hitpos, raycol, cam_p, cam_d, scene, out_rndbuf);
        } else {
            break;
        }
    }
}

static void cast_hit_to_screen(vec3 hitpos, vec3 hitcol, vec3 cam_p, vec3 cam_d, const XXscene * scene, XXrenderbuffer * out_rndbuf) {
    float scr_x = NAN, scr_y = NAN;
    if (project_to_camera(cam_p, cam_d, _vec3(0,1,0), hitpos, &scr_x, &scr_y, (float)SCR_W/SCR_H)) {
        if (!is_occluded(scene, cam_p, hitpos)) {
            float i_x = (scr_x + 1.0f) * SCR_W / 2.0f;
            float i_y = (scr_y + 1.0f) * SCR_H / 2.0f;
            add_sample_to_pixels(i_x, i_y, hitcol, out_rndbuf);
        }
    }
}

static void add_sample_to_pixels(float fx, float fy, vec3 col, XXrenderbuffer * out_rndbuf) {
    int i_y = (int)fy;
    int i_x = (int)fx;
    float px = fmodf(fx, 1.0f);
    float py = fmodf(fy, 1.0f);
    add_sample_to_pixel(i_x, i_y, 1, col, out_rndbuf);
    /*
    add_sample_to_pixel(i_x, i_y, (1-px)*(1-py), col, out_rndbuf);
    add_sample_to_pixel(i_x+1, i_y, px*(1-py), col, out_rndbuf);
    add_sample_to_pixel(i_x+1, i_y+1, px*py, col, out_rndbuf);
    add_sample_to_pixel(i_x, i_y+1, (1-px)*py, col, out_rndbuf);
    */
}

    
static inline void add_sample_to_pixel(int i_x, int i_y, float w, vec3 col, XXrenderbuffer * out_rndbuf) {
    if (i_x < 0 || i_y < 0 || i_x >= out_rndbuf->w || i_y >= out_rndbuf->h) return;
    unsigned int * curr_pixel = &out_rndbuf->pixels[i_y*SCR_W+i_x];
    float fr = (float)((*curr_pixel) & 0xff) / 255;
    float fg = (float)(((*curr_pixel)>>8) & 0xff) / 255;
    float fb = (float)(((*curr_pixel)>>16) & 0xff) / 255;
    float * n = &out_rndbuf->sample_counts[i_y*SCR_W+i_x];

    fr = (fr * (*n) + col.x*w) / ((*n) + w); 
    fg = (fg * (*n) + col.y*w) / ((*n) + w); 
    fb = (fb * (*n) + col.z*w) / ((*n) + w);

    unsigned char c_r = (unsigned char)(clamp(fr) * 255);
    unsigned char c_g = (unsigned char)(clamp(fg) * 255);
    unsigned char c_b = (unsigned char)(clamp(fb) * 255);

    *n+=w;
    *curr_pixel = c_r | (c_g << 8) | (c_b << 16);
}

static int is_occluded(const XXscene * scene, vec3 cam_pos, vec3 test_pos) {
    vec3 d = vec3_sub(test_pos, cam_pos);
    float len0 = vec3_length(d);
    vec3_normalize(&d);
    vec3 hitpos, hitnrm;
    XXsceneobj * hitobj = 0;
    if (isect_scene(scene, cam_pos, d, &hitobj, &hitpos, &hitnrm)) {
        float len1 = vec3_length(vec3_sub(hitpos, cam_pos));
        if (len0 - len1 > EPSILON) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

static int project_to_camera(vec3 cam_p, vec3 cam_d, vec3 wld_u, vec3 p, float * out_x, float * out_y, float aspect) {
    float z = vec3_dot(vec3_sub(p, cam_p), cam_d);
    if (z < CAM_NEAR) {
        return 0;
    } 
    float tanhfov = tanf(CAM_FOV / 2.0f);
    float _W = z * aspect * tanhfov;
    float _H = z * tanhfov;

    vec3 cam_l = vec3_cross(wld_u, cam_d);
    vec3_normalize(&cam_l);
    vec3 cam_u = vec3_cross(cam_d, cam_l);

    vec3 Z = vec3_add(cam_p, vec3_mul(cam_d, z));

    *out_x = vec3_dot(vec3_sub(p, Z), cam_l) / _W;
    *out_y = vec3_dot(vec3_sub(p, Z), cam_u) / _H;

    return fabsf(*out_x) <= 1.0f && fabsf(*out_y) <= 1.0f;
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

static void alloc_scene(int obj_count, XXscene * out_scene) {
    *out_scene = scene_alloc(4);
    XXmat mat_nrm = {
        .col_albd = {.9f,.9f,.9f}, .col_emit={0,0,0}
    };
    XXmat mat_lit = {
        .col_albd = {0,0,0}, .col_emit={1,1,1}
    };

    out_scene->objs[0] = _sceneobj_aab_inv(_vec3(0, 3, 0), 10, 3, 10, mat_nrm);
    out_scene->objs[1] = _sceneobj_sphere(_vec3(0, 0, 0), 2.5f, mat_nrm);
    out_scene->objs[2] = _sceneobj_sphere(_vec3(2, 2, 2), 0.5f, mat_nrm);
    out_scene->objs[3] = _sceneobj_sphere(_vec3(8, 2, 8), 1.5f, mat_lit);

}

int main() {

    //cl_run();

    GLFWwindow * window;
    glfw_init(&window, WINDOW_W, WINDOW_H);
    init();

    glfw_run(window, &run);

    release();
    glfwTerminate();

    return 0;
}


static inline unsigned int cast_ray_from_camera(vec3 r_p, vec3 r_d, float x_var, float y_var, vec3 cam_d, const XXscene * scene) { 
    vec3 pos, nrm;
    vec3 c = _vec3(0,0,0);
    for (int i = 0; i < SAMPLES_PER_PIXEL; i++) {
        vec3 d = r_d;
        d.x += (randf() - 0.5f) * x_var;
        d.y += (randf() - 0.5f) * y_var;
        vec3_normalize(&d);
        c = vec3_add(c, computeradiance(r_p, d, scene, 0));
    }
    c = vec3_mul(c, 1.0f / SAMPLES_PER_PIXEL);
    unsigned char f_r = (unsigned char)(clamp(c.x) * 255);
    unsigned char f_g = (unsigned char)(clamp(c.y) * 255);
    unsigned char f_b = (unsigned char)(clamp(c.z) * 255);
    return f_r | (f_g << 8) | (f_b << 16);
}

static vec3 get_pos_from_obj_surface(const XXsceneobj * obj) {
    if (obj->objtype == SPHERE) {
        return vec3_add(obj->sphere.p, vec3_mul(pickspherepos(), obj->sphere.r));
    } else {
        XX_TODO("unimplemented object type: %d", obj->objtype);
    }
}

static vec3 sample_emitter(vec3 pos, const XXscene * scene) {
    XXsceneobj * emitter_obj = &scene->objs[scene->emitter_objs[0]];
    vec3 emitter_pos = get_pos_from_obj_surface(emitter_obj);
    if (!is_occluded(scene, emitter_pos, pos)) {
        return emitter_obj->mat.col_emit;
    } else {
        return _vec3(0,0,0);
    }
}

static vec3 computeradiance(vec3 r_p, vec3 r_d, const XXscene * scene, int depth) {
    if (depth > RAY_DEPTH) {
        return _vec3(0,0,0);
    }
    vec3 hitpos, hitnrm;
    XXsceneobj * hitobj = 0;
    if (isect_scene(scene, r_p, r_d, &hitobj, &hitpos, &hitnrm)) {
        vec3 emit = vec3_mul(hitobj->mat.col_emit, 1);
        vec3 albd = hitobj->mat.col_albd;
        //vec3 rad = _vec3(hitnrm.x,hitnrm.y,hitnrm.z);
        vec3 rad = _vec3(0,0,0);
        for(int i = 0; i < 1; i++) {
            rad = vec3_add(rad, sample_emitter(hitpos, scene));
        }
        rad = vec3_mul(rad, 1.0f / 1);
        //vec3 rad = _vec3(0,0,0);
        //vec3 nr = genhemisray(hitnrm);
        //krad = computeradiance(hitpos, nr, scene, depth+1);
        rad = _vec3(rad.x * albd.x, rad.y * albd.y, rad.z * albd.z);
        return vec3_add(emit, rad);
    }
    return _vec3(100,0,100);
}






