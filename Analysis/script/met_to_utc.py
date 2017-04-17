#!/usr/bin/env python

import argparse
import re
from datetime import datetime, timedelta

parser = argparse.ArgumentParser(description='program to convert met time to utc time')
parser.add_argument("met", help = "met time")
args = parser.parse_args()

print "met_time         => " + args.met

met_time = int(args.met)
if (met_time > 9279972):
    met_time -= 1

utc_datetime = datetime(2016, 9, 15, 14, 13, 48) + timedelta(seconds = met_time);
utc_time_str = utc_datetime.isoformat()
beijing_datetime = utc_datetime + timedelta(seconds = 28800);
beijing_time_str = beijing_datetime.isoformat()
print "utc_time         => " + utc_time_str
print "beijing_time     => " + beijing_time_str

