#version 400

#include "trace.glsl"

uniform float cam_fov = M_PI * 45.0f / 180.0f;
uniform float scr_aspect = 16.0f / 9.0f;

uniform vec3 cam_p, cam_d;
uniform vec3 wld_u = vec3(0,1,0);

in vec3 vp;
out vec2 uv;
void main() {
    vec3 cam_l = cross(wld_u, cam_d);
    vec3 cam_u = cross(cam_d, cam_l);

    float tanh = tan(cam_fov * 0.5f);
    float tanw = scr_aspect * tanh;

    gl_Position = vec4(vp, 1.0);
    uv = (vp.xy + vec2(1.0f, 1.0f)) * 0.5f;
}


