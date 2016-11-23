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

def sci_1p_read_timespan(filename):
    t_file = File(filename, 'read')
    m_pedship = t_file.get('m_pedship')
    t_file.close()
    return m_pedship.GetTitle()

def aux_1m_read_timespan(filename):
    t_file = File(filename, 'read')
    m_oboxship = t_file.get('m_oboxship')
    t_file.close()
    return m_oboxship.GetTitle()

def ppd_1n_read_timespan(filename):
    t_file = File(filename, 'read')
    m_shipspan = t_file.get('m_shipspan')
    t_file.close()
    return m_shipspan.GetTitle()

def time_diff(sci_filename, aux_filename):
    ref_sci = re.compile(r'T2_POL_POLAR_SCI_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_1P.root')
    ref_aux = re.compile(r'T2_POL_POLAR_AUX_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_(\d\d\d\d\d\d\d\d\d\d\d\d\d\d)_1M.root')
    sci_begin = None
    sci_end = None
    sci_m = ref_sci.match(sci_filename)
    if sci_m:
        sci_begin = datetime.strptime(sci_m.group(1), "%Y%m%d%H%M%S")
        sci_end   = datetime.strptime(sci_m.group(2), "%Y%m%d%H%M%S")
    else:
        return 0
    aux_begin = None
    aux_end = None
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

parser = argparse.ArgumentParser(description='Generate SCI_1Q data')
parser.add_argument("-r", dest = "pathprefix", default = "/hxmt/data/Mission/POLAR/data_in_orbit_test")
parser.add_argument("-t", dest = "type", default = "normal")
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
scrfile_dir = os.path.join(path_prefix, "data_in_orbit_product", args.type, "screen", "decoded")
if not os.path.isdir(scrfile_dir):
    print '"' + scrfile_dir + '"' + ' does not exist.'
    exit(1)

# check dir of type
sci_1q, sci_1p, aux_1m, ppd_1n = 'SCI_1Q', 'SCI_1P', 'AUX_1M', 'PPD_1N'
list_of_type = [sci_1q, sci_1p, aux_1m, ppd_1n]
list_of_path = [product_dir, scrfile_dir]
for x in list_of_path:
    for y in list_of_type:
        if not os.path.isdir(os.path.join(x, y)):
            print '"' + os.path.join(x, y) + '"' + ' does not exist.'
            exit(1)

print " - path_prefix: " + path_prefix
print " - product_dir: " + product_dir + ' ' + str(list_of_type)
print " - scrfile_dir: " + scrfile_dir + '   ' + str(list_of_type)
print delimeter

# get filelist
ref_sci_1Q = re.compile('T2_POL_POLAR_SCI_.*_1Q\.root')
ref_sci_1P = re.compile('T2_POL_POLAR_SCI_.*_1P\.root')
ref_aux_1M = re.compile('T2_POL_POLAR_AUX_.*_1M\.root')
ref_ppd_1N = re.compile('T2_POL_POLAR_PPD_.*_1N\.root')

sci_1q_filelist = []
for x in os.listdir(os.path.join(product_dir, sci_1q)):
    if ref_sci_1Q.match(x): sci_1q_filelist.append(x)
sci_1q_filelist.sort()
sci_1p_filelist = []
for x in os.listdir(os.path.join(product_dir, sci_1p)):
    if ref_sci_1P.match(x): sci_1p_filelist.append(x)
sci_1p_filelist.sort()
aux_1m_filelist = []
for x in os.listdir(os.path.join(product_dir, aux_1m)):
    if ref_aux_1M.match(x): aux_1m_filelist.append(x)
aux_1m_filelist.sort()

# find new and update
sci_1p_filelist_new = []
sci_1p_filelist_update = []

for filename in sci_1p_filelist:
    sci_1p_file = os.path.join(product_dir, sci_1p, filename)
    sci_1q_file = os.path.join(product_dir, sci_1q, filename).replace('1P.root', '1Q.root')
    if os.path.isfile(sci_1q_file):
        if os.stat(sci_1p_file).st_mtime > os.stat(sci_1q_file).st_mtime and file_is_not_open(sci_1p_file) and file_is_not_open(sci_1q_file):
            sci_1p_filelist_update.append(filename)
    else:
        if file_is_not_open(sci_1p_file):
            sci_1p_filelist_new.append(filename)

# match sci and aux
sci_1p_filelist_all = sci_1p_filelist_update + sci_1p_filelist_new
sci_1p_filelist_all.sort()
sci_aux_pair_all = []
aux_search_start = 0
for idx in xrange(len(sci_1p_filelist_all)):
    if aux_search_start >= len(aux_1m_filelist): break
    bad_match = False
    while aux_search_start < len(aux_1m_filelist):
        cur_diff = time_diff(sci_1p_filelist_all[idx], aux_1m_filelist[aux_search_start])
        if cur_diff == 0:
            bad_match = True
            break
        if cur_diff[1] > 300:
            aux_search_start += 1
        else:
            break
    if aux_search_start >= len(aux_1m_filelist):
        for q in xrange(idx, len(sci_1p_filelist_all)):
            sci_aux_pair_all.append((sci_1p_filelist_all[q], 0))
        break
    if bad_match:
        sci_aux_pair_all.append((sci_1p_filelist_all[idx], 0))
        continue
    found_match = False
    matched_aux_it = 0
    for aux_it in xrange(aux_search_start, len(aux_1m_filelist)):
        cur_diff = time_diff(sci_1p_filelist_all[idx], aux_1m_filelist[aux_it])
        if cur_diff == 0: continue
        if cur_diff[0] > -300:
            found_match = True
            matched_aux_it = aux_it
            break
        else:
            break
    if found_match and file_is_not_open(os.path.join(product_dir, aux_1m, aux_1m_filelist[matched_aux_it])):
        sci_aux_pair_all.append((sci_1p_filelist_all[idx], aux_1m_filelist[matched_aux_it]))
    else:
        sci_aux_pair_all.append((sci_1p_filelist_all[idx], 0))

# find ppd data file
sci_aux_ppd_tuple_all = []
for x in sci_aux_pair_all:
    sci_filename = x[0]
    ppd_filename = x[0].replace('_SCI_', '_PPD_').replace('_1P', '_1N')
    if os.path.isfile(os.path.join(product_dir, ppd_1n, ppd_filename)):
        sci_aux_ppd_tuple_all.append(x + (ppd_filename,))
    else:
        sci_aux_ppd_tuple_all.append(x + (0,))

print ' - matching summary before processing: '
for i, p in enumerate(sci_aux_ppd_tuple_all, start = 1):
    print ' - > ' + str(i) + ' < -----------------'
    sci_filename = p[0]
    sci_timespan = sci_1p_read_timespan(os.path.join(product_dir, sci_1m, sci_filename))
    aux_filename = p[1]
    aux_timespan = aux_1m_read_timespan(os.path.join(product_dir, aux_1m, aux_filename)) if aux_filename != 0 else 0
    ppd_filename = p[2]
    ppd_timespan = ppd_1n_read_timespan(os.path.join(product_dir, ppd_1n, ppd_filename)) if ppd_filename != 0 else 0
    print ' v ' + sci_filename + ': { ' + sci_timespan + ' } '
    aux_matched = ' ^ ' + aux_filename + ': { ' + aux_timespan + ' } ' if aux_filename != 0 else " ^ No found matched AUX file"
    print aux_matched
    ppd_matched = ' ^ ' + ppd_filename + ': { ' + ppd_timespan + ' } ' if ppd_filename != 0 else " ^ No found matched PPD file"
    print ppd_matched

if not args.noconfirm:
    if raw_input("Start processing? (Y/n) ").lower() == 'n':
        exit(0)

# start process
sci_1p_filelist_success = []
sci_1p_filelist_fail = []

for i, p in enumerate(sci_aux_ppd_tuple_all, start = 1):
    sci_filename = p[0]
    aux_filename = p[1]
    ppd_filename = p[2]
    if aux_filename == 0: continue
    if ppd_filename == 0: continue
    sci_timespan = sci_1p_read_timespan(os.path.join(product_dir, sci_1p, sci_filename))
    aux_timespan = aux_1m_read_timespan(os.path.join(product_dir, aux_1m, aux_filename))
    ppd_timespan = ppd_1n_read_timespan(os.path.join(product_dir, ppd_1n, ppd_filename))
    print " " + "+" * 80
    print ' - > ' + str(i) + ' < --- processing ...'
    print ' v ' + sci_filename + ': { ' + sci_timespan + ' } '
    print ' ^ ' + aux_filename + ': { ' + aux_timespan + ' } '
    print ' ^ ' + ppd_filename + ': { ' + ppd_timespan + ' } '
    sci_file_1p_root = os.path.join(product_dir, sci_1p, sci_filename)
    aux_file_1m_root = os.path.join(product_dir, aux_1m, aux_filename)
    ppd_file_1n_root = os.path.join(product_dir, ppd_1n, ppd_filename)
    sci_file_1q_root = os.path.join(product_dir, sci_1q, sci_filename.replace('1P.root', '1Q.root'))
    sci_file_1q_cmd  = os.path.join(scrfile_dir, sci_1q, sci_filename.replace('1P.root', '1Q.cmd'))
    sci_file_1q_out  = os.path.join(scrfile_dir, sci_1q, sci_filename.replace('1P.root', '1Q.out'))
    if os.path.isfile(sci_file_1p_root) and file_is_open(sci_file_1p_root): continue
    if os.path.isfile(aux_file_1m_root) and file_is_open(aux_file_1m_root): continue
    if os.path.isfile(ppd_file_1n_root) and file_is_open(ppd_file_1n_root): continue
    if os.path.isfile(sci_file_1q_root) and file_is_open(sci_file_1q_root): continue
    command = 'SAP_Merge ' + sci_file_1p_root + ' -a ' + aux_file_1m_root + ' -p ' + ppd_file_1n_root + ' -o ' + sci_file_1q_root
    with open(sci_file_1q_cmd, 'w') as fcmd: fcmd.write(command)
    ret_value = 0
    with open(sci_file_1q_out, 'w') as fout:
        ret_value = subprocess.call(command.split(), stdout = fout, stderr = fout)
    with open(sci_file_1q_out, 'r') as fin: print fin.read().rstrip('\n')
    if ret_value == 0:
        print ' - file: ' + sci_filename + ' successful to process.'
        sci_1p_filelist_success.append(sci_filename)
    else:
        print ' - file: ' + sci_filename + ' failed to process.'
        sci_1p_filelist_fail.append(sci_filename)
        if os.path.isfile(sci_file_1q_root): os.remove(sci_file_1q_root)
        if os.path.isfile(sci_file_1q_cmd):  os.remove(sci_file_1q_cmd)
        if os.path.isfile(sci_file_1q_out):  os.remove(sci_file_1q_out)

# print summary
print " " + "+" * 80
print " - SUMMARY OF PROCESSING RESULT -"
print " - successful: "
for x in sci_1p_filelist_success:
    print " > " + x
print " - failed: "
for x in sci_1p_filelist_fail:
    print " > " + x
print " " + "+" * 80

exit(len(sci_1p_filelist_success) + len(sci_1p_filelist_fail))

