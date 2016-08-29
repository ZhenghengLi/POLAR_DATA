#!/usr/bin/env python

from rootpy import ROOT
from rootpy.io import File
from rootpy.tree import Tree

class ppd_file:
    def __int__(self):
        self.__t_file_out = None
        self.__t_tree_ppd = None

    def open(self, filename):
        self.__t_file_out = File(filename, "recreate")
        self.__t_tree_ppd = Tree("t_ppd", "platform parameters data")
        self.__t_tree_ppd.create_branches({
            "pitch_angle"          : "D"     ,
            "yaw_angle"            : "D"     ,
            "roll_angle"           : "D"     ,
            "pitch_angle_v"        : "D"     ,
            "yaw_angle_v"          : "D"     ,
            "roll_angle_v"         : "D"     ,
            "orbit_agl_v"          : "D"     ,
            "longitude"            : "D"     ,
            "latitude"             : "D"     ,
            "geocentric_d"         : "D"     ,
            "ship_time_sec"        : "D"     ,
            "utc_time_sec"         : "D"     ,
            "utc_time_str"         : "C[32]" ,
            "flag_of_pos"          : "I"     ,
            "wgs84_x"              : "D"     ,
            "wgs84_y"              : "D"     ,
            "wgs84_z"              : "D"     ,
            "wgs84_x_v"            : "D"     ,
            "wgs84_y_v"            : "D"     ,
            "wgs84_z_v"            : "D"     ,
            "det_z_lat"            : "D"     ,
            "det_z_lon"            : "D"     ,
            "det_z_ra"             : "D"     ,
            "det_z_dec"            : "D"     ,
            "det_x_lat"            : "D"     ,
            "det_x_lon"            : "D"     ,
            "det_x_ra"             : "D"     ,
            "det_x_dec"            : "D"     ,
            "earth_lat"            : "D"     ,
            "earth_lon"            : "D"     ,
            "earth_ra"             : "D"     ,
            "earth_dec"            : "D"     
            })

    def fill_data(self, ppd_obj):
        self.__t_tree_ppd.pitch_angle     = ppd_obj.pitch_angle
        self.__t_tree_ppd.yaw_angle       = ppd_obj.yaw_angle
        self.__t_tree_ppd.roll_angle      = ppd_obj.roll_angle
        self.__t_tree_ppd.pitch_angle_v   = ppd_obj.pitch_angle_v
        self.__t_tree_ppd.yaw_angle_v     = ppd_obj.yaw_angle_v
        self.__t_tree_ppd.roll_angle_v    = ppd_obj.roll_angle_v
        self.__t_tree_ppd.orbit_agl_v     = ppd_obj.orbit_agl_v
        self.__t_tree_ppd.longitude       = ppd_obj.longitude
        self.__t_tree_ppd.latitude        = ppd_obj.latitude
        self.__t_tree_ppd.geocentric_d    = ppd_obj.geocentric_d
        self.__t_tree_ppd.ship_time_sec   = ppd_obj.ship_time_sec
        self.__t_tree_ppd.utc_time_sec    = ppd_obj.utc_time_sec
        self.__t_tree_ppd.utc_time_str    = ppd_obj.utc_time_str
        self.__t_tree_ppd.flag_of_pos     = ppd_obj.flag_of_pos
        self.__t_tree_ppd.wgs84_x         = ppd_obj.wgs84_x
        self.__t_tree_ppd.wgs84_y         = ppd_obj.wgs84_y
        self.__t_tree_ppd.wgs84_z         = ppd_obj.wgs84_z
        self.__t_tree_ppd.wgs84_x_v       = ppd_obj.wgs84_x_v
        self.__t_tree_ppd.wgs84_y_v       = ppd_obj.wgs84_y_v
        self.__t_tree_ppd.wgs84_z_v       = ppd_obj.wgs84_z_v
        self.__t_tree_ppd.det_z_lat       = ppd_obj.det_z_lat
        self.__t_tree_ppd.det_z_lon       = ppd_obj.det_z_lon
        self.__t_tree_ppd.det_z_ra        = ppd_obj.det_z_ra
        self.__t_tree_ppd.det_z_dec       = ppd_obj.det_z_dec
        self.__t_tree_ppd.det_x_lat       = ppd_obj.det_x_lat
        self.__t_tree_ppd.det_x_lon       = ppd_obj.det_x_lon
        self.__t_tree_ppd.det_x_ra        = ppd_obj.det_x_ra
        self.__t_tree_ppd.det_x_dec       = ppd_obj.det_x_dec
        self.__t_tree_ppd.earth_lat       = ppd_obj.earth_lat
        self.__t_tree_ppd.earth_lon       = ppd_obj.earth_lon
        self.__t_tree_ppd.earth_ra        = ppd_obj.earth_ra
        self.__t_tree_ppd.earth_dec       = ppd_obj.earth_dec
        self.__t_tree_ppd.fill()

    def write_tree(self):
        self.__t_file_out.cd()
        self.__t_tree_ppd.write()

    def write_meta(self, key, value):
        self.__t_file_out.cd()
        ROOT.TNamed(key, value).Write()

    def close(self):
        self.__t_file_out.close()
        self.__t_file_out = None
        self.__t_tree_ppd = None

