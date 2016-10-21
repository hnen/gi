#include <stdio.h>
#include <stdlib.h>

#include "glfw.h"
#include "cl.h"
#include "gl.h"
#include "obj.h"

#define W 1024
#define H (1024*9/16)

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

static void createTextureData(char * out_data, int w, int h);


static void init() {
  XXobj obj;
  obj_parse("./data/kuutiot.obj", &obj);
  obj_printf(&obj);


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
    createTextureData(data, W, H);
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








