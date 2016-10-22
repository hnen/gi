#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "glfw.h"
#include "cl.h"
#include "gl.h"
#include "obj.h"
#include "isect.h"
#include "scene.h"

#define W 1024
#define H ((W)*9/16)

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
static void renderScene(const XXobj * obj, char * out_data, int w, int h, float t);
static inline unsigned int evalpixel(vec3 r_p, vec3 r_d, vec3 cam_d, const XXscene * scene);
static void allocrandomscene(int obj_count, XXscene * out_scene);

XXscene scene;

static void init() {
  obj_parse("./data/kuutiot.obj", &g_obj);
  obj_printf(&g_obj);

  srand(time(0));
  allocrandomscene(10, &scene);

  XX_E(gl_createShaderProgram(vertex_shader, fragment_shader, &g_program));
  gl_createTexture(GL_CLAMP_TO_EDGE, GL_LINEAR, &g_tex);
}

static void release() {
  scene_free(scene);
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
    //createTextureData(data, W, H);
    renderScene(&g_obj, data, W, H, glfwGetTime());
    gl_setTextureData(g_tex, W, H, data);
    xxfree(data);
    
    GL_E(glUseProgram(g_program.program));
    GL_E(glUniform1i(glGetUniformLocation(g_program.program, "tex"), 0));
    
    gl_renderVBO(vbo, g_program.program, 2);
    
    gl_releaseVBO(vbo);
    gl_releaseVAO(vao);
    
}

static void createTextureData(char * out_data, int w, int h) {
    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            for (int c = 0; c < 4; c++) {
                *(out_data++) = x^y;
            }
        }
    }
}


static void renderScene(const XXobj * obj, char * out_data, int w, int h, float t) {

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

  for(int y = 0; y < h; y++) {
      for(int x = 0; x < w; x++) { 
        vec3 i = vec3_mul(cam_l, (float)(x-w/2) * 0.003f);
        vec3 j = vec3_mul(cam_u, (float)(y-h/2) * 0.003f);
        vec3 r_d = vec3_add(cam_d, vec3_add(i, j));
        vec3_normalize(&r_d);

        *(buf++) = evalpixel(cam_p, r_d, cam_d, &scene);
      }
    }
}

static inline unsigned int evalpixel(vec3 r_p, vec3 r_d, vec3 cam_d, const XXscene * scene) { 
  vec3 pos, nrm;
  if (isect_scene(scene, r_p, r_d, &pos, &nrm)) {
    float nd = vec3_dot(nrm, cam_d);
    float f = (-nd);
    unsigned char f_c = (unsigned char)(clamp(f) * 255);
    return f_c | (f_c << 8) | (f_c << 16);
  } else {
    return 0;
  }
}

/*
static inline unsigned int evalpixel(vec3 r_p, vec3 r_d, vec3 cam_d, const XXobj * obj) { 
  vec3 pos, nrm;
  if (isect_obj(obj, r_p, r_d, &pos, &nrm)) {
    float f = (-vec3_dot(nrm, cam_d)) + 0.2f;
    unsigned char f_c = (unsigned char)(clamp(f) * 255);
    return f_c | (f_c << 8) | (f_c << 16);
  } else {
    return 0;
  }
}
*/

static void allocrandomscene(int obj_count, XXscene * out_scene) {
  *out_scene = scene_alloc(obj_count);
  XXmat objmat = { .col_albd={1,1,1}, .col_emit={0,0,0} };
  XXmat litmat = { .col_albd={0,0,0}, .col_emit={1,1,1} };
  for (int i = 0; i < obj_count; i++) {
    XXsceneobj obj = {
      .p={(randf() - 0.5f) * 7.5f, (randf() - 0.5f) * 7.5f, (randf() - 0.5f) * 7.5f},
      .r=randf()*0.5f+1.0f,
      .mat = i < obj_count-1 ? objmat : litmat
    };
    out_scene->objs[i] = obj;
  }

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








