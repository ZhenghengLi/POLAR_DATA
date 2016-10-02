#!/usr/bin/env python

import argparse
import os
import shutil
import re
import subprocess
from datetime import datetime, timedelta
from rootpy import ROOT
from rootpy.io import File

FNULL = open(os.devnull, 'w')

def sci_1m_read_timespan(filename):
    t_file = File(filename, 'read')
    m_ped_gps_frm = t_file.get('m_ped_gps_frm')
    t_file.close()
    return m_ped_gps_frm.GetTitle()

def aux_1m_read_timespan(filename):
    t_file = File(filename, 'read')
    m_ibox_gps = t_file.get('m_ibox_gps')
    t_file.close()
    return m_ibox_gps.GetTitle()

def time_diff(sci_filename, aux_filename):
    ref_sci = re.compile(r'TS_TG02_POL_POLAR_SCI_\d+_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_\d+_1M.root')
    ref_aux = re.compile(r'TS_TG02_POL_POLAR_AUX_\d+_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_\d+_1M.root')
    sci_begin = None
    sci_end = None
    sci_m = ref_sci.match(sci_filename)
    if sci_m:
        sci_begin = datetime.strptime(sci_m.group(1), "%Y%m%d%H%M%S")
        sci_end   = datetime.strptime(sci_m.group(2), "%Y%m%d%H%M%S")
    else:
        return 0
    aux_begin = ''
    aux_end = ''
    aux_m = ref_aux.match(aux_filename)
    if aux_m:
        aux_begin = datetime.strptime(aux_m.group(1), "%Y%m%d%H%M%S")
        aux_end   = datetime.strptime(aux_m.group(2), "%Y%m%d%H%M%S")
    else:
        return 0
    return ((sci_begin - aux_begin).total_seconds(), (sci_end - aux_end).total_seconds())

def file_is_not_open(filename):
    return subprocess.call(['lsof', filename], stdout = FNULL, stderr = FNULL) != 0

def file_is_open(filename):
    return subprocess.call(['lsof', filename], stdout = FNULL, stderr = FNULL) == 0

delimeter = " " + "-" * 80
subpro_begin = "*-*-*-*-*-*-* subprocess begin *-*-*-*-*-*-*"
subpro_end   = "*-*-*-*-*-*-*- subprocess end -*-*-*-*-*-*-*"

parser = argparse.ArgumentParser(description='Generate 1M level data from a raw data folder for PSDC')
parser.add_argument("-r", dest = "pathprefix", default = "/hxmt/data/Mission/POLAR/data_in_orbit_test")
parser.add_argument("-t", dest = "type", default = "manual")
parser.add_argument("--noconfirm", help = "confirm before process", action="store_true")
args = parser.parse_args()

path_prefix = os.path.abspath(args.pathprefix)
if not os.path.isdir(path_prefix):
    print '"' + path_prefix + '"' + ' does not exist.'
    exit(1)
product_dir = os.path.join(path_prefix, "data_in_orbit_product", args.type, "internal", "decoded")
if not os.path.isdir(product_dir):
    print '"' + product_dir + '"' + ' does not exist.'
    exit(1)
logfile_dir = os.path.join(path_prefix, "data_in_orbit_product", args.type, "log", "decoded")
if not os.path.isdir(logfile_dir):
    print '"' + logfile_dir + '"' + ' does not exist.'
    exit(1)
scrfile_dir = os.path.join(path_prefix, "data_in_orbit_product", args.type, "screen", "decoded")
if not os.path.isdir(scrfile_dir):
    print '"' + scrfile_dir + '"' + ' does not exist.'
    exit(1)

# check dir of type
sci_1m, sci_1p, aux_1m, ppd_1m = 'SCI_1M', 'SCI_1P', 'AUX_1M', 'PPD_1M'
list_of_type = [sci_1m, sci_1p, aux_1m, ppd_1m]
list_of_path = [product_dir, logfile_dir, scrfile_dir]
for x in list_of_path:
    for y in list_of_type:
        if not os.path.isdir(os.path.join(x, y)):
            print '"' + os.path.join(x, y) + '"' + ' does not exist.'
            exit(1)

print " - path_prefix: " + path_prefix
print " - product_dir: " + product_dir + ' ' + str(list_of_type)
print " - logfile_dir: " + logfile_dir + '      ' + str(list_of_type)
print " - scrfile_dir: " + scrfile_dir + '   ' + str(list_of_type)
print delimeter

# get filelist
ref_sci_1M = re.compile('TS_TG02_POL_POLAR_SCI_.*_1M\.root')
ref_sci_1P = re.compile('TS_TG02_POL_POLAR_SCI_.*_1P\.root')
ref_aux_1M = re.compile('TS_TG02_POL_POLAR_AUX_.*_1M\.root')

sci_1m_filelist = []
for x in os.listdir(os.path.join(product_dir, sci_1m)):
    if ref_sci_1M.match(x): sci_1m_filelist.append(x)
sci_1m_filelist.sort()
sci_1p_filelist = []
for x in os.listdir(os.path.join(product_dir, sci_1p)):
    if ref_sci_1P.match(x): sci_1p_filelist.append(x)
sci_1p_filelist.sort()
aux_1m_filelist = []
for x in os.listdir(os.path.join(product_dir, aux_1m)):
    if ref_aux_1M.match(x): aux_1m_filelist.append(x)
aux_1m_filelist.sort()

# find new and update
sci_1m_filelist_new = []
sci_1m_filelist_update = []

FNULL = open(os.devnull, 'w')

for filename in sci_1m_filelist:
    sci_1m_file = os.path.join(product_dir, sci_1m, filename)
    sci_1p_file = os.path.join(product_dir, sci_1p, filename).replace('1M.root', '1P.root')
    if os.path.isfile(sci_1p_file):
        if os.stat(sci_1m_file).st_mtime > os.stat(sci_1p_file).st_mtime and file_is_not_open(sci_1m_file) and file_is_not_open(sci_1p_file):
            sci_1m_filelist_update.append(filename)
    else:
        if file_is_not_open(sci_1m_file):
            sci_1m_filelist_new.append(filename)

# match sci and aux
sci_1m_filelist_all = sci_1m_filelist_update + sci_1m_filelist_new
sci_1m_filelist_all.sort()
sci_aux_1m_pair_all = []
aux_search_start = 0
for idx in xrange(len(sci_1m_filelist_all)):
    if aux_search_start >= len(aux_1m_filelist): break
    bad_match = False
    while aux_search_start < len(aux_1m_filelist):
        cur_diff = time_diff(sci_1m_filelist_all[idx], aux_1m_filelist[aux_search_start])
        if cur_diff == 0:
            bad_match = True
            break
        if cur_diff[1] > 300: 
            aux_search_start += 1
        else:
            break
    if aux_search_start >= len(aux_1m_filelist):
        for q in xrange(idx, len(sci_1m_filelist_all)):
            sci_aux_1m_pair_all.append((sci_1m_filelist_all[q], 0))
        break
    if bad_match:
        sci_aux_1m_pair_all.append((sci_1m_filelist_all[idx], 0))
        continue
    found_match = False
    matched_aux_it = 0
    for aux_it in xrange(aux_search_start, len(aux_1m_filelist)):
        cur_diff = time_diff(sci_1m_filelist_all[idx], aux_1m_filelist[aux_search_start])
        if cur_diff == 0: continue
        if cur_diff[0] > -300:
            found_match = True
            matched_aux_it = aux_it
            break
        else:
            break
    if found_match and file_is_not_open(os.path.join(product_dir, aux_1m, aux_1m_filelist[matched_aux_it])):
        sci_aux_1m_pair_all.append((sci_1m_filelist_all[idx], aux_1m_filelist[matched_aux_it]))
    else:
        sci_aux_1m_pair_all.append((sci_1m_filelist_all[idx], 0))

print ' - pair summary before processing: '
for i, p in enumerate(sci_aux_1m_pair_all, start = 1):
    print ' - > ' + str(i) + ' < -----------------'
    sci_filename = p[0]
    sci_gps_span = sci_1m_read_timespan(os.path.join(product_dir, sci_1m, sci_filename))
    aux_filename = p[1]
    aux_gps_span = aux_1m_read_timespan(os.path.join(product_dir, aux_1m, aux_filename)) if aux_filename != 0 else 0
    print ' v ' + sci_filename + ': { ' + sci_gps_span + ' } '
    aux_matched = ' ^ ' + aux_filename + ': { ' + aux_gps_span + ' } ' if aux_filename != 0 else " ^ No found matched AUX file"
    print aux_matched

if not args.noconfirm:
    if raw_input("Start processing? (Y/n) ").lower() == 'n':
        exit(0)

# start process
sci_1m_filelist_success = []
sci_1m_filelist_fail = []

for i, p in enumerate(sci_aux_1m_pair_all, start = 1):
    sci_filename = p[0]
    aux_filename = p[1]
    if aux_filename == 0: continue
    sci_gps_span = sci_1m_read_timespan(os.path.join(product_dir, sci_1m, sci_filename))
    aux_gps_span = aux_1m_read_timespan(os.path.join(product_dir, aux_1m, aux_filename))
    print " " + "+" * 80
    print ' - > ' + str(i) + ' < --- processing ...'
    print ' v ' + sci_filename + ': { ' + sci_gps_span + ' } '
    print ' ^ ' + aux_filename + ': { ' + aux_gps_span + ' } '
    sci_file_1m_root = os.path.join(product_dir, sci_1m, sci_filename)
    aux_file_1m_root = os.path.join(product_dir, aux_1m, aux_filename)
    sci_file_1p_root = os.path.join(product_dir, sci_1p, sci_filename.replace('1M.root', '1P.root'))
    sci_file_1p_log  = os.path.join(logfile_dir, sci_1p, sci_filename.replace('1M.root', '1P.log'))
    sci_file_1p_cmd  = os.path.join(scrfile_dir, sci_1p, sci_filename.replace('1M.root', '1P.cmd'))
    sci_file_1p_out  = os.path.join(scrfile_dir, sci_1p, sci_filename.replace('1M.root', '1P.out'))
    if os.path.isfile(sci_file_1m_root) and file_is_open(sci_file_1m_root): continue
    if os.path.isfile(aux_file_1m_root) and file_is_open(aux_file_1m_root): continue
    if os.path.isfile(sci_file_1p_root) and file_is_open(sci_file_1p_root): continue
    if os.path.isfile(sci_file_1p_log)  and file_is_open(sci_file_1p_log):  continue
    command = 'Time_Calculate ' + sci_file_1m_root + ' -k ' + aux_file_1m_root + ' -o ' + sci_file_1p_root + ' -g ' + sci_file_1p_log
    with open(sci_file_1p_cmd, 'w') as fcmd: fcmd.write(command)
    ret_value = 0
    with open(sci_file_1p_out, 'w') as fout:
        ret_value = subprocess.call(command.split(), stdout = fout, stderr = fout)
    with open(sci_file_1p_out, 'r') as fin: print fin.read().rstrip('\n')
    if ret_value == 0:
        print ' - file: ' + sci_filename + ' successful to process.'
        sci_1m_filelist_success.append(sci_filename)
    else:
        print ' - file: ' + sci_filename + ' failed to process.'
        sci_1m_filelist_fail.append(sci_filename)
        if os.path.isfile(sci_file_1p_root): os.remove(sci_file_1p_root)
        if os.path.isfile(sci_file_1p_log):  os.remove(sci_file_1p_log)
        if os.path.isfile(sci_file_1p_cmd):  os.remove(sci_file_1p_cmd)
        if os.path.isfile(sci_file_1p_out):  os.remove(sci_file_1p_out)

# print summary
print " " + "+" * 80
print " - SUMMARY OF PROCESSING RESULT -" 
print " - successful: "
for x in sci_1m_filelist_success:
    print " > " + x
print " - failed: "
for x in sci_1m_filelist_fail:
    print " > " + x
print " " + "+" * 80

exit(len(sci_1m_filelist_success) + len(sci_1m_filelist_fail))

