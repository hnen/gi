
#version 400

#define MAX_OBJS 5

struct SceneObj_sphere {
  vec3 p;
  float r;
};
struct SceneObj_aab {
  vec3 p;
  float x, y, z;
};

struct Scene {
    int obj_sphere_count;
    SceneObj_sphere obj_sphere_list[MAX_OBJS];
    int obj_aab_count;
    SceneObj_sphere obj_aab_list[MAX_OBJS];

    int emitter_objs[MAX_OBJS*2];
    int emitter_objs_count;
};

vec3 computeradiance(vec3 r_p, vec3 r_d, Scene scene) {
  return vec3(0,0,0);
}

in vec2 uv;
layout(location = 0) out vec4 frag_colour;
uniform Scene g_scene;

void main() {
    frag_colour = vec4(1,0,0,1);
}


