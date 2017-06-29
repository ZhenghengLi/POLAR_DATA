#!/usr/bin/env python

import sys
from rootpy import ROOT
from rootpy.io import File
from rootpy.tree import Tree
from collections import deque
from time_conv import gps_to_beijing

if len(sys.argv) < 2:
    print "USAGE: " + sys.argv[0] + " <ppd_file.root>"
    exit(2)

filename = sys.argv[1]

print "list of maintenance (Beijing Time): "
aux_file = File(filename, 'read')
aux_tree = aux_file.get('t_hk_obox')
maintenance_start = False
gps_time_list = []
for entry in aux_tree:
    if entry.obox_is_bad > 0: continue
    if entry.obox_mode.encode('hex') == '04':
        if not maintenance_start:
            maintenance_start = True
        gps_time_list.append(entry.abs_gps_week * 604800 + entry.abs_gps_second)
    else:
        if maintenance_start:
            maintenance_start = False
            maintenance_gps = (gps_time_list[0] + gps_time_list[-1]) / 2
            week_second = "%d:%d" % (maintenance_gps / 604800, maintenance_gps % 604800)
            print gps_to_beijing(week_second)
            gps_time_list = []

