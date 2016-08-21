#!/usr/bin/env python

import re
from time import strptime, strftime, mktime
from datetime import datetime
import numpy as np

def gen_utc_time_str(utc_ym, utc_dtime):
    # utc_ym    => 0-0
    # utc_dtime => 0000D00:00:00
    ym   = [int(x) for x in utc_ym.split('-')]
    dhms = [int(x) for x in re.split('[D:]', utc_dtime)]
    return "%04d-%02d-%02dT%02d:%02d:%02d+0000" % (ym[0], ym[1], dhms[0], dhms[1], dhms[2], dhms[3])

def calc_utc_time_sec(utc_time_str):
    td_vals = [int(x) for x in re.split('[-T:\+]', utc_time_str)]
    if td_vals[0] < 1 or td_vals[1] < 1 or td_vals[2] < 1: return -1
    t_fmt_str = "%Y-%m-%dT%H:%M:%S+0000"
    utc_gps_zero = "1980-01-06T00:00:00+0000"
    return mktime(strptime(utc_time_str, t_fmt_str)) - mktime(strptime(utc_gps_zero, t_fmt_str))

def calc_ship_time_sec(ship_time_str):
    # 0000D01:19:04.000.0
    timevar = [int(x) for x in re.split('[D:\.]', ship_time_str)]
    return timevar[0] * 24 * 3600 + timevar[1] * 3600 + timevar[2] * 60 + timevar[3] + timevar[4] * 1E-3 + timevar[5] * 1E-4

def xyz_to_lati_longi(x, y, z): 
    r = np.sqrt(x**2 + y**2 + z**2)
    return [90 - np.arccos(z / r) / np.pi * 180, np.arctan2(y, x) / np.pi * 180]

def lati_longi_to_xyz(lati, longi):
    theta = (90 - lati) / 180 * np.pi
    pha = (longi if longi >= 0 else longi + 360) / 180 * np.pi
    return [np.sin(theta) * np.cos(pha), np.sin(theta) * np.sin(pha), np.cos(theta)]

