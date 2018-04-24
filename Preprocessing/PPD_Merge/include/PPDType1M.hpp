#ifndef PPDTYPE_H
#define PPDTYPE_H

#include "RootInc.hpp"

class PPDType1M {
public:
    struct PPD_T {
        Double_t    pitch_angle;
        Double_t    yaw_angle;
        Double_t    roll_angle;
        Double_t    pitch_angle_v;
        Double_t    yaw_angle_v;
        Double_t    roll_angle_v;
        Double_t    orbit_agl_v;
        Double_t    longitude;
        Double_t    latitude;
        Double_t    geocentric_d;
        Double_t    ship_time_sec;
        Double_t    utc_time_sec;
        Char_t      utc_time_str[32];
        Int_t       flag_of_pos;
        Double_t    wgs84_x;
        Double_t    wgs84_y;
        Double_t    wgs84_z;
        Double_t    wgs84_x_v;
        Double_t    wgs84_y_v;
        Double_t    wgs84_z_v;
        Double_t    det_z_lat;
        Double_t    det_z_lon;
        Double_t    det_z_ra;
        Double_t    det_z_dec;
        Double_t    det_x_lat;
        Double_t    det_x_lon;
        Double_t    det_x_ra;
        Double_t    det_x_dec;
        Double_t    earth_lat;
        Double_t    earth_lon;
        Double_t    earth_ra;
        Double_t    earth_dec;
        Double_t    sun_ra;
        Double_t    sun_dec;
    };

public:
    static void bind_ppd_tree(TTree* t_ppd_tree, PPD_T& t_ppd);

    static void build_ppd_tree(TTree* t_ppd_tree, PPD_T& t_ppd);
};

#endif
