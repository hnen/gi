
#version 400
in vec3 vp;
out vec2 uv;
void main() {
  gl_Position = vec4(vp, 1.0);
  uv = (vp.xy + vec2(1.0f, 1.0f)) * 0.5f;
}


