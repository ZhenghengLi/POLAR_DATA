#!/usr/bin/env python

import argparse
import os
from datetime import datetime
from ppd_data import ppd_data
from ppd_file import ppd_file

parser = argparse.ArgumentParser(description='Decode platform parameters data from 1553B')
parser.add_argument("filename", help = "CSV file of platform parameters data")
parser.add_argument("-o", dest = "outfile", help = "ROOT file to store platform parameters data", default = "TG2_PPD_file.root")
args = parser.parse_args()

file_1553b = open(args.filename, 'rb')
file_size  = os.stat(args.filename).st_size

cnt_total_pkt  = 0
cnt_header_err = 0
cnt_ppd_pkt    = 0
cnt_ppd_err    = 0

ppd_data_obj = ppd_data()
ppd_file_obj = ppd_file()
ppd_file_obj.open(args.outfile)

block_size = 76
for i in xrange(file_size / block_size):
    block = file_1553b.read(block_size)
    cnt_total_pkt += 1
    if int(block[0:2].encode('hex'), 16) != 0x0180:
        cnt_header_err += 1
        continue
    if int(block[2:4].encode('hex'), 16) != 0x3d80:
        continue
    cnt_ppd_pkt += 1
    if int(block[4:6].encode('hex'), 16) != 0x3d80:
        cnt_ppd_err += 1
        continue
    ppd_data_obj.decode(block)
    ppd_data_obj.calc_j2000()
    ppd_file_obj.fill_data(ppd_data_obj)

file_1553b.close()

ppd_file_obj.write_tree()
ppd_file_obj.write_meta("dattype", "PLATFORM PARAMETERS DATA")
ppd_file_obj.write_meta("version", "PPD_Decode.py v1.0.0")
ppd_file_obj.write_meta("gentime", datetime.now().isoformat() + "+0800")
ppd_file_obj.write_meta("ship_time_span", "0")
ppd_file_obj.write_meta("utc_time_span", "1")

ppd_file_obj.close()

print '{0:<20}{1:<20d}'.format('cnt_total_pkt:',  cnt_total_pkt)
print '{0:<20}{1:<20d}'.format('cnt_header_err:', cnt_header_err)
print '{0:<20}{1:<20d}'.format('cnt_ppd_pkt:',    cnt_ppd_pkt)
print '{0:<20}{1:<20d}'.format('cnt_ppd_err:',    cnt_ppd_err)
