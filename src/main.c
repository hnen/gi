#include <stdio.h>
#include <stdlib.h>

#include "glfw.h"
#include "cl.h"
#include "gl.h"
#include "obj.h"
#include "isect.h"

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


static void init() {
  obj_parse("./data/kuutio.obj", &g_obj);
  obj_printf(&g_obj);


  XX_E(gl_createShaderProgram(vertex_shader, fragment_shader, &g_program));
  gl_createTexture(GL_CLAMP_TO_EDGE, GL_LINEAR, &g_tex);
}

static void release() {
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

static inline unsigned int evalpixel(vec3 r_p, vec3 r_d, vec3 cam_d, XXobj * obj);

static void renderScene(const XXobj * obj, char * out_data, int w, int h, float t) {

  vec3 cam_p = {sinf(t) * 5.0f, 3, cosf(t) * 5.0f};
  vec3 cam_dst = {0, 0, 0};
  //vec3 cam_d = {0, -1, 1};
  //vec3 cam_d = {-1, -1, 1};
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

        *(buf++) = evalpixel(cam_p, r_d, cam_d, obj);
      }
    }
}

static inline unsigned int evalpixel(vec3 r_p, vec3 r_d, vec3 cam_d, XXobj * obj) { 
  vec3 pos, nrm;
  /*
  int i = ((int)(glfwGetTime() * 10.0f)) % obj->f_count;
  int i0 = obj->f_ind[i*3+0];
  int i1 = obj->f_ind[i*3+1];
  int i2 = obj->f_ind[i*3+2];
  vec3 p0 = obj->v_pos[i0];
  vec3 p1 = obj->v_pos[i1];
  vec3 p2 = obj->v_pos[i2]; 
  float x;
  if (isect_tri(p0, p1, p2, r_p, r_d, &x)) {
  */
  if (isect_obj(obj, r_p, r_d, &pos, &nrm)) {
    float f = fabsf(vec3_dot(nrm, cam_d));
    //float f = 1.0f;
    unsigned char f_c = (unsigned char)(clamp(f) * 255);
    return f_c | (f_c << 8) | (f_c << 16);
  } else {
    return 0;
  }
}

/*
static inline unsigned int evalpixel(vec3 r_p, vec3 r_d, vec3 cam_d){ 
  vec3 s_p = {0, 2, 15};
  float s_r = 10;
  vec3 ipos0, inorm0, ipos1, inorm1;
  if (isect_sphere(s_p, s_r, r_p, r_d, &ipos0, &inorm0, &ipos1, &inorm1)) {
    vec3_normalize(&inorm0);
    float f = (fabsf(vec3_dot(inorm0, cam_d)));
    unsigned char f_c = (unsigned char)(f * 255);
    return f_c | (f_c << 8) | (f_c << 16);
  } else {
    return 0;
  }
}
*/

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








