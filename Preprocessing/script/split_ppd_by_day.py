#!/usr/bin/env python

import argparse
import os, re, time
from datetime import datetime, timedelta
import subprocess

delimeter = " " + "-" * 80
leapsecond = 18

parser = argparse.ArgumentParser(description='list time not matched of data')
parser.add_argument("dir", help = 'dir to store splitted data')
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

ppd_1m_dir = os.path.join(path_prefix, 'data_in_orbit_product', args.type, 'internal', 'decoded', 'PPD_1M')
if not os.path.isdir(ppd_1m_dir):
    print '"' + ppd_1m_dir + '" does not exist.'
    exit(1)

output_dir = args.dir
if not os.path.isdir(output_dir):
    print '"' + output_dir + '" does not exist.'
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
print ' - ppd_1m_dir:  ' + ppd_1m_dir
print ' - output_dir:  ' + output_dir
print delimeter

ref_filename = None

if args.type in ['normal']:
    ref_filename = re.compile(r'T2_POL_.*_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_1[M|P].root')
else:
    ref_filename = re.compile(r'TS_TG02_POL_.*_\d+_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_\d+_1[M|P].root')

all_file_list_ppd_1m = [x for x in os.listdir(ppd_1m_dir) if ref_filename.match(x)]

def calc_time(filename):
    m = ref_filename.match(filename)
    return (datetime.strptime(m.group(1), timeformat), datetime.strptime(m.group(2), timeformat))

start_time = datetime.strptime(args.timefrom, timeformat)
stop_time  = datetime.strptime(args.timeto, timeformat)
file_list_ppd_1m = []
for x in all_file_list_ppd_1m:
    begin_time, end_time = calc_time(x)
    if end_time >= start_time  and begin_time <= stop_time:
        file_list_ppd_1m.append(x)

if len(file_list_ppd_1m) < 2:
    print 'too small number of files.'
    exit(0)

file_list_ppd_1m.sort()
file_list_ppd_1m_alone = [file_list_ppd_1m[0]]
pre_time = calc_time(file_list_ppd_1m_alone[0])
print file_list_ppd_1m_alone[0]
for i in xrange(1, len(file_list_ppd_1m)):
    cur_file = file_list_ppd_1m[i]
    cur_time = calc_time(cur_file)
    if ((cur_time[0] - pre_time[1]).total_seconds() > -5):
        file_list_ppd_1m_alone.append(cur_file)
        pre_time = cur_time

UTCStartTime = 1157984027.3750000000
start_time = UTCStartTime + 596000.0
utc_start = datetime(1980, 1, 6, 8, 0, 0)
ppd_1m_start_index = 0
day_num = -1
day_data_dict = {}
while True:
    day_num += 1
    cur_start_time = start_time + day_num * 86400.0
    cur_stop_time = start_time + (day_num + 1) * 86400.0
    cur_time = calc_time(file_list_ppd_1m_alone[ppd_1m_start_index])
    cur_time_second = ((cur_time[0] - utc_start).total_seconds(), (cur_time[1] - utc_start).total_seconds())
    while cur_time_second[1] < cur_start_time:
        ppd_1m_start_index += 1
        if ppd_1m_start_index >= len(file_list_ppd_1m_alone): break
        cur_time = calc_time(file_list_ppd_1m_alone[ppd_1m_start_index])
        cur_time_second = ((cur_time[0] - utc_start).total_seconds(), (cur_time[1] - utc_start).total_seconds())
    if ppd_1m_start_index >= len(file_list_ppd_1m_alone): break
    index_shift = -1
    file_list = []
    while True:
        index_shift += 1
        cur_index = ppd_1m_start_index + index_shift
        if cur_index >= len(file_list_ppd_1m_alone): break
        cur_file = file_list_ppd_1m_alone[cur_index]
        cur_time = calc_time(cur_file)
        cur_time_second = ((cur_time[0] - utc_start).total_seconds(), (cur_time[1] - utc_start).total_seconds())
        if cur_time_second[0] < cur_stop_time:
            file_list.append(cur_file)
        else:
            break
    if len(file_list) > 0:
        day_data_dict[day_num] = file_list


for day_num in sorted(day_data_dict.keys()):
    print " - day: " + str(day_num)
    for x in day_data_dict[day_num]:
        print " -> " + x
    cur_start_time = start_time + day_num * 86400.0 - 20.0
    cur_start_time_week = int(cur_start_time / 604800)
    cur_start_time_second = int(cur_start_time % 604800)
    cur_stop_time = start_time + (day_num + 1) * 86400.0 + 20.0
    cur_stop_time_week = int(cur_stop_time / 604800)
    cur_stop_time_second = int(cur_stop_time % 604800)
    first_file = day_data_dict[day_num][0]
    first_file_time = calc_time(first_file)
    first_file_time_second = ((first_file_time[0] - utc_start).total_seconds(), (first_file_time[1] - utc_start).total_seconds())
    last_file = day_data_dict[day_num][-1]
    last_file_time = calc_time(last_file)
    last_file_time_second = ((last_file_time[0] - utc_start).total_seconds(), (last_file_time[1] - utc_start).total_seconds())
    begin_time = None
    end_time = None
    if cur_start_time < first_file_time_second[0] + 5.0:
        begin_time = 'begin'
    else:
        begin_time = '%d:%d' % (cur_start_time_week, cur_start_time_second)
    if cur_stop_time > last_file_time_second[1] - 5.0:
        end_time = 'end'
    else:
        end_time = '%d:%d' % (cur_stop_time_week, cur_stop_time_second)
    print begin_time + ' => ' + end_time
    cur_ppd_file_list = [os.path.join(ppd_1m_dir, x) for x in day_data_dict[day_num]]
    outputfile = 'ppd_day_%02d.root' % day_num
    command = 'PPD_Split.py ' + ' '.join(cur_ppd_file_list) + ' -B ' + begin_time + ' -E ' + end_time + ' -o ' + os.path.join(output_dir, outputfile)
#    print command
    subprocess.call(command, shell = True)





