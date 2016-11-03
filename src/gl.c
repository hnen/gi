//
//  xxGl.c
//  opencl-xcode
//
//  Created by Harri Hätinen on 16/10/2016.
//  Copyright © 2016 Harri Hätinen. All rights reserved.
//

#include "gl.h"
#include "glsl_preproc.h"
#include "common.h"

XXerr gl_compileShader(GLuint shadertype, const char * source, const char * name, GLuint * out_handle);

XXerr _gl_createShaderProgram(const char * vs_src, const char * vs_name, const char * fs_src, const char *fs_name, struct XXshaderprogram * out_program) {
    XX_E(gl_compileShader(GL_VERTEX_SHADER, vs_src, vs_name, &out_program->vs));
    XX_E(gl_compileShader(GL_FRAGMENT_SHADER, fs_src, fs_name, &out_program->fs));
    
    GLuint prg = glCreateProgram();
    GL_E(glAttachShader(prg, out_program->fs));
    GL_E(glAttachShader(prg, out_program->vs));
    glLinkProgram(prg);
    
    GLint link_status = -1;
    glGetProgramiv(prg, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
        GLint info_len = -1;
        glGetProgramiv(prg, GL_INFO_LOG_LENGTH, &info_len);
        char * str = (char*)xxalloc(info_len);
        glGetProgramInfoLog(prg, info_len, &info_len, str);
        fprintf(stderr, "!!Error with shader program linkage:\n%s\n", str);
        glDeleteProgram(prg);
        return 1;
    }
    
    out_program->program = prg;
    
    return 0;
}

XXerr gl_createShaderProgramFromFile(const char * vs_filename, const char * fs_filename, struct XXshaderprogram * out_program) {
   char *vs_src, *fs_src;
   size_t vs_size, fs_size;
   //file_read(vs_filename, &vs_src, &vs_size); 
   //file_read(fs_filename, &fs_src, &fs_size); 

   glsl_preproc_process(vs_filename, &vs_src);
   glsl_preproc_process(fs_filename, &fs_src);

   //printf("done!\n");


   XXerr ret = _gl_createShaderProgram(vs_src, vs_filename, fs_src, fs_filename, out_program);

   xxfree(vs_src);
   xxfree(fs_src);
   
   return ret;
}

XXerr gl_createShaderProgram(const char * vs_src, const char * fs_src, struct XXshaderprogram * out_program) {
    return _gl_createShaderProgram(vs_src, "(inline vertex shader)", fs_src, "(inline fragment shader)", out_program);
}


XXerr gl_releaseShaderProgram(struct XXshaderprogram program) {
    GL_E(glDeleteShader(program.fs));
    GL_E(glDeleteShader(program.vs));
    GL_E(glDeleteProgram(program.program));
    program.fs = -1;
    program.vs = -1;
    program.program = -1;
    return 0;
}

XXerr gl_compileShader(GLuint shadertype, const char * source, const char *name, GLuint * out_handle) {
    GLuint handle = glCreateShader(shadertype);
    GL_E(glShaderSource(handle, 1, &source, NULL));
    GL_E(glCompileShader(handle));
    
    GLint iscompiled = -1;
    GL_E(glGetShaderiv(handle, GL_COMPILE_STATUS, &iscompiled));
    if (iscompiled == GL_FALSE) {
        GLint loglength = -1;
        GL_E(glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &loglength));
        
        char * str = (char*)xxalloc(loglength);
        GL_E(glGetShaderInfoLog(handle, loglength, &loglength, str));
        fprintf(stderr, "!!Error with shader compilation %s:\n%s\n", name, str);
        xxfree(str);
        
        GL_E(glDeleteShader(handle));
        
        return 1;
    }
    
    *out_handle = handle;
    
    return 0;
}

void gl_createVAO(GLuint * out_handle) {
    GLuint vao;
    GL_E(glGenVertexArrays(1, &vao));
    GL_E(glBindVertexArray(vao));
    *out_handle = vao;
}

void gl_releaseVAO(GLuint handle) {
    GL_E(glDeleteVertexArrays(1, &handle));
}

void gl_createVBO(GLuint * out_handle, const void * data, int data_size) {
    GLuint vbo;
    GL_E(glGenBuffers(1, &vbo));
    GL_E(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_E(glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW));
    *out_handle = vbo;
}

void gl_releaseVBO(GLuint handle){
    GL_E(glDeleteBuffers(1, &handle));
}

void gl_renderVBO(GLuint vbo, GLuint use_shader_program, int tricount) {
    GL_E(glEnableVertexAttribArray(0));
    GL_E(glDisable(GL_DEPTH_TEST));
    GL_E(glUseProgram(use_shader_program));
    GL_E(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_E(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    GL_E(glDrawArrays(GL_TRIANGLES, 0, tricount * 3));
    GL_E(glDisableVertexAttribArray(0));
}

void gl_createFramebuffer(GLuint txt_texture, GLuint * out_fb) {
    GLuint ret;
    GL_E(glGenFramebuffers(1, &ret));
    GL_E(glBindFramebuffer(GL_FRAMEBUFFER, ret));
    GL_E(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ret, 0));
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    GL_E(glDrawBuffers(1, drawBuffers));
    *out_fb = ret;
}

void gl_deleteFramebuffer(GLuint fb) {
    GL_E(glDeleteFramebuffers(1, &fb));
}

void gl_createTexture(int w, int h, GLuint wrap, GLuint filter, GLuint * out_tex) {
    GLuint tex;
    GL_E(glGenTextures(1, &tex));
    GL_E(glBindTexture(GL_TEXTURE_2D, tex));
    GL_E(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap));
    GL_E(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap));
    GL_E(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
    GL_E(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
    GL_E(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
    *out_tex = tex;
}

void gl_deleteTexture(GLuint tex) {
    GL_E(glDeleteTextures(1, &tex));
}

void gl_setTextureData(GLuint tex, int w, int h, const void * data) {
    GL_E(glBindTexture(GL_TEXTURE_2D, tex));
    GL_E(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
}

void gl_bindTexture(GLuint tex, GLuint texture_unit) {
    GL_E(glActiveTexture(texture_unit));
    GL_E(glBindTexture(GL_TEXTURE_2D, tex));
}

void gl_handleErrors(const char * filename, int line, const char * call) {
    int had_errors = 0;
    for(GLenum err; (err = glGetError()) != GL_NO_ERROR;) {
        const GLubyte * err_str = gluErrorString(err);
        fprintf(stderr, "Error \"%s\"(0x%04x) on call \"%s\". %s:%d", err_str, err, call, filename, line);
        had_errors = 1;
    }
    if (had_errors) {
        __builtin_trap();
    }
}












