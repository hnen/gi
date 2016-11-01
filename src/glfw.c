//
//  glfw.c
//  opencl-xcode
//
//  Created by Harri Hätinen on 16/10/2016.
//  Copyright © 2016 Harri Hätinen. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "glfw.h"

void glfw_run(GLFWwindow *window, XXcallback runCallback) {
    double t0 = glfwGetTime();
    int framecount = 0;
    while (!glfwWindowShouldClose(window)) {
        runCallback();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
}

void glfw_init(GLFWwindow ** out_window, int w, int h, int * out_w, int * out_h) {
    
    if (!glfwInit()) {
        fprintf(stderr, "Couldn't initialize GLFW");
        exit(1);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow * window = glfwCreateWindow(w, h, "", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Couldn't create GLFW window");
        glfwTerminate();
        exit(1);
    }
    
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    glewInit();
    
    const GLubyte* renderer_name = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("OpenGL Renderer: %s\n", renderer_name);
    printf("OpenGL version supported %s\n", version);

    glfwGetWindowSize(window, out_w, out_h);
    
    *out_window = window;
}
