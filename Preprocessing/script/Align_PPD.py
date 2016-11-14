#!/usr/bin/env python

import argparse
import os, re, time
from datetime import datetime, timedelta
import subprocess

delimeter = " " + "-" * 80
leapsecond = 17

parser = argparse.ArgumentParser(description='list time not matched of data')
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

sci_1p_dir = os.path.join(path_prefix, 'data_in_orbit_product', args.type, 'internal', 'decoded', 'SCI_1P')
if not os.path.isdir(sci_1p_dir):
    print '"' + sci_1p_dir + '" does not exist.'
    exit(1)

ppd_1m_dir = os.path.join(path_prefix, 'data_in_orbit_product', args.type, 'internal', 'decoded', 'PPD_1M')
if not os.path.isdir(ppd_1m_dir):
    print '"' + ppd_1m_dir + '" does not exist.'
    exit(1)

ppd_1n_dir = os.path.join(path_prefix, 'data_in_orbit_product', args.type, 'internal', 'decoded', 'PPD_1N')
if not os.path.isdir(ppd_1n_dir):
    print '"' + ppd_1n_dir + '" does not exist.'
    exit(1)

scrfile_dir_ppd_1n = os.path.join(path_prefix, 'data_in_orbit_product', args.type, 'screen', 'decoded', 'PPD_1N')
if not os.path.isdir(scrfile_dir_ppd_1n):
    print '"' + scrfile_dir_ppd_1n + '" does not exist.'
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
print ' - sci_1p_dir:  ' + sci_1p_dir
print ' - ppd_1m_dir:  ' + ppd_1m_dir
print ' - ppd_1n_dir:  ' + ppd_1n_dir
print ' - scrfile_dir: ' + scrfile_dir_ppd_1n
print delimeter

ref_filename = None

if args.type in ['normal']:
    ref_filename = re.compile(r'T2_POL_.*_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_1[M|P].root')
else:
    ref_filename = re.compile(r'TS_TG02_POL_.*_\d+_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_\d+_1[M|P].root')

all_file_list_sci_1p = [x for x in os.listdir(sci_1p_dir) if ref_filename.match(x)]
all_file_list_ppd_1m = [x for x in os.listdir(ppd_1m_dir) if ref_filename.match(x)]

def calc_time(filename):
    m = ref_filename.match(filename)
    return (datetime.strptime(m.group(1), timeformat), datetime.strptime(m.group(2), timeformat))

start_time = datetime.strptime(args.timefrom, timeformat)
stop_time  = datetime.strptime(args.timeto, timeformat)
file_list_sci_1p = []
file_list_ppd_1m = []
for x in all_file_list_sci_1p:
    ppd_1n_filename = x.replace('_SCI_', '_PPD_').replace('_1P', '_1N')
    if os.path.isfile(os.path.join(ppd_1n_dir, ppd_1n_filename)): continue
    begin_time, end_time = calc_time(x)
    if begin_time >= start_time and end_time <= stop_time:
        file_list_sci_1p.append(x)
for x in all_file_list_ppd_1m:
    begin_time, end_time = calc_time(x)
    if end_time >= start_time  and begin_time <= stop_time:
        file_list_ppd_1m.append(x)

if len(file_list_sci_1p) < 2 or len(file_list_ppd_1m) < 2:
    print 'too small number of files.'
    exit(0)

file_list_sci_1p.sort()
file_list_ppd_1m.sort()

# find ppd_1m for sci_1p
sci_1p_ppd_1m_dict = {}
ppd_1m_start_index = 0
for x in file_list_sci_1p:
    sci_1p_begin_time, sci_1p_end_time = calc_time(x)
    ppd_1m_begin_time, ppd_1m_end_time = calc_time(file_list_ppd_1m[ppd_1m_start_index])
    while ppd_1m_end_time < sci_1p_begin_time:
        ppd_1m_start_index += 1
        ppd_1m_begin_time, ppd_1m_end_time = calc_time(file_list_ppd_1m[ppd_1m_start_index])
    if ppd_1m_begin_time > sci_1p_begin_time: continue
    index_shift = 0
    ppd_1m_list = [file_list_ppd_1m[ppd_1m_start_index]]
    found_gap = False
    found_overlap = False
    pre_end_time = ppd_1m_end_time
    while ppd_1m_end_time < sci_1p_end_time:
        index_shift += 1
        ppd_1m_begin_time, ppd_1m_end_time = calc_time(file_list_ppd_1m[ppd_1m_start_index + index_shift])
        if (ppd_1m_begin_time - pre_end_time).total_seconds() > 7:
            found_gap = True
            break
        if (ppd_1m_begin_time - pre_end_time).total_seconds() < 0:
            found_overlap = True
            break
        ppd_1m_list.append(file_list_ppd_1m[ppd_1m_start_index + index_shift])
    if found_gap or found_overlap: continue
    sci_1p_ppd_1m_dict[x] = ppd_1m_list

for k in sorted(sci_1p_ppd_1m_dict.keys()):
    print k
    for p in sci_1p_ppd_1m_dict[k]:
        print ' - ' + p
    sci_1p_begin_time, sci_1p_end_time = calc_time(k)
    start_total_seconds = (sci_1p_begin_time - datetime(1980, 1, 6, 8, 0, 0)).total_seconds() - 20
    stop_total_seconds  = (sci_1p_end_time   - datetime(1980, 1, 6, 8, 0, 0)).total_seconds() + 20
    start_week   = int(start_total_seconds / 604800)
    start_second = int(start_total_seconds % 604800)
    stop_week    = int(stop_total_seconds  / 604800)
    stop_second  = int(stop_total_seconds  % 604800)
    start_time = "%d:%d" % (start_week, start_second)
    stop_time  = "%d:%d" % (stop_week,  stop_second)
    print start_time + ' => ' + stop_time
    ppd_1n_filename = x.replace('_SCI_', '_PPD_').replace('_1P', '_1N')
    print ppd_1n_filename
    ppd_1n_file = os.path.isfile(os.path.join(ppd_1n_dir, ppd_1n_filename))
    ppd_1n_outfile = os.path.join(scrfile_dir_ppd_1n, ppd_1n_filename.replace('.root', '.out'))
    ppd_1n_cmdfile = os.path.join(scrfile_dir_ppd_1n, ppd_1n_filename.replace('.root', '.cmd'))
    cur_ppd_file_list = [os.path.join(ppd_1m_dir, x) for x in sci_1p_ppd_1m_dict[k]]
    command = 'PPD_Split.py ' + ' '.join(cur_ppd_file_list) + ' -B ' + start_time + ' -E ' + stop_time + ' -o ' + ppd_1n_file
    with open(ppd_1n_cmdfile, 'w') as fcmd: fcmd.write(command)
    ret_value = 0
    with open(ppd_1n_outfile, 'w') as fout:
        ret_value = subprocess.call(command.split(), stdout = fout, stderr = fout)
    out_str = ''
    with open(ppd_1n_outfile, 'r') as fin: out_str = fin.read()
    with open(ppd_1n_outfile, 'w') as fout: fout.write(re.sub(r'^.*\nOpening', 'Opening', out_str))
    with open(ppd_1n_outfile, 'r') as fin: print fin.read().rstrip('\n')

