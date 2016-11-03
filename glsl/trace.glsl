
#define MAX_OBJS 5

vec4 get_color() {
    return vec4(0,1,0,1);
}

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


