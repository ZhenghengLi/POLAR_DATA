#!/usr/bin/env python

import argparse
import os
import re
import subprocess

parser = argparse.ArgumentParser(description='Generate 1M level data from a raw data folder for PSDC')
parser.add_argument("-r", dest = "pathprefix", default = "/hxmt/data/Mission/POLAR/data_in_orbit_test")
parser.add_argument("-t", dest = "type", default = "normal")
args = parser.parse_args()

path_prefix = args.pathprefix
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

delimeter = " " + "-" * 80
print " - path_prefix: " + path_prefix
print " - rawdata_dir: " + rawdata_dir
print " - product_dir: " + product_dir
print delimeter

rawdata_dates = [x for x in os.listdir(rawdata_dir) if os.path.isdir(os.path.join(rawdata_dir, x))]
product_dates = [x for x in os.listdir(product_dir) if os.path.isdir(os.path.join(product_dir, x))]

new_dates = list(set(rawdata_dates) - set(product_dates))
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

cur_rawpath = os.path.join(rawdata_dir, cur_date)
print delimeter
subprocess.call('tree ' + cur_rawpath, shell = True)
print delimeter

check_md5 = raw_input(' - check md5? (Y/n) ').lower()
if check_md5 == 'y' or check_md5 == '':
    md5file = 'MD5SUMS_' + cur_date + '.txt'
    if not os.path.isfile(os.path.join(cur_rawpath, md5file)):
        print '"' + md5file + '"' + ' does not exist.'
        exit(1)
    ret = subprocess.call('cd ' + cur_rawpath + ' && ' + 'md5sum -c ' + md5file, shell = True)
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
raw_filelist = [x for x in os.listdir(os.path.join(cur_rawpath, '0B')) if ref_0bfile.match(x)]
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

# make dir in internal/
# delete dir in log/ if exists
# delete dir in screen/ if exists

print ' - generating 1M level SCI data ... '

print ' - generating 1M level AUX data ... '

print ' - generating 1M level PPD data ... '


