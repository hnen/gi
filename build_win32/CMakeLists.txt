cmake_minimum_required (VERSION 2.6)
project (gi)

FIND_PATH(GLEW_INCLUDE GL/glew.h ../deps/glew-2.0.0/include NO_DEFAULT_PATH)
FIND_PATH(GLFW_INCLUDE GLFW/glfw3.h ../deps/glfw-3.2.1.bin.WIN32/include NO_DEFAULT_PATH)

FIND_LIBRARY(GLFW_LIBRARY glfw3 ../deps NO_DEFAULT_PATH REQUIRED)
FIND_LIBRARY(GLEW_LIBRARY glew32 ../deps/glew-2.0.0/lib NO_DEFAULT_PATH REQUIRED)
FIND_LIBRARY(GL_LIBRARY OpenGL32)
#FIND_LIBRARY(CL_LIBRARY OpenCL32)

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DDEBUG)

file(GLOB SOURCES "../src/*.c")
add_executable(gi ${SOURCES})

add_definitions("-m32" "-static" "-std=c99")

include_directories(${GLEW_INCLUDE} ${GLFW_INCLUDE})
target_link_libraries(gi ${GLFW_LIBRARY} ${GLEW_LIBRARY} ${GL_LIBRARY} "-static")

