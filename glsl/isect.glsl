

bool isect_sphere(vec3 s_p, float s_r, vec3 r_p, vec3 r_d, out float t) {
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
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}



