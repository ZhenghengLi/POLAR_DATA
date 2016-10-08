#!/usr/bin/env python

SW_NAME      = 'PPD_Split.py'
SW_VERSION   = 'v1.0.0'
RELEASE_DATE = '2016 Sep  5'

import argparse
from os.path import basename
from ppd_file_r import ppd_file_r
from tqdm import tqdm
import numpy as np
import ephem as ep
from datetime import datetime

UTCStartTime = 1157984027.3750000000

time_fmt_in  = '%Y-%m-%dT%H:%M:%S+0000'
time_fmt_out = '%Y-%m-%d %H:%M:%S'

def _xyz_to_latlon(x, y, z):
    r = np.sqrt(x**2 + y**2 + z**2)
    return [90 - np.arccos(z / r) / np.pi * 180, np.arctan2(y, x) / np.pi * 180]

def _wgs84_to_j2000(wgs84_xyz, utc):
    lat, lon = _xyz_to_latlon(*wgs84_xyz)
    location = ep.Observer()
    location.lat  = lat / 180 * np.pi
    location.lon  = lon / 180 * np.pi
    location.date = utc
    ra_now  = location.sidereal_time()
    dec_now = lat / 180 * np.pi
    pos_now   = ep.Equatorial(ra_now, dec_now, epoch = utc)
    pos_j2000 = ep.Equatorial(pos_now, epoch = ep.J2000)
    return [float(pos_j2000.ra) / np.pi * 12, float(pos_j2000.dec) / np.pi * 180]

def _radec_to_j2000xyz(ra, dec, d):
    theta = ra / 12 * np.pi
    pha = (90 - dec) / 180 * np.pi
    return (d * np.sin(theta) * np.cos(pha), d * np.sin(theta) * np.sin(pha), d * np.cos(theta)) 

parser = argparse.ArgumentParser(description='print location and time')
parser.add_argument('filename', help = 'decoded file to open')
parser.add_argument('-B', dest = 'begin', help = 'utc time of beginning as gps time form, like utc_week:utc_second', default = 'begin')
parser.add_argument('-E', dest = 'end'  , help = 'utc time of ending as gps time form, like utc_week:utc_second', default = 'end')
parser.add_argument('-o', dest = 'outfile', help = 'text file to store location and time', default = 'loc_output.csv')
args = parser.parse_args()

ppd_file_r_obj = ppd_file_r()
if not ppd_file_r_obj.open_file(args.filename, args.begin, args.end):
    print 'Error: root file open failed: ' + args.filename
    exit(1)

ppd_file_r_obj.print_file_info()

fout = open(args.outfile, 'w')

for i in tqdm(xrange(ppd_file_r_obj.begin_entry, ppd_file_r_obj.end_entry)):
    ppd_file_r_obj.t_tree_ppd.get_entry(i)
    cur_time = ppd_file_r_obj.t_tree_ppd.utc_time_sec - UTCStartTime
    wgs84_xyz = (ppd_file_r_obj.t_tree_ppd.wgs84_x, ppd_file_r_obj.t_tree_ppd.wgs84_y, ppd_file_r_obj.t_tree_ppd.wgs84_z)
    utc = datetime.strptime(str(ppd_file_r_obj.t_tree_ppd.utc_time_str), time_fmt_in).strftime(time_fmt_out)
    ra, dec = _wgs84_to_j2000(wgs84_xyz, utc)
    j2000_xyz = _radec_to_j2000xyz(ra, dec, ppd_file_r_obj.t_tree_ppd.geocentric_d)
    fout.write(str(cur_time) + ' ' + str(j2000_xyz[0]) + ' ' + str(j2000_xyz[1]) + ' ' + str(j2000_xyz[2]) + ' ' + str(ppd_file_r_obj.t_tree_ppd.det_z_ra) + ' ' + str(ppd_file_r_obj.t_tree_ppd.det_z_dec) + '\n')

fout.close()

