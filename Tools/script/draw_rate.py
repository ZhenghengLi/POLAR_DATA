#!/usr/bin/env python

import argparse
from rootpy.plotting import Hist
from rootpy.plotting import Canvas
from rootpy.interactive import wait
from sci_trigger_r import sci_trigger_r

parser = argparse.ArgumentParser(description='program to draw event rate')
parser.add_argument("filename", help = "1P or 1R level SCI data file")
parser.add_argument('-B', dest = 'begin', help = "begin GPS time as form week:second", default = 'begin')
parser.add_argument('-E', dest = 'end',   help = 'end GPS time as form week:second', default = 'end')
args = parser.parse_args()

sci_trigger_r_obj = sci_trigger_r()
sci_trigger_r_obj.open_file(args.filename, args.begin, args.end)
sci_trigger_r_obj.print_file_info()
print sci_trigger_r_obj.start_week
print sci_trigger_r_obj.start_second
print sci_trigger_r_obj.stop_week
print sci_trigger_r_obj.stop_second
print sci_trigger_r_obj.gps_time_length

# todo


sci_trigger_r_obj.close_file()
