cmake_minimum_required (VERSION 2.6)
project (gi)

FIND_PATH(GLEW_INCLUDE GL/glew.h)

FIND_LIBRARY(GLFW_LIBRARY glfw3)
FIND_LIBRARY(GLEW_LIBRARY GLEW)
FIND_LIBRARY(GL_LIBRARY OpenGL)
FIND_LIBRARY(CL_LIBRARY OpenCL)

file(GLOB SOURCES "../src/*.c")
add_executable(gi ${SOURCES})

include_directories(${GLEW_INCLUDE})
target_link_libraries(gi ${GLFW_LIBRARY} ${GLEW_LIBRARY} ${GL_LIBRARY} ${CL_LIBRARY})

