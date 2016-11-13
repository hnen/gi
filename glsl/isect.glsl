

int isect_sphere(vec3 s_p, float s_r, vec3 r_p, vec3 r_d, out float t) {
    float a = dot(r_d, r_d);
    vec3 oc = r_p - s_p;
    float b = 2.0f * dot(r_d, oc);
    float c = dot(oc, oc) - s_r * s_r;

    float det = b*b - 4*a*c;
    if (det >= 0) {
        float sdet = sqrt(det);
        float t0 = (-b - sdet) / (2.0f * a);
        if (t0 > 0) {
            t = t0;
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

#define t_plane_s(p_p_x, r_p_x, r_d_x) ((p_p_x - r_p_x) / r_d_x)

int isect_aab(vec3 b_p, float a_x, float a_y, float a_z, vec3 r_p, vec3 r_d, int inv, out float out_t, out vec3 out_inorm) {
    vec3 norm;

    float t_a00 = t_plane_s(b_p.x + a_x, r_p.x, r_d.x);
    float t_a01 = t_plane_s(b_p.x - a_x, r_p.x, r_d.x);

    float tmin0 = min(t_a00, t_a01);
    float tmax0 = max(t_a00, t_a01);

    float t_a10 = t_plane_s(b_p.y + a_y, r_p.y, r_d.y);
    float t_a11 = t_plane_s(b_p.y - a_y, r_p.y, r_d.y);

    float tmin1 = max(tmin0, min(t_a10, t_a11));
    float tmax1 = min(tmax0, max(t_a10, t_a11));

    if (tmin1 > tmax1) return 0;
    
    float t_a20 = t_plane_s(b_p.z + a_z, r_p.z, r_d.z);
    float t_a21 = t_plane_s(b_p.z - a_z, r_p.z, r_d.z);

    float tmin2 = max(tmin1, min(t_a20, t_a21));
    float tmax2 = min(tmax1, max(t_a20, t_a21));

    if (tmin2 > tmax2) return 0;

    if (inv == 0) {
        if (tmin2 > tmin1) {
            out_inorm = vec3(0, 0, t_a20 < t_a21 ? 1 : -1);
        } else if (tmin1 > tmin0) {
            out_inorm = vec3(0, t_a10 < t_a11 ? 1 : -1, 0);
        } else {
            out_inorm = vec3(t_a00 < t_a01 ? 1 : -1, 0, 0);
        }
        out_t = tmin2;
    } else {
        if (tmax2 < tmax1) {
            out_inorm = vec3(0, 0, t_a20 < t_a21 ? 1 : -1);
        } else if (tmax1 < tmax0) {
            out_inorm = vec3(0, t_a10 < t_a11 ? 1 : -1, 0);
        } else {
            out_inorm = vec3(t_a00 < t_a01 ? 1 : -1, 0, 0);
        }
        out_t = tmax2;
    }

    out_inorm = normalize(out_inorm);

    return 1;
}

int isect_aabobj(SceneObj_aab obj, vec3 r_p, vec3 r_d, out float t, out vec3 norm) {
    return isect_aab(obj.p, obj.size.x, obj.size.y, obj.size.z, r_p, r_d, 1, t, norm);
}


int isect_sphereobj(SceneObj_sphere obj, vec3 r_p, vec3 r_d, out float t) {
    return isect_sphere(obj.p, obj.r, r_p, r_d, t);
}

int isect_scene(Scene scene, vec3 r_p, vec3 r_d, out float _t, out vec3 _norm) {
    int _i = -1;
    float mint = 1./0.;
    for (int i = 0; i < scene.obj_sphere_count; i++) {
        float t = 0;
        int isect = isect_sphere(scene.obj_sphere_list[i].p, scene.obj_sphere_list[i].r, r_p, r_d, t);
        if (isect!=0) {
            if (t < mint) {
                mint = t;
                _t = t;
                _i = i;
                vec3 norm = normalize(r_p + r_d * t - scene.obj_sphere_list[i].p);
                _norm = norm;
            }
        }
    }
    for (int i = 0; i < scene.obj_aab_count; i++) {
        float t = 0;
        vec3 norm;
        int isect = isect_aabobj(scene.obj_aab_list[i], r_p, r_d, t, norm);
        if (isect!=0) {
            if (t < mint) {
                mint = t;
                _i = i + MAX_OBJS;
                _t = t;
                _norm = norm;
            }
        }
    }

    return _i;
}









