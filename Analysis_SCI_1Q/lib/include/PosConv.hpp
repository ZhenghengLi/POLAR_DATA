#include "RootInc.hpp"

double deg_to_rad(double deg) {
    return deg / 180.0 * TMath::Pi();
}

double rad_to_deg(double rad) {
    return rad / TMath::Pi() * 180.0;
}

TVectorD vec_normalize(TVectorD vec) {
    TVectorD vec_res(3);
    if (vec.GetNrows() != 3) return vec_res;
    double r = TMath::Sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    for (int i = 0; i < 3; i++) {
        vec_res[i] = vec[i] / r;
    }
    return vec_res;
}

double vec_dot(TVectorD vec1, TVectorD vec2) {
    if (vec1.GetNrows() != 3 || vec2.GetNrows() != 3) {
        return 0;
    } else {
        return (vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2]);
    }
}

TVectorD vec_cross(TVectorD vec1, TVectorD vec2) {
    TVectorD vec_res(3);
    if (vec1.GetNrows() != 3 || vec2.GetNrows() != 3) return vec_res;
    vec_res[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    vec_res[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
    vec_res[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
    return vec_res;
}

TVectorD radec_to_vec(double ra, double dec) {
    TVectorD vec_res(3);
    double theta = deg_to_rad(90 - dec);
    double pha   = deg_to_rad(ra / 24 * 360);
    vec_res[0] = TMath::Sin(theta) * TMath::Cos(pha);
    vec_res[1] = TMath::Sin(theta) * TMath::Sin(pha);
    vec_res[2] = TMath::Cos(theta);
    return vec_res;
}

double wgs84_to_latitude(double wgs84_x, double wgs84_y, double wgs84_z) {
    double r = TMath::Sqrt(wgs84_x * wgs84_x + wgs84_y * wgs84_y + wgs84_z * wgs84_z);
    return (90 - rad_to_deg(TMath::ACos(wgs84_z / r)));
}

double wgs84_to_longitude(double wgs84_x, double wgs84_y, double wgs84_z) {
    return rad_to_deg(TMath::ATan2(wgs84_y, wgs84_x));
}

double radec_to_theta(double ra_z, double dec_z, double ra_x, double dec_x, double ra_src, double dec_src) { // ra(hour), dec(degree)
    TVectorD vec_z = radec_to_vec(ra_z, dec_z);
    TVectorD vec_s = vec_normalize(radec_to_vec(ra_src, dec_src));
    double cos_theta = vec_dot(vec_s, vec_z);
    return rad_to_deg(TMath::ACos(cos_theta));
}

double radec_to_pha(double ra_z, double dec_z, double ra_x, double dec_x, double ra_src, double dec_src) { // ra(hour), dec(degree)
    TVectorD vec_x = radec_to_vec(ra_x, dec_x);
    TVectorD vec_z = radec_to_vec(ra_z, dec_z);
    TVectorD vec_y = vec_cross(vec_z, vec_x);
    TVectorD vec_s = vec_normalize(radec_to_vec(ra_src, dec_src));
    double pha_x = vec_dot(vec_s, vec_x);
    double pha_y = vec_dot(vec_s, vec_y);
    double pha   = rad_to_deg(TMath::ATan2(pha_y, pha_x));
    return (pha < 0 ? pha + 360.0 : pha);
}

