#!/usr/bin/env python

import argparse
import re
from datetime import datetime, timedelta

parser = argparse.ArgumentParser(description='program to convert GPS time to UTC time')
parser.add_argument("weeksecond", help = "week:second of GPS time")
parser.add_argument('-l', dest = "leapsecond", help = 'current leapsecond', type = int, default = 17)
args = parser.parse_args()

print "GPS_time => " + args.weeksecond
print "Leap_second => " + str(args.leapsecond)

ref_weeksecond = re.compile(r'^(\d+):(\d+\.?\d*)$');
week = 0
second = 0
m = ref_weeksecond.match(args.weeksecond)
if m:
    week = int(m.group(1))
    second = float(m.group(2))
else:
    print "ERROR: bad gps time."
    exit(1)

total_second = week * 604800 + second
cur_datetime = datetime(1980, 1, 6, 0, 0, 0) + timedelta(seconds = total_second - args.leapsecond);
time_str = cur_datetime.isoformat()
print "UTC_time => " + time_str

