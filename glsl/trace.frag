
#version 400

in vec2 uv;
layout(location = 0) out vec4 frag_colour;
uniform sampler2D tex;

void main() {
    frag_colour = vec4(1,0,0,1);
}


