#!/usr/bin/env python

import argparse
import os, re, time
from datetime import datetime, timedelta
from split_time import find_maintenance
from time_conv import gps_to_beijing

delimeter = " " + "-" * 80

parser = argparse.ArgumentParser(description='find time of maintenance mode from AUX_1M data')
parser.add_argument("-r", dest = "pathprefix", default = "/hxmt/data/Mission/POLAR/data_in_orbit_test")
parser.add_argument("--from", dest = "timefrom", default = '20100901000000')
parser.add_argument("--to", dest = "timeto", default = '20300901000000')
parser.add_argument("--type", dest = "type", default = 'normal')
args = parser.parse_args()

if args.type not in ['normal', 'manual']:
    print 'illegal type'
    exit(1)

path_prefix = os.path.abspath(args.pathprefix)
if not os.path.isdir(path_prefix):
    print '"' + path_prefix + '" does not exist.'
    exit(1)

data_dir = os.path.join(path_prefix, 'data_in_orbit_product', args.type, 'internal', 'decoded', 'AUX_1M')
if not os.path.isdir(data_dir):
    print '"' + data_dir + '" does not exist.'
    exit(1)

ref_timestr = re.compile(r'\d\d\d\d\d\d\d\d\d\d\d\d\d\d')
timeformat = '%Y%m%d%H%M%S'

if not ref_timestr.match(args.timefrom):
    print 'bad time string of timefrom'
    exit(1)
if not ref_timestr.match(args.timeto):
    print 'bad time string of timeto'
    exit(1)

print ' - path_prefix: ' + path_prefix
print ' - data_dir:    ' + data_dir
print delimeter

ref_filename = None

if args.type in ['normal']:
    ref_filename = re.compile(r'T2_POL_.*_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_1[M|P].root')
else:
    ref_filename = re.compile(r'TS_TG02_POL_.*_\d+_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_\d+_1[M|P].root')

all_file_list = [x for x in os.listdir(data_dir) if ref_filename.match(x)]

def calc_time(filename):
    m = ref_filename.match(filename)
    return (datetime.strptime(m.group(1), timeformat), datetime.strptime(m.group(2), timeformat))

start_time = datetime.strptime(args.timefrom, timeformat)
stop_time  = datetime.strptime(args.timeto, timeformat)
file_list = []
for x in all_file_list:
    begin_time, end_time = calc_time(x)
    if end_time >= start_time and begin_time <= stop_time:
        file_list.append(x)

if len(file_list) < 1:
    print 'too small number of files.'
    exit(0)

file_list.sort()
file_list_no_overlap = []
pre_time = None
for i, x in enumerate(file_list):
    cur_time = calc_time(x)
    if i == 0:
        pre_time = cur_time
        continue
    if pre_time[1] > cur_time[1]:
        continue
    else:
        file_list_no_overlap.append(x)
        pre_time = cur_time

print "list of maintenance: "
for x in file_list_no_overlap:
    cur_maintenance = find_maintenance(os.path.join(data_dir, x))
    if len(cur_maintenance) < 1: continue
    cur_beijing_time = []
    for y in cur_maintenance:
        cur_beijing_time.append(gps_to_beijing(y[1]))
    print ' - ' + x
    print ' > ' + str(cur_maintenance) + ' <=> ' + str(cur_beijing_time)

