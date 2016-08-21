#!/usr/bin/env python

import argparse
import csv
from rootpy import ROOT
from rootpy.io import File
from rootpy.tree import Tree
from conv_fun import *

parser = argparse.ArgumentParser(description='Convert platform parameters data from CSV file to ROOT file')
parser.add_argument("filename", help = "CSV file of platform parameters data")
parser.add_argument("-o", dest = "outfile", help = "ROOT file to store platform parameters data", default = "TG2_PPD_file.root")
args = parser.parse_args()

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

first_ship_time_sec  = 0
last_ship_time_sec   = 0
ship_time_is_first   = True

first_utc_time_sec   = 0
last_utc_time_sec    = 0
utc_time_is_first    = True

with open(args.filename, 'rb') as csvfile:
    reader = csv.reader(csvfile)
    for i, row in enumerate(reader):
        if i < 2: continue
        t_tree_ppd.pitch_angle        = float(row[1])
        t_tree_ppd.yaw_angle          = float(row[2])
        t_tree_ppd.roll_angle         = float(row[3])
        t_tree_ppd.pitch_angle_v      = float(row[4])
        t_tree_ppd.yaw_angle_v        = float(row[5])
        t_tree_ppd.roll_angle_v       = float(row[6])
        t_tree_ppd.orbit_agl_v        = float(row[7])
        t_tree_ppd.longitude          = float(row[8])
        t_tree_ppd.latitude           = float(row[9])
        t_tree_ppd.geocentric_d       = float(row[10])
        cur_ship_time_sec             = calc_ship_time_sec(row[11])
        t_tree_ppd.ship_time_sec      = cur_ship_time_sec
        cur_utc_time_str              = gen_utc_time_str(row[12], row[13])
        cur_utc_time_sec              = calc_utc_time_sec(cur_utc_time_str)
        t_tree_ppd.utc_time_str       = cur_utc_time_str
        t_tree_ppd.utc_time_sec       = cur_utc_time_sec
        cur_flag_of_pos               = int(row[14], 16)
        t_tree_ppd.flag_of_pos        = cur_flag_of_pos
        t_tree_ppd.wgs84_x            = float(row[15])
        t_tree_ppd.wgs84_y            = float(row[16])
        t_tree_ppd.wgs84_z            = float(row[17])
        t_tree_ppd.wgs84_x_v          = float(row[18])
        t_tree_ppd.wgs84_y_v          = float(row[19])
        t_tree_ppd.wgs84_z_v          = float(row[20])
        t_tree_ppd.fill()
        if cur_flag_of_pos != 0x55: continue
        if ship_time_is_first:
            ship_time_is_first  = False
            first_ship_time_sec = cur_ship_time_sec
        last_ship_time_sec = cur_ship_time_sec
        if utc_time_is_first:
            utc_time_is_first  = False
            first_utc_time_sec = cur_utc_time_sec
        last_utc_time_sec = cur_utc_time_sec

dattype        = ROOT.TNamed("dattype", "PLATFORM PARAMETERS DATA")
version        = ROOT.TNamed("version", "PPD_Gen1M.py v1.0.0")
gentime        = ROOT.TNamed("gentime", datetime.now().isoformat() + "+0800")
ship_time_span = ROOT.TNamed("ship_time_span", str(first_ship_time_sec) + " => " + str(last_ship_time_sec))
utc_time_span  = ROOT.TNamed("utc_time_span",  str(first_utc_time_sec) + " => " + str(last_utc_time_sec))

t_file_out.cd()
t_tree_ppd.write()
dattype.Write()
version.Write()
gentime.Write()
ship_time_span.Write()
utc_time_span.Write()
t_file_out.close()

