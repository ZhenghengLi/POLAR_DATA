#!/usr/bin/env python

from rootpy import ROOT
from rootpy.io import File
from rootpy.tree import Tree
from collections import deque

def find_maitenance(filename):
    print "test"


def find_orbitstart(filename):
    LAT_LEN = 500
    lat_deque = deque()
    orbitstart_list = []
    ppd_file = File(filename, 'read')
    ppd_tree = ppd_file.get('t_ppd')
    ready_flag = True
    pre_diff = 0.0
    cur_diff = 0.0
    for entry in ppd_tree:
        if entry.flag_of_pos != 0x55: continue
        lat_deque.append((entry.latitude, entry.ship_time_sec, entry.utc_time_sec))
        if len(lat_deque) < LAT_LEN: 
            pre_diff = lat_deque[-1][0] - lat_deque[0][0]
            continue
        else:
            lat_deque.popleft()
            cur_diff = lat_deque[-1][0] - lat_deque[0][0]
        if ready_flag and pre_diff < 0 and cur_diff >= 0:
            orbitstart_list.append(((lat_deque[-1][1] + lat_deque[0][1]) / 2, (lat_deque[-1][2] + lat_deque[0][2]) / 2))
            ready_flag = False
        if not ready_flag and pre_diff > 0 and cur_diff <= 0:
            ready_flag = True
        pre_diff = cur_diff
    return [(int(x[0]), "%d:%d" % (int(x[1] / 604800), int(x[1] % 604800))) for x in orbitstart_list]
