#!/usr/bin/env python

import argparse
import os, re, time
from datetime import datetime, timedelta
import subprocess

delimeter = " " + "-" * 80

ref_time = re.compile(r'(\d+):(\d+)\[\d+\] => (\d+):(\d+)\[\d+\].*')

FNULL = open(os.devnull, 'w')

def read_timespan(filename):
    t_file = File(filename, 'read')
    m = ref_time.match(t_file.get('m_utc_span').GetTitle())
    week1, second1, week2, second2 = int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(4))
    t_file.close()
    return (week1 * 604800 + second1, week2 * 604800 + second2)

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
    ref_filename = re.compile(r'T2_POL_.*_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_1[M|P](_part)?\.root')
else:
    ref_filename = re.compile(r'TS_TG02_POL_.*_\d+_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_\d+_1[M|P](_part)?\.root')

ref_filename_part = re.compile(r'T[2|S]_.*_1[M|P]_part\.root')

all_file_list_sci_1p = [x for x in os.listdir(sci_1p_dir) if ref_filename.match(x)]
all_file_list_ppd_1m = [x for x in os.listdir(ppd_1m_dir) if ref_filename.match(x)]

def calc_time(filename):
    m = ref_filename.match(filename)
    return (datetime.strptime(m.group(1), timeformat), datetime.strptime(m.group(2), timeformat))

def get_begin_time_str(filename):
    return ref_filename.match(filename).group(1)

class filename_time:
    def __init__(self, filename):
        self.fn = filename
        time_span = calc_time(filename)
        self.begin_time = time_span[0]
        self.end_time   = time_span[1]
    def __hash__(self):
        return hash(self.fn)
    def __lt__(self, right):
        if isinstance(right, filename_time):
            return self.begin_time < right.begin_time
        else:
            return NotImplemented
    def __le__(self, right):
        if isinstance(right, filename_time):
            return self.begin_time <= right.begin_time
        else:
            return NotImplemented
    def __eq__(self, right):
        if isinstance(right, filename_time):
            return self.begin_time == right.begin_time
        else:
            return NotImplemented
    def __gt__(self, right):
        if isinstance(right, filename_time):
            return self.begin_time > right.begin_time
        else:
            return NotImplemented
    def __ge__(self, right):
        if isinstance(right, filename_time):
            return self.begin_time >= right.begin_time
        else:
            return NotImplemented

start_time = datetime.strptime(args.timefrom, timeformat)
stop_time  = datetime.strptime(args.timeto, timeformat)
file_list_sci_1p = []
file_list_ppd_1m = []
for x in all_file_list_sci_1p:
    if ref_filename_part.match(x): continue
    ppd_1n_filename = x.replace('_SCI_', '_PPD_').replace('_1P', '_1N')
    if os.path.isfile(os.path.join(ppd_1n_dir, ppd_1n_filename)): continue
    filename_time_obj = filename_time(x)
    if filename_time_obj.begin_time >= start_time and filename_time_obj.end_time <= stop_time:
        file_list_sci_1p.append(filename_time_obj)
for x in all_file_list_ppd_1m:
    if ref_filename_part.match(x): continue
    filename_time_obj = filename_time(x)
    if filename_time_obj.end_time >= start_time and filename_time_obj.begin_time <= stop_time:
        file_list_ppd_1m.append(filename_time_obj)

if len(file_list_sci_1p) < 2 or len(file_list_ppd_1m) < 2:
    print 'too small number of files.'
    exit(0)

# cope with overlap
_MIN_DIFF = 1
file_list_sci_1p.sort()
file_list_ppd_1m.sort()
file_list_ppd_1m_alone = [file_list_ppd_1m[0]]
pre_begin_time = file_list_ppd_1m_alone[0].begin_time
pre_end_time   = file_list_ppd_1m_alone[0].end_time
pre_filename   = file_list_ppd_1m_alone[0].fn
for x in file_list_ppd_1m[1:]:
    cur_begin_time = x.begin_time
    cur_end_time   = x.end_time
    cur_filename   = x.fn
    if (cur_end_time - pre_end_time).total_seconds() < 5:
        continue
    elif (cur_begin_time - pre_end_time).total_seconds() < 0:
        pre_timespan = read_timespan(os.path.join(ppd_1m_dir, pre_filename))
        cur_timespan = read_timespan(os.path.join(ppd_1m_dir, cur_filename))
        if pre_timespan[1] - cur_timespan[0] < 1:
            file_list_ppd_1m_alone.append(x)
            pre_begin_time = cur_begin_time
            pre_end_time   = cur_end_time
            pre_filename   = cur_filename
        else:
            # split cur_filename from pre_timespan[1] to end as cur_filename_part
            # use the part file instead
            start_time_sec = pre_timespan[1] + 1
            start_time_str = (cur_begin_time + timedelta(seconds = start_time_sec - cur_timespan[0])).strftime(timeformat)
            cur_filename_part = cur_filename.replace(get_begin_time_str(cur_filename), start_time_str)
            cur_filename_part = cur_filename_part.replace('_1M.root', '_1M_part.root')
            if os.path.isfile(os.path.join(ppd_1m_dir, cur_filename_part)):
                filename_time_obj = filename_time(cur_filename_part)
                file_list_ppd_1m_alone.append(filename_time_obj)
                pre_begin_time = filename_time_obj.begin_time
                pre_end_time   = filename_time_obj.end_time
                pre_filename   = filename_time_obj.fn
            else:
                start_time = '%d:%d' % (start_time_sec / 604800, start_time_sec % 604800)
                command = 'PPD_Split.py ' + os.path.join(ppd_1m_dir, cur_filename) + ' -B ' + start_time + \
                        ' -o ' + os.path.join(ppd_1m_dir, cur_filename_part)
                print 'coping with overlap ...'
                print ' - ' + cur_filename
                print ' - ' + cur_filename_part
                ret_value = subprocess.call(command.split(), stdout = FNULL, stderr = FNULL)
                if ret_value > 0:
                    print ' # error: failed to split file. '
                    if os.path.isfile(os.path.join(ppd_1m_dir, cur_filename_part)):
                        os.remove(os.path.join(ppd_1m_dir, cur_filename_part))
                else:
                    print ' - successful'
                    filename_time_obj = filename_time(cur_filename_part)
                    file_list_ppd_1m_alone.append(filename_time_obj)
                    pre_begin_time = filename_time_obj.begin_time
                    pre_end_time   = filename_time_obj.end_time
                    pre_filename   = filename_time_obj.fn
    else:
        file_list_ppd_1m_alone.append(x)
        pre_begin_time = cur_begin_time
        pre_end_time   = cur_end_time
        pre_filename   = cur_filename

# find ppd_1m for sci_1p
sci_1p_ppd_1m_dict = {}
ppd_1m_start_index = 0
for x in file_list_sci_1p:
    sci_1p_begin_time, sci_1p_end_time = x.begin_time, x.end_time
    ppd_1m_begin_time = file_list_ppd_1m_alone[ppd_1m_start_index].begin_time
    ppd_1m_end_time   = file_list_ppd_1m_alone[ppd_1m_start_index].end_time
    while ppd_1m_end_time < sci_1p_begin_time:
        ppd_1m_start_index += 1
        ppd_1m_begin_time = file_list_ppd_1m_alone[ppd_1m_start_index].begin_time
        ppd_1m_end_time   = file_list_ppd_1m_alone[ppd_1m_start_index].end_time
    if ppd_1m_begin_time > sci_1p_begin_time: continue
    index_shift = 0
    ppd_1m_list = [file_list_ppd_1m_alone[ppd_1m_start_index]]
    found_gap = False
    reach_end = False
    while ppd_1m_end_time < sci_1p_end_time:
        index_shift += 1
        cur_index = ppd_1m_start_index + index_shift
        if cur_index >= len(file_list_ppd_1m_alone):
            reach_end = True
            break
        pre_end_time = ppd_1m_end_time
        ppd_1m_begin_time = file_list_ppd_1m_alone[cur_index].begin_time
        ppd_1m_end_time   = file_list_ppd_1m_alone[cur_index].end_time
        if (ppd_1m_begin_time - pre_end_time).total_seconds() > 25:
            found_gap = True
            break
        ppd_1m_list.append(file_list_ppd_1m_alone[cur_index])
    if found_gap or reach_end: continue
    sci_1p_ppd_1m_dict[x] = ppd_1m_list

for k in sorted(sci_1p_ppd_1m_dict.keys()):
    print k.fn
    for p in sci_1p_ppd_1m_dict[k]:
        print ' - ' + p.fn
    sci_1p_begin_time, sci_1p_end_time = k.begin_time, k.end_time
    start_total_seconds = (sci_1p_begin_time - datetime(1980, 1, 6, 8, 0, 0)).total_seconds() - 20
    stop_total_seconds  = (sci_1p_end_time   - datetime(1980, 1, 6, 8, 0, 0)).total_seconds() + 20
    first_time_seconds = (sci_1p_ppd_1m_dict[k][0].begin_time - datetime(1980, 1, 6, 8, 0, 0)).total_seconds()
    last_time_seconds = (sci_1p_ppd_1m_dict[k][-1].end_time - datetime(1980, 1, 6, 8, 0, 0)).total_seconds()
    start_week   = int(start_total_seconds / 604800)
    start_second = int(start_total_seconds % 604800)
    stop_week    = int(stop_total_seconds  / 604800)
    stop_second  = int(stop_total_seconds  % 604800)
    start_time = "%d:%d" % (start_week, start_second) if start_total_seconds > first_time_seconds + 10.0 else 'begin'
    stop_time  = "%d:%d" % (stop_week,  stop_second) if stop_total_seconds < last_time_seconds - 10.0 else 'end'
    print start_time + ' => ' + stop_time
    ppd_1n_filename = k.fn.replace('_SCI_', '_PPD_').replace('_1P', '_1N')
    print ppd_1n_filename
    ppd_1n_file = os.path.join(os.path.join(ppd_1n_dir, ppd_1n_filename))
    ppd_1n_outfile = os.path.join(scrfile_dir_ppd_1n, ppd_1n_filename.replace('.root', '.out'))
    ppd_1n_cmdfile = os.path.join(scrfile_dir_ppd_1n, ppd_1n_filename.replace('.root', '.cmd'))
    cur_ppd_file_list = [os.path.join(ppd_1m_dir, x.fn) for x in sci_1p_ppd_1m_dict[k]]
    command = 'PPD_Split.py ' + ' '.join(cur_ppd_file_list) + ' -B ' + start_time + ' -E ' + stop_time + ' -o ' + ppd_1n_file
    with open(ppd_1n_cmdfile, 'w') as fcmd: fcmd.write(command)
    ret_value = 0
    with open(ppd_1n_outfile, 'w') as fout:
        ret_value = subprocess.call(command.split(), stdout = fout, stderr = fout)
    if ret_value > 0:
        if os.path.isfile(ppd_1n_file): os.remove(ppd_1n_file)
        if os.path.isfile(ppd_1n_cmdfile): os.remove(ppd_1n_cmdfile)
        if os.path.isfile(ppd_1n_outfile): os.remove(ppd_1n_outfile)
    out_str = ''
    with open(ppd_1n_outfile, 'r') as fin: out_str = fin.read()
    out_str_lines = [re.sub(r'^.*Opening', 'Opening', x) for x in out_str.split('\n')]
    out_str_lines = [x for x in out_str_lines if not re.search(r'%', x)]
    with open(ppd_1n_outfile, 'w') as fout: fout.write('\n'.join(out_str_lines))
    with open(ppd_1n_outfile, 'r') as fin: print fin.read().rstrip('\n')

