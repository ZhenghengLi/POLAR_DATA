#!/usr/bin/env python

import argparse
from ppd_file_r import ppd_file_r
from ppd_file_w import ppd_file_w
from datetime import datetime
from tqdm import tqdm

parser = argparse.ArgumentParser(description='Split platfrom parameters data by UTC time')
parser.add_argument('filelist', metavar = 'filename', nargs = '+', help = 'list of filenames to open')
parser.add_argument('-B', dest = 'begin', help = 'utc time of beginning as gps time form, like utc_week:utc_second', default = 'begin')
parser.add_argument('-E', dest = 'end'  , help = 'utc time of ending as gps time form, like utc_week:utc_second', default = 'end')
parser.add_argument('-o', dest = 'outfile', help = 'ROOT file to store splitted platform parameters data', default = 'TG2_PPD_file_split.root')
args = parser.parse_args()

_MAX_DIFF = 10

number_of_files = len(args.filelist)
ppd_file_r_objs = [ppd_file_r() for x in xrange(number_of_files)]

# open file list
if number_of_files < 2:
    print 'Opening file: ' + args.filelist[0]
    if not ppd_file_r_objs[0].open_file(args.filelist[0], args.begin, args.end):
        print 'Error: root file open failed: ' + args.filelist[0]
        ppd_file_r_objs[0].close_file()
        exit(1)
    else:
        ppd_file_r_objs[0].print_file_info()
else:
    for i in xrange(number_of_files):
        print 'Opening file: ' + args.filelist[i]
        open_result = False
        if i == 0:
            open_result = ppd_file_r_objs[i].open_file(args.filelist[i], args.begin, 'end')
        elif i == number_of_files - 1:
            open_result = ppd_file_r_objs[i].open_file(args.filelist[i], 'begin', args.end)
        else:
            open_result = ppd_file_r_objs[i].open_file(args.filelist[i], 'begin', 'end')
        if open_result:
            ppd_file_r_objs[i].print_file_info()
            if i > 0:
                if ppd_file_r_objs[i].first_utc_time_sec - ppd_file_r_objs[i - 1].last_utc_time_sec > _MAX_DIFF:
                    print 'Error: two files cannot connect in UTC time: ' + str(ppd_file_r_objs[i].first_utc_time_sec - ppd_file_r_objs[i - 1].last_utc_time_sec)
                    for j in xrange(i + 1):
                        ppd_file_r_objs[j].close_file()
                    exit(1)
                elif ppd_file_r_objs[i].first_utc_time_sec - ppd_file_r_objs[i - 1].last_utc_time_sec < -1:
                    print 'Error: two files have overlap in UTC time: ' + str(ppd_file_r_objs[i].first_utc_time_sec - ppd_file_r_objs[i - 1].last_utc_time_sec)
                    for j in xrange(i + 1):
                        ppd_file_r_objs[j].close_file()
                    exit(1)
        else:
            print 'Error: root file open failed: ' + args.filelist[i]
            for j in xrange(i + 1):
                ppd_file_r_objs[j].close_file()
            exit(1)

# write data
ppd_file_w_obj = ppd_file_w()
ppd_file_w_obj.open_file(args.outfile)

time_is_first        = True
first_ship_time_sec  = 0
last_ship_time_sec   = 0
first_utc_time_sec   = 0
last_utc_time_sec    = 0
cur_tree_index        = -1
first_valid_index    = 0
last_valid_index     = 0
total_valid_cnt      = 0

for i in xrange(number_of_files):
    print "-----------------------------------------------------------------------------"
    print "Processing file: " + args.filelist[i] + " ... "
    for j in tqdm(xrange(ppd_file_r_objs[i].begin_entry, ppd_file_r_objs[i].end_entry)):
        ppd_file_r_objs[i].t_tree_ppd.get_entry(j)
        ppd_file_w_obj.fill_data(ppd_file_r_objs[i].t_tree_ppd)
        cur_tree_index += 1
        if ppd_file_r_objs[i].t_tree_ppd.flag_of_pos != 0x55: continue
        total_valid_cnt += 1
        if time_is_first:
            time_is_first = False
            first_ship_time_sec = ppd_file_r_objs[i].t_tree_ppd.ship_time_sec
            first_utc_time_sec  = ppd_file_r_objs[i].t_tree_ppd.utc_time_sec
            first_valid_index   = cur_tree_index
        last_ship_time_sec = ppd_file_r_objs[i].t_tree_ppd.ship_time_sec
        last_utc_time_sec  = ppd_file_r_objs[i].t_tree_ppd.utc_time_sec
        last_valid_index   = cur_tree_index
    ppd_file_r_objs[i].close_file()

ppd_file_w_obj.write_tree()

# write meta info
ppd_file_w_obj.write_meta("dattype", "PLATFORM PARAMETERS DATA AFTER SPLITTED")
ppd_file_w_obj.write_meta("version", "PPD_Split.py v1.0.0")
ppd_file_w_obj.write_meta("gentime", datetime.now().isoformat() + "+0800")
ship_time_span_str = "%d[%d] => %d[%d]; %d/%d" % (int(first_ship_time_sec), first_valid_index, int(last_ship_time_sec), last_valid_index,
                                                  total_valid_cnt, cur_tree_index + 1)
ppd_file_w_obj.write_meta("ship_time_span", ship_time_span_str)
utc_time_span_str = "%d:%d[%d] => %d:%d[%d]; %d/%d" % (int(first_utc_time_sec / 604800), int(first_utc_time_sec % 604800), first_valid_index,
                                                       int(last_utc_time_sec / 604800),  int(last_utc_time_sec % 604800),  last_valid_index,
                                                       total_valid_cnt, cur_tree_index + 1)
ppd_file_w_obj.write_meta("utc_time_span", utc_time_span_str)

ppd_file_w_obj.close_file()

print '====================================================================='
print 'ship time span: { ' + ship_time_span_str + ' }'
print 'UTC time span: { ' + utc_time_span_str + ' }'
print '====================================================================='
