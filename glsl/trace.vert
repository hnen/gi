#version 400

#include "trace.glsl"


in vec3 vp;
out vec3 _r_d;

void main() {
    float tanh = tan(cam_fov * 0.5f);
    float tanw = scr_aspect * tanh;

    vec3 i = cam_l * vp.x * tanw;
    vec3 j = cam_u * vp.y * tanh;

    _r_d = cam_d + i + j;

    gl_Position = vec4(vp, 1.0);
}


