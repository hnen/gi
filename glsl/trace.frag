#version 400

#include "trace.glsl"

vec3 computeradiance(vec3 r_p, vec3 r_d, Scene scene) {
  return vec3(0,0,0);
}

in vec3 _r_d;

layout(location = 0) out vec4 frag_colour;
uniform Scene g_scene;

/*
vec3 computeradiance(vec3 r_p, vec3 r_d, Scene scene) {
    vec3 hitpos, hitnrm;
    XXsceneobj * hitobj = 0;
    if (isect_scene(scene, r_p, r_d, &hitobj, &hitpos, &hitnrm)) {
        vec3 emit = vec3_mul(hitobj->mat.col_emit, 1);
        vec3 albd = hitobj->mat.col_albd;
        vec3 rad = _vec3(0,0,0);
        vec3 testhitpos = vec3_add(hitpos, vec3_mul(hitnrm, EPSILON));
        for(int i = 0; i < 1; i++) {
            rad = vec3_add(rad, sample_emitter(testhitpos, hitnrm, scene));
        }
        rad = vec3_mul(rad, 1.0f / 1);
        rad = _vec3(rad.x * albd.x, rad.y * albd.y, rad.z * albd.z);
        return vec3_add(emit, rad);
    }
    return _vec3(100,0,100);
}
*/

vec3 cast_ray_from_camera(vec3 r_p, vec3 r_d, Scene scene) { 
    return r_d;
    //return computeradiance(r_p, r_d, scene);
}

void main() {
    vec3 r_d = normalize(_r_d);
    vec3 col = cast_ray_from_camera(cam_p, r_d, g_scene);
    frag_colour = vec4(col,1);
}




