#!/usr/bin/env python

SW_NAME      = 'PPD_Decode.py'
SW_VERSION   = 'v1.0.0'
RELEASE_DATE = '2016 Sep  5'

import argparse
import os
from os.path import basename
from datetime import datetime
from dateutil.tz import tzlocal
from ppd_data import ppd_data
from ppd_file_w import ppd_file_w
from tqdm import tqdm

parser = argparse.ArgumentParser(description='Decode platform parameters data from 1553B')
parser.add_argument("filename", help = "0B level 1553B raw data file")
parser.add_argument("-o", dest = "outfile", help = "ROOT file to store platform parameters data", default = "TG2_PPD_file.root")
parser.add_argument("-g", dest = "logfile", help = "text file to store error log from raw data", default = "NULL")
args = parser.parse_args()

file_1553b = open(args.filename, 'rb')
file_size  = os.stat(args.filename).st_size

cnt_total_pkt  = 0
cnt_header_err = 0
cnt_ppd_pkt    = 0
cnt_ppd_err    = 0

ppd_data_obj = ppd_data()
ppd_file_w_obj = ppd_file_w()
ppd_file_w_obj.open_file(args.outfile)

log_file = None
log_flag = False
if args.logfile != "NULL":
    log_flag = True
    log_file = open(args.logfile, 'w')
    log_file.write("*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" + "\n")
    log_file.write(SW_NAME + " " + SW_VERSION + "\n")
    log_file.write("LOG START TIME: " + datetime.now(tzlocal()).isoformat() + "\n")
    log_file.write("*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" + "\n")
    log_file.write("\n")

time_is_first        = True
first_ship_time_sec  = 0
last_ship_time_sec   = 0
first_utc_time_sec   = 0
last_utc_time_sec    = 0
cur_tree_index       = -1
first_valid_index    = 0
last_valid_index     = 0
total_valid_cnt      = 0

block_size = 76
print "Decoding file: " + basename(args.filename) + " ..."
for i in tqdm(xrange(file_size / block_size)):
    block = file_1553b.read(block_size)
    cnt_total_pkt += 1
    if int(block[0:2].encode('hex'), 16) != 0x0180:
        cnt_header_err += 1
        if cnt_header_err / (cnt_total_pkt + 9) > 0.5:
            print 'may be there is half 1553b frame at the start.'
            exit(1)
        if log_flag:
            log_file.write("==== 1553B FRAME " + str(cnt_total_pkt) + ": " + "header error ====")
            log_file.write("[ " + block.encode('hex') + " ]")
        continue
    if int(block[2:4].encode('hex'), 16) != 0x3d80:
        continue
    cnt_ppd_pkt += 1
    if int(block[4:6].encode('hex'), 16) != 0x3d80:
        cnt_ppd_err += 1
        if log_flag:
            log_file.write("==== 1553B FRAME " + str(cnt_total_pkt) + ": " + "ppd error ====")
            log_file.write("[ " + block.encode('hex') + " ]")
        continue
    ppd_data_obj.decode(block)
    ppd_data_obj.calc_j2000()
    ppd_file_w_obj.fill_data(ppd_data_obj)
    cur_tree_index += 1
    if ppd_data_obj.flag_of_pos != 0x55 or ppd_data_obj.utc_time_sec < 0: continue
    total_valid_cnt += 1
    if time_is_first:
        time_is_first = False
        first_ship_time_sec = ppd_data_obj.ship_time_sec
        first_utc_time_sec  = ppd_data_obj.utc_time_sec
        first_valid_index   = cur_tree_index
    last_ship_time_sec = ppd_data_obj.ship_time_sec
    last_utc_time_sec  = ppd_data_obj.utc_time_sec
    last_valid_index   = cur_tree_index

file_1553b.close()
if log_flag:
    log_file.close()

ppd_file_w_obj.write_tree()
ppd_file_w_obj.write_meta("m_dattype", "PLATFORM PARAMETERS DATA")
ppd_file_w_obj.write_meta("m_version", SW_NAME + " " + SW_VERSION)
ppd_file_w_obj.write_meta("m_gentime", datetime.now(tzlocal()).isoformat())
ppd_file_w_obj.write_meta("m_rawfile", basename(args.filename))
ship_time_span_str = "%d[%d] => %d[%d]; %d/%d" % (int(first_ship_time_sec), first_valid_index, int(last_ship_time_sec), last_valid_index,
                                                  total_valid_cnt, cur_tree_index + 1)
ppd_file_w_obj.write_meta("m_shipspan", ship_time_span_str)
utc_time_span_str = "%d:%d[%d] => %d:%d[%d]; %d/%d" % (int(first_utc_time_sec / 604800), int(first_utc_time_sec % 604800), first_valid_index,
                                                       int(last_utc_time_sec / 604800),  int(last_utc_time_sec % 604800),  last_valid_index,
                                                       total_valid_cnt, cur_tree_index + 1)
ppd_file_w_obj.write_meta("m_utc_span", utc_time_span_str)

ppd_file_w_obj.close_file()

print '====================================================================='
print '{0:<20}{1:<20d}'.format('cnt_total_pkt:',  cnt_total_pkt)
print '{0:<20}{1:<20d}'.format('cnt_header_err:', cnt_header_err)
print '{0:<20}{1:<20d}'.format('cnt_ppd_pkt:',    cnt_ppd_pkt)
print '{0:<20}{1:<20d}'.format('cnt_ppd_err:',    cnt_ppd_err)
print '---------------------------------------------------------------------'
print 'shiptime span: { ' + ship_time_span_str + ' }'
print 'UTC time span: { ' + utc_time_span_str + ' }'
print '====================================================================='
