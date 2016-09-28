#!/usr/bin/env python

import argparse
import os
import shutil
import re
import subprocess

delimeter = " " + "-" * 80
subpro_begin = "*-*-*-*-*-*-* subprocess begin *-*-*-*-*-*-*"
subpro_end   = "*-*-*-*-*-*-*- subprocess end -*-*-*-*-*-*-*"

parser = argparse.ArgumentParser(description='Generate 1M level data from a raw data folder for PSDC')
parser.add_argument("-r", dest = "pathprefix", default = "/hxmt/data/Mission/POLAR/data_in_orbit_test")
parser.add_argument("-t", dest = "type", default = "normal")
args = parser.parse_args()

path_prefix = os.path.abspath(args.pathprefix)
if not os.path.isdir(path_prefix):
    print '"' + path_prefix + '"' + ' does not exist.'
    exit(1)
rawdata_dir = os.path.join(path_prefix, 'data_in_orbit_raw', args.type)
if not os.path.isdir(rawdata_dir):
    print '"' + rawdata_dir + '"' + ' does not exist.'
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
print " - rawdata_dir: " + rawdata_dir
print " - product_dir: " + product_dir
print " - logfile_dir: " + logfile_dir
print " - scrfile_dir: " + scrfile_dir
print delimeter

rawdata_dates = [x for x in os.listdir(rawdata_dir) if os.path.isdir(os.path.join(rawdata_dir, x))]
product_dates = [x for x in os.listdir(product_dir) if os.path.isdir(os.path.join(product_dir, x))]

new_dates = rawdata_dates
if len(new_dates) < 1:
    print " - There is no new raw data to process for 1M generating."
    exit(0)

print " - Found new raw datasets to generate 1M data product: "
for x in new_dates:
    print " > " + x

cur_date = raw_input(" - choose one to process: ")
if cur_date not in new_dates:
    print " - " + cur_date + " is not in the new datasets"
    exit(1)

cur_raw_path = os.path.join(rawdata_dir, cur_date)
print delimeter
print subpro_begin
subprocess.call('tree ' + cur_raw_path, shell = True)
print subpro_end
print delimeter

check_md5 = raw_input(' - check md5? (Y/n) ').lower()
if check_md5 == 'y' or check_md5 == '':
    md5file = 'MD5SUMS_' + cur_date + '.txt'
    if not os.path.isfile(os.path.join(cur_raw_path, md5file)):
        print '"' + md5file + '"' + ' does not exist.'
        exit(1)
    print subpro_begin
    ret = subprocess.call('cd ' + cur_raw_path + ' && ' + 'md5sum -c ' + md5file, shell = True)
    print subpro_end
    if not ret:
        print ' - all md5 is ok'
    else:
        print ' - some md5 is not ok'
        con = raw_input(' - continue to process? (y/N) ').lower()
        if con == 'n' or con == '':
            print " - Abort."
            exit(1)
        else:
            print " - Continue."

print delimeter

ref_0bfile = re.compile('T2_POL_.*_0B\.dat')
ref_sci = re.compile('T2_POL_POLAR_SCI_.*_0B\.dat')
ref_aux = re.compile('T2_POL_POLAR_AUX_.*_0B\.dat')
ref_psd = re.compile('T2_POL_PSD_ENG_.*_0B\.dat')
cur_raw_path_0B = os.path.join(cur_raw_path, '0B')
raw_filelist = [x for x in os.listdir(cur_raw_path_0B) if ref_0bfile.match(x)]
sci_filelist = []
aux_filelist = []
psd_filelist = []
for x in raw_filelist:
    if ref_sci.match(x):
        sci_filelist.append(x)
        continue
    if ref_aux.match(x):
        aux_filelist.append(x)
        continue
    if ref_psd.match(x):
        psd_filelist.append(x)
        continue
print " - raw data files that need to process: "
print " - SCI file list:"
for x in sci_filelist:
    print ' > ' + x
print " - AUX file list:"
for x in aux_filelist:
    print ' > ' + x
print " - PSD file list:"
for x in psd_filelist:
    print ' > ' + x

print delimeter

start_flag = raw_input(' - start generating? (Y/n) ').lower()
if start_flag == 'n':
    print ' - Abort.'
    exit(1)

# internal
cur_decoded_path_int = os.path.join(product_dir, cur_date)
print ' - create directory: ' + cur_decoded_path_int
if os.path.isdir(cur_decoded_path_int):
    print cur_decoded_path_int + " => exist, so omit"
else:
    os.mkdir(cur_decoded_path_int)
cur_1M_path_int = os.path.join(cur_decoded_path_int, '1M')
print ' - create directory: ' + cur_1M_path_int
if os.path.isdir(cur_1M_path_int):
    print cur_1M_path_int + " => exist, so omit"
else:
    os.mkdir(cur_1M_path_int)

# log
cur_decoded_path_log = os.path.join(logfile_dir, cur_date)
print ' - create directory: ' + cur_decoded_path_log
if os.path.isdir(cur_decoded_path_log):
    print cur_decoded_path_log + " => exist, so omit"
else:
    os.mkdir(cur_decoded_path_log)
cur_1M_path_log = os.path.join(cur_decoded_path_log, '1M')
print ' - create directory: ' + cur_1M_path_log
if os.path.isdir(cur_1M_path_log):
    print cur_1M_path_log + " => exist, so omit"
else:
    os.mkdir(cur_1M_path_log)

# screen
cur_decoded_path_scr = os.path.join(scrfile_dir, cur_date)
print ' - create directory: ' + cur_decoded_path_scr
if os.path.isdir(cur_decoded_path_scr):
    print cur_decoded_path_scr + " => exist, so omit"
else:
    os.mkdir(cur_decoded_path_scr)
cur_1M_path_scr = os.path.join(cur_decoded_path_scr, '1M')
print ' - create directory: ' + cur_1M_path_scr
if os.path.isdir(cur_1M_path_scr):
    print cur_1M_path_scr + " => exist, so omit"
else:
    os.mkdir(cur_1M_path_scr)

failed_files = []

print ' - generating 1M level SCI data ... '
for x in sci_filelist:
    print ' > processing: ' + x
    cur_1M_rootfn = x.replace('0B.dat', '1M.root')
    cur_1M_logfn  = x.replace('0B.dat', '1M.log')
    cur_1M_cmdfn  = x.replace('0B.dat', '1M.cmd')
    cur_1M_outfn  = x.replace('0B.dat', '1M.out')
    cur_raw_file  = os.path.join(cur_raw_path_0B, x)
    cur_root_file = os.path.join(cur_1M_path_int, cur_1M_rootfn)
    if os.path.isfile(cur_root_file):
        print cur_root_file + ' => exist, so omit'
        continue
    cur_log_file  = os.path.join(cur_1M_path_log, cur_1M_logfn)
    cur_cmd_file  = os.path.join(cur_1M_path_scr, cur_1M_cmdfn)
    cur_out_file  = os.path.join(cur_1M_path_scr, cur_1M_outfn)
    command = 'SCI_Decode ' + cur_raw_file + ' -o ' + cur_root_file + ' -g ' + cur_log_file
    with open(cur_cmd_file, 'w') as f: f.write(command)
    print subpro_begin
    ret_val = subprocess.call('set -o pipefail; sh ' + cur_cmd_file + ' | tee ' + cur_out_file, shell = True, executable='/bin/bash')
    print subpro_end
    if ret_val < 1:
        print ' > done.'
    else:
        failed_files.append(cur_raw_file)
        print ' > ERROR occurred.'

print ' - generating 1M level AUX data ... '
for x in aux_filelist:
    print ' > processing: ' + x 
    cur_1M_rootfn = x.replace('0B.dat', '1M.root')
    cur_1M_logfn  = x.replace('0B.dat', '1M.log')
    cur_1M_cmdfn  = x.replace('0B.dat', '1M.cmd')
    cur_1M_outfn  = x.replace('0B.dat', '1M.out')
    cur_raw_file  = os.path.join(cur_raw_path_0B, x)
    cur_root_file = os.path.join(cur_1M_path_int, cur_1M_rootfn)
    if os.path.isfile(cur_root_file):
        print cur_root_file + ' => exist, so omit'
        continue
    cur_log_file  = os.path.join(cur_1M_path_log, cur_1M_logfn)
    cur_cmd_file  = os.path.join(cur_1M_path_scr, cur_1M_cmdfn)
    cur_out_file  = os.path.join(cur_1M_path_scr, cur_1M_outfn)
    command = 'HK_Decode ' + cur_raw_file + ' -o ' + cur_root_file + ' -g ' + cur_log_file
    with open(cur_cmd_file, 'w') as f: f.write(command)
    print subpro_begin
    ret_val = subprocess.call('set -o pipefail; sh ' + cur_cmd_file + ' | tee ' + cur_out_file, shell = True, executable='/bin/bash')
    print subpro_end
    if ret_val < 1:
        print ' > done.'
    else:
        failed_files.append(cur_raw_file)
        print ' > ERROR occurred.'

print ' - generating 1M level PPD data ... '
for x in psd_filelist:
    print ' > processing: ' + x 
    cur_1M_rootfn = x.replace('0B.dat', '1M.root').replace('T2_POL_PSD', 'T2_POL_PPD')
    cur_1M_logfn  = x.replace('0B.dat', '1M.log').replace('T2_POL_PSD', 'T2_POL_PPD')
    cur_1M_cmdfn  = x.replace('0B.dat', '1M.cmd').replace('T2_POL_PSD', 'T2_POL_PPD')
    cur_1M_outfn  = x.replace('0B.dat', '1M.out').replace('T2_POL_PSD', 'T2_POL_PPD')
    cur_raw_file  = os.path.join(cur_raw_path_0B, x)
    cur_root_file = os.path.join(cur_1M_path_int, cur_1M_rootfn)
    if os.path.isfile(cur_root_file):
        print cur_root_file + ' => exist, so omit'
        continue
    cur_log_file  = os.path.join(cur_1M_path_log, cur_1M_logfn)
    cur_cmd_file  = os.path.join(cur_1M_path_scr, cur_1M_cmdfn)
    cur_out_file  = os.path.join(cur_1M_path_scr, cur_1M_outfn)
    command = 'PPD_Decode.py ' + cur_raw_file + ' -o ' + cur_root_file + ' -g ' + cur_log_file
    with open(cur_cmd_file, 'w') as f: f.write(command)
    print subpro_begin
    ret_val = subprocess.call('set -o pipefail; sh ' + cur_cmd_file + ' | tee ' + cur_out_file, shell = True, executable='/bin/bash')
    print subpro_end
    if ret_val < 1:
        print ' > done.'
    else:
        failed_files.append(cur_raw_file)
        print ' > ERROR occurred.'

if len(failed_files) > 0:
    print ' - The following raw data files are failed to process:'
    for x in failed_files:
        print x
else:
    print ' - All raw data files are processed successfully.'

