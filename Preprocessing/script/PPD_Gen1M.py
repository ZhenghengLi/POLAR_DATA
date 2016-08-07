#!/usr/bin/env python

import argparse
import re
from time import strptime, strftime, mktime
import csv
from rootpy.io import File
from rootpy.tree import Tree

parser = argparse.ArgumentParser(description='Convert platform parameters data from CSV file to ROOT file')
parser.add_argument("filename", help = "CSV file of platform parameters data")
parser.add_argument("-o", dest = "outfile", help = "ROOT file to store platform parameters data", default = "TG2_PPD_file.root")
args = parser.parse_args()

print args.filename
print args.outfile

# ============

def gen_utc_time_str(utc_ym, utc_dtime):
    # utc_ym    => 0-0
    # utc_dtime => 0000D00:00:00
    ym   = [int(x) for x in utc_ym.split('-')]
    dhms = [int(x) for x in re.split('[D:]', utc_dtime)]
    return "%04d-%02d-%02dT%02d:%02d:%02d+0000" % (ym[0], ym[1], dhms[0], dhms[1], dhms[2], dhms[3])

def calc_utc_time_sec(utc_time_str):
    t_fmt_str = "%Y-%m-%dT%H:%M:%S+0000"
    utc_gps_zero = "1980-01-06T00:00:00+0000"
    return mktime(strptime(utc_time_str, t_fmt_str)) - mktime(strptime(utc_gps_zero, t_fmt_str))

def calc_ship_time_sec(ship_time_str):
    # 0000D01:19:04.000.0
    timevar = [int(x) for x in re.split('[D:\.]', ship_time_str)]
    return timevar[0] * 24 * 3600 + timevar[1] * 3600 + timevar[2] * 60 + timevar[3] + timevar[4] * 1E-3 + timevar[5] * 1E-4

# ===========

t_file_out = File(args.outfile, "recreate")
t_tree_ppd = Tree("t_ppd", "platform parameters data")
t_tree_ppd.create_branches({
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
    })



t_file_out.cd()
t_tree_ppd.write()
t_file_out.close()
