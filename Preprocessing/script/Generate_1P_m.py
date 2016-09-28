#!/usr/bin/env python

import argparse
import os
import shutil
import re
import subprocess
from rootpy import ROOT
from rootpy.io import File

delimeter = " " + "-" * 80
subpro_begin = "*-*-*-*-*-*-* subprocess begin *-*-*-*-*-*-*"
subpro_end   = "*-*-*-*-*-*-*- subprocess end -*-*-*-*-*-*-*"

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

parser = argparse.ArgumentParser(description='Generate 1P level data from a 1M data folder for PSDC')
parser.add_argument("-r", dest = "pathprefix", default = "/hxmt/data/Mission/POLAR/data_in_orbit_test")
parser.add_argument("-t", dest = "type", default = "manual")
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

print " - path_prefix: " + path_prefix
print " - product_dir: " + product_dir
print " - logfile_dir: " + logfile_dir
print " - scrfile_dir: " + scrfile_dir
print delimeter

product_dates = [x for x in os.listdir(product_dir) if os.path.isdir(os.path.join(product_dir, x))]

new_dates = product_dates

#for x in product_dates:
#    if os.path.isdir(os.path.join(product_dir, x, '1M')) and not os.path.isdir(os.path.join(product_dir, x, '1P')):
#        new_dates.append(x)

if len(new_dates) < 1:
    print " - There is no new 1M data to process for 1P generating."
    exit(0)

print " - Found new 1M datasets to generate 1P data product: "
for x in new_dates:
    print " > " + x

cur_date = raw_input(" - choose one to process: ")
if cur_date not in new_dates:
    print " - " + cur_date + " is not in the new datasets"
    exit(1)

cur_1M_path = os.path.join(product_dir, cur_date, '1M')
if not os.path.isdir(os.path.join(logfile_dir, cur_date)):
    print ' - ERROR: log directory for ' + cur_date + ' does not exist.'
    exit(1)
if not os.path.isdir(os.path.join(scrfile_dir, cur_date)):
    print ' - ERROR: screen directory for ' + cur_date + ' does not exist.'
    exit(1)

sci_1M_filelist = []
aux_1M_filelist = []

ref_sci_1M = re.compile('TS_TG02_POL_POLAR_SCI_.*_1M\.root')
ref_aux_1M = re.compile('TS_TG02_POL_POLAR_AUX_.*_1M\.root')

for x in os.listdir(cur_1M_path):
    if ref_sci_1M.match(x):
        sci_1M_filelist.append(x)
        continue
    if ref_aux_1M.match(x):
        aux_1M_filelist.append(x)
        continue

sci_1M_filelist.sort()
aux_1M_filelist.sort()

print " - 1M data files of " + cur_date + " that need to process: "
print " - SCI 1M file list:"
for i, x in enumerate(sci_1M_filelist, start = 1):
    print ' > (' + str(i) + ') ' + x + ': { ' + sci_1m_read_timespan(os.path.join(cur_1M_path, x)) + ' } '
print " - AUX 1M file list:"
for i, x in enumerate(aux_1M_filelist, start = 1):
    print ' > (' + str(i) + ') ' + x + ': { ' + aux_1m_read_timespan(os.path.join(cur_1M_path, x)) + ' } '

con = raw_input(' - Continue? (Y/n) ').lower()
if con == 'n':
    print ' - Abort.'
    exit(0)

sci_aux_pair = []

for i in xrange(len(sci_1M_filelist)):
    aux_i = int(raw_input(' - choose AUX file number for SCI file (' + str(i + 1) + '): '))
    if aux_i == 0: continue
    if aux_i not in range(1, len(aux_1M_filelist) + 1):
        print ' - ERROR: the number you choosed is out of range.'
        exit(1)
    sci_aux_pair.append((i, aux_i - 1))

print ' - pair summary before processing: '
for i, p in enumerate(sci_aux_pair, start = 1):
    cur_sci_1M_fn = sci_1M_filelist[p[0]]
    cur_aux_1M_fn = aux_1M_filelist[p[1]]
    print ' - > ' + str(i) + ' < -----------------'
    print ' v ' + cur_sci_1M_fn + ': { ' + sci_1m_read_timespan(os.path.join(cur_1M_path, cur_sci_1M_fn)) + ' } '
    print ' ^ ' + cur_aux_1M_fn + ': { ' + aux_1m_read_timespan(os.path.join(cur_1M_path, cur_aux_1M_fn)) + ' } '

start_flag = raw_input(' - start processing? (Y/n) ').lower()
if start_flag == 'n':
    print ' - Abort.'
    exit(0)

print delimeter

# internal
cur_1P_path_int = os.path.join(product_dir, cur_date, '1P')
print ' - create directory: ' + cur_1P_path_int
if os.path.isdir(cur_1P_path_int):
    print cur_1P_path_int + " => exist, so omit"
else:
    os.mkdir(cur_1P_path_int)

# log
cur_1P_path_log = os.path.join(logfile_dir, cur_date, '1P')
print ' - create directory: ' + cur_1P_path_log
if os.path.isdir(cur_1P_path_log):
    print cur_1P_path_log + " => exist, so omit"
else:
    os.mkdir(cur_1P_path_log)

# screen
cur_1P_path_scr = os.path.join(scrfile_dir, cur_date, '1P')
print ' - create directory: ' + cur_1P_path_scr
if os.path.isdir(cur_1P_path_scr):
    print cur_1P_path_scr + " => exist, so omit"
else:
    os.mkdir(cur_1P_path_scr)

failed_files = []

for i, p in enumerate(sci_aux_pair, start = 1):
    print ' - > ' + str(i) + ' < --- processing ...'
    cur_sci_1M_fn    = sci_1M_filelist[p[0]]
    cur_aux_1M_fn    = aux_1M_filelist[p[1]]
    print ' v ' + cur_sci_1M_fn + ': { ' + sci_1m_read_timespan(os.path.join(cur_1M_path, cur_sci_1M_fn)) + ' } '
    print ' ^ ' + cur_aux_1M_fn + ': { ' + aux_1m_read_timespan(os.path.join(cur_1M_path, cur_aux_1M_fn)) + ' } '
    cur_1M_sci_file  = os.path.join(cur_1M_path, cur_sci_1M_fn)
    cur_1M_aux_file  = os.path.join(cur_1M_path, cur_aux_1M_fn)
    cur_1P_rootfn    = cur_sci_1M_fn.replace('1M.root', '1P.root')
    cur_1P_logfn     = cur_sci_1M_fn.replace('1M.root', '1P.log')
    cur_1P_cmdfn     = cur_sci_1M_fn.replace('1M.root', '1P.cmd')
    cur_1P_outfn     = cur_sci_1M_fn.replace('1M.root', '1P.out')
    cur_1P_root_file = os.path.join(cur_1P_path_int, cur_1P_rootfn)
    cur_1P_log_file  = os.path.join(cur_1P_path_log, cur_1P_logfn)
    cur_1P_cmd_file  = os.path.join(cur_1P_path_scr, cur_1P_cmdfn)
    cur_1P_out_file  = os.path.join(cur_1P_path_scr, cur_1P_outfn)
    if os.path.isfile(cur_1P_root_file):
        if os.stat(cur_1M_sci_file).st_mtime < os.stat(cur_1P_root_file).st_mtime:
            print cur_1P_rootfn + " => exist, so omit"
            continue
        else:
            print "1M data file update, so regenerate"
    command = 'Time_Calculate ' + cur_1M_sci_file + ' -k ' + cur_1M_aux_file + ' -o ' + cur_1P_root_file + ' -g ' + cur_1P_log_file
    with open(cur_1P_cmd_file, 'w') as f: f.write(command)
    print subpro_begin
    ret_val = subprocess.call('set -o pipefail; sh ' + cur_1P_cmd_file + ' | tee ' + cur_1P_out_file, shell=True, executable='/bin/bash')
    print subpro_end
    if ret_val < 1:
        print ' > done.'
    else:
        failed_files.append(cur_1M_sci_file)
        print ' > ERROR occurred.'

if len(failed_files) > 0:
    print ' - The following 1M SCI files are failed to generate its 1P file:'
    for x in failed_files:
        print x
else:
    print ' - All 1M SCI files are successful to generate its 1P file.'

