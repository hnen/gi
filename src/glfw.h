//
//  glfw.h
//  opencl-xcode
//
//  Created by Harri Hätinen on 16/10/2016.
//  Copyright © 2016 Harri Hätinen. All rights reserved.
//

#ifndef glfw_h
#define glfw_h

#include "common.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void glfw_init(GLFWwindow ** out_window, int w, int h);
void glfw_run(GLFWwindow *window, XXcallback runCallback);

#endif /* glfw_h */
