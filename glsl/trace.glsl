
#define M_PI 3.141593f

#define MAX_OBJS 5

uniform float cam_fov = M_PI * 45.0f / 180.0f;
uniform float scr_aspect = 16.0f / 9.0f;

uniform vec3 cam_p, cam_d;
uniform vec3 wld_u = vec3(0,1,0);
uniform vec3 cam_l;
uniform vec3 cam_u;

vec4 get_color() {
    return vec4(0,1,0,1);
}

struct Mat {
    vec3 col_albd, col_emit;
};

struct SceneObj_sphere {
  vec3 p;
  float r;
};
struct SceneObj_aab {
  vec3 p;
  vec3 size;
};

struct Scene {
    int obj_sphere_count;
    SceneObj_sphere obj_sphere_list[MAX_OBJS];
    int obj_aab_count;
    SceneObj_aab obj_aab_list[MAX_OBJS];

    int emitter_objs[MAX_OBJS*2];
    int emitter_objs_count;
};


