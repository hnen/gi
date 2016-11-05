#version 400

#include "trace.glsl"

vec3 computeradiance(vec3 r_p, vec3 r_d, Scene scene) {
  return vec3(0,0,0);
}

in vec2 uv;
layout(location = 0) out vec4 frag_colour;
uniform Scene g_scene;

void main() {
    frag_colour = get_color();
}


