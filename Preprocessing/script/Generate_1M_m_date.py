#!/usr/bin/env python

import argparse
import os
import shutil
import re
import subprocess

FNULL = open(os.devnull, 'w')

def file_is_not_open(filename):
    return subprocess.call(['lsof', filename], stdout = FNULL, stderr = FNULL) != 0

delimeter = " " + "-" * 80
subpro_begin = "*-*-*-*-*-*-* subprocess begin *-*-*-*-*-*-*"
subpro_end   = "*-*-*-*-*-*-*- subprocess end -*-*-*-*-*-*-*"

parser = argparse.ArgumentParser(description='Generate 1M level data from a raw data folder for PSDC')
parser.add_argument("date", help = "select the date")
parser.add_argument("-r", dest = "pathprefix", default = "/hxmt/data/Mission/POLAR/data_in_orbit_test")
parser.add_argument("-t", dest = "type", default = "manual")
parser.add_argument("--noconfirm", help = "confirm before process", action="store_true")
args = parser.parse_args()

path_prefix = os.path.abspath(args.pathprefix)
if not os.path.isdir(path_prefix):
    print '"' + path_prefix + '"' + ' does not exist.'
    exit(1)
rawdata_dir = os.path.join(path_prefix, 'data_in_orbit_raw', args.type, args.date)
if not os.path.isdir(rawdata_dir):
    print 'There is no data corresponding to date ' + args.date + ' found.'
    exit(0)
rawdata_dir = os.path.join(rawdata_dir, '0B')
if not os.path.isdir(rawdata_dir):
    print 'There is no 0B level data found of ' + args.date + '.'
    exit(0)
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
print " - rawdata_dir: " + rawdata_dir
print " - product_dir: " + product_dir + ' ' + str(list_of_type)
print " - logfile_dir: " + logfile_dir + '      ' + str(list_of_type)
print " - scrfile_dir: " + scrfile_dir + '   ' + str(list_of_type)
print delimeter

# get filelist
ref_0bfile = re.compile('TS_TG02_POL_.*_0B\.dat')
ref_sci = re.compile('TS_TG02_POL_POLAR_SCI_.*_0B\.dat')
ref_aux = re.compile('TS_TG02_POL_POLAR_AUX_.*_0B\.dat')
ref_eng = re.compile('TS_TG02_POL_PSD_ENG_.*_0B\.dat')
raw_filelist = [x for x in os.listdir(rawdata_dir) if ref_0bfile.match(x)]
sci_filelist = []
aux_filelist = []
eng_filelist = []
for x in raw_filelist:
    if ref_sci.match(x):
        sci_filelist.append(x)
        continue
    if ref_aux.match(x):
        aux_filelist.append(x)
        continue
    if ref_eng.match(x):
        eng_filelist.append(x)
        continue

# get new and update filelist
sci_filelist_new = []
sci_filelist_update = []
aux_filelist_new = []
aux_filelist_update = []
eng_filelist_new = []
eng_filelist_update = []

for filename in sci_filelist:
    file_0b = os.path.join(rawdata_dir, filename)
    file_1m = os.path.join(product_dir, sci_1m, filename).replace('0B.dat', '1M.root')
    if os.path.isfile(file_1m):
        if os.stat(file_0b).st_mtime > os.stat(file_1m).st_mtime and file_is_not_open(file_0b) and file_is_not_open(file_1m):
            sci_filelist_update.append(filename);
    else:
        if file_is_not_open(file_0b):
            sci_filelist_new.append(filename)

for filename in aux_filelist:
    file_0b = os.path.join(rawdata_dir, filename)
    file_1m = os.path.join(product_dir, aux_1m, filename).replace('0B.dat', '1M.root')
    if os.path.isfile(file_1m):
        if os.stat(file_0b).st_mtime > os.stat(file_1m).st_mtime and file_is_not_open(file_0b) and file_is_not_open(file_1m):
            aux_filelist_update.append(filename);
    else:
        if file_is_not_open(file_0b):
            aux_filelist_new.append(filename)

for filename in eng_filelist:
    file_0b = os.path.join(rawdata_dir, filename)
    file_1m = os.path.join(product_dir, ppd_1m, filename).replace('0B.dat', '1M.root').replace('TS_TG02_POL_PSD', 'TS_TG02_POL_PPD')
    if os.path.isfile(file_1m):
        if os.stat(file_0b).st_mtime > os.stat(file_1m).st_mtime and file_is_not_open(file_0b) and file_is_not_open(file_1m):
            eng_filelist_update.append(filename);
    else:
        if file_is_not_open(file_0b):
            eng_filelist_new.append(filename)

print " - files that are updated, need reprocess:" 
for x in sci_filelist_update:
    print ' > ' + os.path.join(args.date, '0B', x) + ' [' + str(os.stat(os.path.join(rawdata_dir, x)).st_size / 1024) + 'K]'
for x in aux_filelist_update:
    print ' > ' + os.path.join(args.date, '0B', x) + ' [' + str(os.stat(os.path.join(rawdata_dir, x)).st_size / 1024) + 'K]'
for x in eng_filelist_update:
    print ' > ' + os.path.join(args.date, '0B', x) + '   [' + str(os.stat(os.path.join(rawdata_dir, x)).st_size / 1024) + 'K]'

print " - files that are new, need process: "
for x in sci_filelist_new:
    print ' > ' + os.path.join(args.date, '0B', x) + ' [' + str(os.stat(os.path.join(rawdata_dir, x)).st_size / 1024) + 'K]'
for x in aux_filelist_new:
    print ' > ' + os.path.join(args.date, '0B', x) + ' [' + str(os.stat(os.path.join(rawdata_dir, x)).st_size / 1024) + 'K]'
for x in eng_filelist_new:
    print ' > ' + os.path.join(args.date, '0B', x) + '   [' + str(os.stat(os.path.join(rawdata_dir, x)).st_size / 1024) + 'K]'

print delimeter

if not args.noconfirm:
    if raw_input("Start processing? (Y/n) ").lower() == 'n':
        exit(0)

# start processing

sci_filelist_fail = []
sci_filelist_success = []
aux_filelist_fail = []
aux_filelist_success = []
eng_filelist_fail = []
eng_filelist_success = []

# process SCI
for filename in sci_filelist_update + sci_filelist_new:
    file_0b_dat  = os.path.join(rawdata_dir, filename)
    file_1m_root = os.path.join(product_dir, sci_1m, filename).replace('0B.dat', '1M.root')
    file_1m_log  = os.path.join(logfile_dir, sci_1m, filename).replace('0B.dat', '1M.log')
    file_1m_cmd  = os.path.join(scrfile_dir, sci_1m, filename).replace('0B.dat', '1M.cmd')
    file_1m_out  = os.path.join(scrfile_dir, sci_1m, filename).replace('0B.dat', '1M.out')
    command = 'SCI_Decode ' + file_0b_dat + ' -o ' + file_1m_root + ' -g ' + file_1m_log
    with open(file_1m_cmd, 'w') as fcmd: fcmd.write(command)
    print " " + "+" * 80
    print ' - processing file: ' + os.path.join(args.date, '0B', filename) + ' ... '
    ret_value = 0
    with open(file_1m_out, 'w') as fout: 
        ret_value = subprocess.call(command.split(), stdout = fout, stderr = fout)
    with open(file_1m_out, 'r') as fin: print fin.read().rstrip('\n')
    if ret_value == 0:
        print ' - file: ' + os.path.join(args.date, '0B', filename) + ' successful to process.'
        sci_filelist_success.append(filename)
    else:
        print ' - file: ' + os.path.join(args.date, '0B', filename) + ' failed to process.'
        sci_filelist_fail.append(filename)
        if os.path.isfile(file_1m_root): os.remove(file_1m_root)
        if os.path.isfile(file_1m_log):  os.remove(file_1m_log)
        if os.path.isfile(file_1m_cmd):  os.remove(file_1m_cmd)
        if os.path.isfile(file_1m_out):  os.remove(file_1m_out)

# process AUX
for filename in aux_filelist_update + aux_filelist_new:
    file_0b_dat  = os.path.join(rawdata_dir, filename)
    file_1m_root = os.path.join(product_dir, aux_1m, filename).replace('0B.dat', '1M.root')
    file_1m_log  = os.path.join(logfile_dir, aux_1m, filename).replace('0B.dat', '1M.log')
    file_1m_cmd  = os.path.join(scrfile_dir, aux_1m, filename).replace('0B.dat', '1M.cmd')
    file_1m_out  = os.path.join(scrfile_dir, aux_1m, filename).replace('0B.dat', '1M.out')
    command = 'HK_Decode ' + file_0b_dat + ' -o ' + file_1m_root + ' -g ' + file_1m_log
    with open(file_1m_cmd, 'w') as fcmd: fcmd.write(command)
    print " " + "+" * 80
    print ' - processing file: ' + os.path.join(args.date, '0B', filename) + ' ... '
    ret_value = 0
    with open(file_1m_out, 'w') as fout: 
        ret_value = subprocess.call(command.split(), stdout = fout, stderr = fout)
    with open(file_1m_out, 'r') as fin: print fin.read().rstrip('\n')
    if ret_value == 0:
        print ' - file: ' + os.path.join(args.date, '0B', filename) + ' successful to process.'
        aux_filelist_success.append(filename)
    else:
        print ' - file: ' + os.path.join(args.date, '0B', filename) + ' failed to process.'
        aux_filelist_fail.append(filename)
        if os.path.isfile(file_1m_root): os.remove(file_1m_root)
        if os.path.isfile(file_1m_log):  os.remove(file_1m_log)
        if os.path.isfile(file_1m_cmd):  os.remove(file_1m_cmd)
        if os.path.isfile(file_1m_out):  os.remove(file_1m_out)

# process ENG
for filename in eng_filelist_update + eng_filelist_new:
    file_0b_dat  = os.path.join(rawdata_dir, filename)
    file_1m_root = os.path.join(product_dir, ppd_1m, filename).replace('0B.dat', '1M.root').replace('TS_TG02_POL_PSD', 'TS_TG02_POL_PPD')
    file_1m_log  = os.path.join(logfile_dir, ppd_1m, filename).replace('0B.dat', '1M.log').replace('TS_TG02_POL_PSD', 'TS_TG02_POL_PPD')
    file_1m_cmd  = os.path.join(scrfile_dir, ppd_1m, filename).replace('0B.dat', '1M.cmd').replace('TS_TG02_POL_PSD', 'TS_TG02_POL_PPD')
    file_1m_out  = os.path.join(scrfile_dir, ppd_1m, filename).replace('0B.dat', '1M.out').replace('TS_TG02_POL_PSD', 'TS_TG02_POL_PPD')
    command = 'PPD_Decode.py ' + file_0b_dat + ' -o ' + file_1m_root + ' -g ' + file_1m_log
    with open(file_1m_cmd, 'w') as fcmd: fcmd.write(command)
    print " " + "+" * 80
    print ' - processing file: ' + os.path.join(args.date, '0B', filename) + ' ... '
    ret_value = 0
    with open(file_1m_out, 'w') as fout: 
        ret_value = subprocess.call(command.split(), stdout = fout, stderr = fout)
    with open(file_1m_out, 'r') as fin: print fin.read().rstrip('\n')
    if ret_value == 0:
        print ' - file: ' + os.path.join(args.date, '0B', filename) + ' successful to process.'
        eng_filelist_success.append(filename)
    else:
        print ' - file: ' + os.path.join(args.date, '0B', filename) + ' failed to process.'
        eng_filelist_fail.append(filename)
        if os.path.isfile(file_1m_root): os.remove(file_1m_root)
        if os.path.isfile(file_1m_log):  os.remove(file_1m_log)
        if os.path.isfile(file_1m_cmd):  os.remove(file_1m_cmd)
        if os.path.isfile(file_1m_out):  os.remove(file_1m_out)

# print summary
print " " + "+" * 80
print " - SUMMARY OF PROCESSING RESULT -" 
print " - successful: "
for x in sci_filelist_success:
    print " > " + os.path.join(args.date, '0B', x)
for x in aux_filelist_success:
    print " > " + os.path.join(args.date, '0B', x)
for x in eng_filelist_success:
    print " > " + os.path.join(args.date, '0B', x)
print " - failed: "
for x in sci_filelist_fail:
    print " > " + os.path.join(args.date, '0B', x)
for x in aux_filelist_fail:
    print " > " + os.path.join(args.date, '0B', x)
for x in eng_filelist_fail:
    print " > " + os.path.join(args.date, '0B', x)
print " " + "+" * 80

exit(len(sci_filelist_success) + len(aux_filelist_success) + len(eng_filelist_success) \
        + len(sci_filelist_fail) + len(aux_filelist_fail) + len(eng_filelist_fail))

