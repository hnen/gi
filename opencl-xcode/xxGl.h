//
//  xxGl.h
//  opencl-xcode
//
//  Created by Harri Hätinen on 16/10/2016.
//  Copyright © 2016 Harri Hätinen. All rights reserved.
//

#ifndef xxGl_h
#define xxGl_h

#include <stdlib.h>
#include <stdio.h>
#include "glfw.h"
#include "xxCommon.h"

#define GL_E(...) { __VA_ARGS__; gl_handleErrors(__FILE__, __LINE__, #__VA_ARGS__); }
//#define GL_E(...) { __VA_ARGS__; gl_handleErrors(__FILE__, __LINE__, ""); }

struct XXshaderprogram {
    GLuint vs, fs, program;
};

XXerr gl_releaseShaderProgram(struct XXshaderprogram program);
XXerr gl_createShaderProgram(const char * vs_src, const char * fs_src, struct XXshaderprogram * out_program);

void gl_createVAO(GLuint * out_handle);
void gl_releaseVAO(GLuint handle);
void gl_createVBO(GLuint * out_handle, const void * data, int data_size);
void gl_releaseVBO(GLuint handle);
void gl_renderVBO(GLuint vbo, GLuint use_shader_program, int tricount);

void gl_createTexture(GLuint wrap, GLuint filter, GLuint * out_tex);
void gl_deleteTexture(GLuint tex);
void gl_setTextureData(GLuint tex, int w, int h, const void * data);
void gl_bindTexture(GLuint tex, GLuint texture_unit);

void gl_handleErrors(const char * filename, int line, const char * call);

#endif /* xxGl_h */
