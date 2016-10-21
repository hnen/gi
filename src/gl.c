//
//  xxGl.c
//  opencl-xcode
//
//  Created by Harri Hätinen on 16/10/2016.
//  Copyright © 2016 Harri Hätinen. All rights reserved.
//

#include "gl.h"

XXerr gl_compileShader(GLuint shadertype, const char * source, GLuint * out_handle);

XXerr gl_createShaderProgram(const char * vs_src, const char * fs_src, struct XXshaderprogram * out_program) {
    XX_E(gl_compileShader(GL_VERTEX_SHADER, vs_src, &out_program->vs));
    XX_E(gl_compileShader(GL_FRAGMENT_SHADER, fs_src, &out_program->fs));
    
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

XXerr gl_releaseShaderProgram(struct XXshaderprogram program) {
    GL_E(glDeleteShader(program.fs));
    GL_E(glDeleteShader(program.vs));
    GL_E(glDeleteProgram(program.program));
    program.fs = -1;
    program.vs = -1;
    program.program = -1;
    return 0;
}

XXerr gl_compileShader(GLuint shadertype, const char * source, GLuint * out_handle) {
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
        fprintf(stderr, "!!Error with shader compilation:\n%s\n", str);
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

void gl_createTexture(GLuint wrap, GLuint filter, GLuint * out_tex) {
    GLuint tex;
    GL_E(glGenTextures(1, &tex));
    GL_E(glBindTexture(GL_TEXTURE_2D, tex));
    GL_E(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap));
    GL_E(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap));
    GL_E(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
    GL_E(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
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












