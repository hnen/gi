#version 400

#define EMITTER_SAMPLES 10 

#include "trace.glsl"

#include "isect.glsl"

vec3 computeradiance(vec3 r_p, vec3 r_d, Scene scene) {
  return vec3(0,0,0);
}

in vec3 _r_d;

layout(location = 0) out vec4 frag_colour;
uniform Scene g_scene;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 pickspherepos(vec2 x) {
    float h = rand(x) * 2.0f - 1.0f;
    float a = rand(x*2) * 6.28f;
    float r0 = sqrt(1 - h*h);
    return vec3(sin(a)*r0, h, cos(a)*r0);
}


vec3 cast_ray_from_camera(vec3 r_p, vec3 r_d, Scene scene) { 

    float t;
    vec3 norm;

    if (isect_scene(scene, r_p, r_d, t, norm)!=-1) {
        vec3 ppos = r_p + r_d * t + norm * 0.0001f;

        vec3 rad = vec3(0,0,0);
        for(int i = 0; i < EMITTER_SAMPLES; i++) {
            int _ei = i%scene.emitter_objs_count;
            int ei = scene.emitter_objs[_ei];
            SceneObj_sphere eobj = scene.obj_sphere_list[ei];
            float t0;
            vec3 norm0;
            vec3 empos = eobj.p + pickspherepos(r_d.xy*(i+10)) * eobj.r;
            vec3 edir = normalize(empos - ppos);
            int iobj = isect_scene(scene, ppos, edir, t0, norm0);
            if (iobj==ei) {
                rad += vec3(1,1,1);
            }
        }
        rad /= EMITTER_SAMPLES;
        return rad;
    } else {
        return vec3(0,0,0);
    }


    //return r_d;
    //return computeradiance(r_p, r_d, scene);
}

void main() {
    vec3 r_d = normalize(_r_d);
    vec3 col = cast_ray_from_camera(cam_p, r_d, g_scene);
    frag_colour = vec4(col,1);
}




