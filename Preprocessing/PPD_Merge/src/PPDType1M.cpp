#include "PPDType1M.hpp"

void PPDType1M::bind_ppd_tree(TTree* t_ppd_tree, PPD_T& t_ppd) {
    t_ppd_tree->SetBranchAddress("pitch_angle",         &t_ppd.pitch_angle          );
    t_ppd_tree->SetBranchAddress("yaw_angle",           &t_ppd.yaw_angle            );
    t_ppd_tree->SetBranchAddress("roll_angle",          &t_ppd.roll_angle           );
    t_ppd_tree->SetBranchAddress("pitch_angle_v",       &t_ppd.pitch_angle_v        );
    t_ppd_tree->SetBranchAddress("yaw_angle_v",         &t_ppd.yaw_angle_v          );
    t_ppd_tree->SetBranchAddress("roll_angle_v",        &t_ppd.roll_angle_v         );
    t_ppd_tree->SetBranchAddress("longitude",           &t_ppd.longitude            );
    t_ppd_tree->SetBranchAddress("latitude",            &t_ppd.latitude             );
    t_ppd_tree->SetBranchAddress("geocentric_d",        &t_ppd.geocentric_d         );
    t_ppd_tree->SetBranchAddress("ship_time_sec",       &t_ppd.ship_time_sec        );
    t_ppd_tree->SetBranchAddress("utc_time_sec",        &t_ppd.utc_time_sec         );
    t_ppd_tree->SetBranchAddress("utc_time_str",         t_ppd.utc_time_str         );
    t_ppd_tree->SetBranchAddress("flag_of_pos",         &t_ppd.flag_of_pos          );
    t_ppd_tree->SetBranchAddress("wgs84_x",             &t_ppd.wgs84_x              );
    t_ppd_tree->SetBranchAddress("wgs84_y",             &t_ppd.wgs84_y              );
    t_ppd_tree->SetBranchAddress("wgs84_z",             &t_ppd.wgs84_z              );
    t_ppd_tree->SetBranchAddress("wgs84_x_v",           &t_ppd.wgs84_x_v            );
    t_ppd_tree->SetBranchAddress("wgs84_y_v",           &t_ppd.wgs84_y_v            );
    t_ppd_tree->SetBranchAddress("wgs84_z_v",           &t_ppd.wgs84_z_v            );
    t_ppd_tree->SetBranchAddress("det_z_lat",           &t_ppd.det_z_lat            );
    t_ppd_tree->SetBranchAddress("det_z_lon",           &t_ppd.det_z_lon            );
    t_ppd_tree->SetBranchAddress("det_z_ra",            &t_ppd.det_z_ra             );
    t_ppd_tree->SetBranchAddress("det_z_dec",           &t_ppd.det_z_dec            );
    t_ppd_tree->SetBranchAddress("det_x_lat",           &t_ppd.det_x_lat            );
    t_ppd_tree->SetBranchAddress("det_x_lon",           &t_ppd.det_x_lon            );
    t_ppd_tree->SetBranchAddress("det_x_ra",            &t_ppd.det_x_ra             );
    t_ppd_tree->SetBranchAddress("det_x_dec",           &t_ppd.det_x_dec            );
    t_ppd_tree->SetBranchAddress("earth_lat",           &t_ppd.earth_lat            );
    t_ppd_tree->SetBranchAddress("earth_lon",           &t_ppd.earth_lon            );
    t_ppd_tree->SetBranchAddress("earth_ra",            &t_ppd.earth_ra             );
    t_ppd_tree->SetBranchAddress("earth_dec",           &t_ppd.earth_dec            );
    t_ppd_tree->SetBranchAddress("sun_ra",              &t_ppd.sun_ra               );
    t_ppd_tree->SetBranchAddress("sun_dec",             &t_ppd.sun_dec              );
}


void PPDType1M::build_ppd_tree(TTree* t_ppd_tree, PPD_T& t_ppd) {
    t_ppd_tree->Branch("pitch_angle",         &t_ppd.pitch_angle,          "pitch_angle/D"        );
    t_ppd_tree->Branch("yaw_angle",           &t_ppd.yaw_angle,            "yaw_angle/D"          );
    t_ppd_tree->Branch("roll_angle",          &t_ppd.roll_angle,           "roll_angle/D"         );
    t_ppd_tree->Branch("pitch_angle_v",       &t_ppd.pitch_angle_v,        "pitch_angle_v/D"      );
    t_ppd_tree->Branch("yaw_angle_v",         &t_ppd.yaw_angle_v,          "yaw_angle_v/D"        );
    t_ppd_tree->Branch("roll_angle_v",        &t_ppd.roll_angle_v,         "roll_angle_v/D"       );
    t_ppd_tree->Branch("longitude",           &t_ppd.longitude,            "longitude/D"          );
    t_ppd_tree->Branch("latitude",            &t_ppd.latitude,             "latitude/D"           );
    t_ppd_tree->Branch("geocentric_d",        &t_ppd.geocentric_d,         "geocentric_d/D"       );
    t_ppd_tree->Branch("ship_time_sec",       &t_ppd.ship_time_sec,        "ship_time_sec/D"      );
    t_ppd_tree->Branch("utc_time_sec",        &t_ppd.utc_time_sec,         "utc_time_sec/D"       );
    t_ppd_tree->Branch("utc_time_str",         t_ppd.utc_time_str,         "utc_time_str/C"       );
    t_ppd_tree->Branch("flag_of_pos",         &t_ppd.flag_of_pos,          "flag_of_pos/I"        );
    t_ppd_tree->Branch("wgs84_x",             &t_ppd.wgs84_x,              "wgs84_x/D"            );
    t_ppd_tree->Branch("wgs84_y",             &t_ppd.wgs84_y,              "wgs84_y/D"            );
    t_ppd_tree->Branch("wgs84_z",             &t_ppd.wgs84_z,              "wgs84_z/D"            );
    t_ppd_tree->Branch("wgs84_x_v",           &t_ppd.wgs84_x_v,            "wgs84_x_v/D"          );
    t_ppd_tree->Branch("wgs84_y_v",           &t_ppd.wgs84_y_v,            "wgs84_y_v/D"          );
    t_ppd_tree->Branch("wgs84_z_v",           &t_ppd.wgs84_z_v,            "wgs84_z_v/D"          );
    t_ppd_tree->Branch("det_z_lat",           &t_ppd.det_z_lat,            "det_z_lat/D"          );
    t_ppd_tree->Branch("det_z_lon",           &t_ppd.det_z_lon,            "det_z_lon/D"          );
    t_ppd_tree->Branch("det_z_ra",            &t_ppd.det_z_ra,             "det_z_ra/D"           );
    t_ppd_tree->Branch("det_z_dec",           &t_ppd.det_z_dec,            "det_z_dec/D"          );
    t_ppd_tree->Branch("det_x_lat",           &t_ppd.det_x_lat,            "det_x_lat/D"          );
    t_ppd_tree->Branch("det_x_lon",           &t_ppd.det_x_lon,            "det_x_lon/D"          );
    t_ppd_tree->Branch("det_x_ra",            &t_ppd.det_x_ra,             "det_x_ra/D"           );
    t_ppd_tree->Branch("det_x_dec",           &t_ppd.det_x_dec,            "det_x_dec/D"          );
    t_ppd_tree->Branch("earth_lat",           &t_ppd.earth_lat,            "earth_lat/D"          );
    t_ppd_tree->Branch("earth_lon",           &t_ppd.earth_lon,            "earth_lon/D"          );
    t_ppd_tree->Branch("earth_ra",            &t_ppd.earth_ra,             "earth_ra/D"           );
    t_ppd_tree->Branch("earth_dec",           &t_ppd.earth_dec,            "earth_dec/D"          );
    t_ppd_tree->Branch("sun_ra",              &t_ppd.sun_ra,               "sun_ra/D"             );
    t_ppd_tree->Branch("sun_dec",             &t_ppd.sun_dec,              "sun_dec/D"            );

}
