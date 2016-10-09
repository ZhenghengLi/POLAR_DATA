#!/usr/bin/env python

import argparse
import os, re, time
from datetime import datetime, timedelta
from rootpy import ROOT
from rootpy.io import File

delimeter = " " + "-" * 80
leapsecond = 18

tnamed_dict = {'SCI_1P':'m_ped_gps', 'SCI_1M':'m_ped_gps_frm', 'AUX_1M':'m_ibox_gps', 'PPD_1M':'m_utc_span'}
leap_seconds_dict = {'SCI_1P':leapsecond, 'SCI_1M':leapsecond, 'AUX_1M':leapsecond, 'PPD_1M':-3}
ref_time = re.compile(r'(\d+):(\d+)\[\d+\] => (\d+):(\d+)\[\d+\].*')

def read_timespan(filename, dat_type):
    t_file = File(filename, 'read')
    m = ref_time.match(t_file.get(tnamed_dict[dat_type]).GetTitle())
    week1, second1, week2, second2 = int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(4))
    t_file.close()
    time_seconds_begin = week1 * 604800 + second1
    time_seconds_end   = week2 * 604800 + second2
    beijing_time_begin     = datetime(1980, 1, 6, 0, 0, 0) + timedelta(seconds = time_seconds_begin - leap_seconds_dict[dat_type] + 28800);
    beijing_time_end       = datetime(1980, 1, 6, 0, 0, 0) + timedelta(seconds = time_seconds_end   - leap_seconds_dict[dat_type] + 28800);
    return (beijing_time_begin, beijing_time_end) 

parser = argparse.ArgumentParser(description='list time not matched of data')
parser.add_argument("dir", help = 'must be SCI_1M, AUX_1M or PPD_1M')
parser.add_argument("-r", dest = "pathprefix", default = "/hxmt/data/Mission/POLAR/data_in_orbit_test")
parser.add_argument("--from", dest = "timefrom", default = '20100901000000')
parser.add_argument("--to", dest = "timeto", default = '20300901000000')
parser.add_argument("--type", dest = "type", default = 'normal')
args = parser.parse_args()

if args.dir not in ['SCI_1P', 'SCI_1M', 'AUX_1M', 'PPD_1M']:
    print 'illegal dir'
    exit(1)

if args.type not in ['normal', 'manual']:
    print 'illegal type'
    exit(1)

path_prefix = os.path.abspath(args.pathprefix)
if not os.path.isdir(path_prefix):
    print '"' + path_prefix + '" does not exist.'
    exit(1)

data_dir = os.path.join(path_prefix, 'data_in_orbit_product', args.type, 'internal', 'decoded', args.dir)
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

if len(file_list) < 2:
    print 'too small number of files.'
    exit(0)

file_list.sort()
print " - time bad matched list: "
for i, x in enumerate(file_list):
    cur_file = os.path.join(data_dir, x)
    filename_time = calc_time(x)
    internal_time = read_timespan(cur_file, args.dir)
    begin_diff_sec = (internal_time[0] - filename_time[0]).total_seconds()
    end_diff_sec   = (filename_time[1] - internal_time[1]).total_seconds()
    if abs(begin_diff_sec) > 5 or abs(end_diff_sec) > 5:
        print re.sub('_1[M|P].root', '_0B.dat', x)
        print ' > ' + filename_time[0].strftime(timeformat) + '_' + filename_time[1].strftime(timeformat) + ' <=> ' \
                + internal_time[0].strftime(timeformat) + '_' + internal_time[1].strftime(timeformat) \
                + ' { ' + str(begin_diff_sec) + ' seconds, ' + str(end_diff_sec) + ' seconds } '




