#!/usr/bin/env python

import argparse
import re
from datetime import datetime, timedelta

parser = argparse.ArgumentParser(description='program to convert GPS time to UTC time')
parser.add_argument("utc_str", help = "week:second of GPS time")
parser.add_argument('-l', dest = "leapsecond", help = 'current leapsecond', type = int, default = 17)
args = parser.parse_args()

print "UTC_time         => " + args.utc_str
print "Leap_second      => " + str(args.leapsecond)

cur_datetime = datetime.strptime(args.utc_str, "%Y-%m-%dT%H:%M:%S")
dt = cur_datetime - datetime(1980, 1, 6, 0, 0, 0) + timedelta(seconds = args.leapsecond)
total_second = dt.total_seconds()
week = int(total_second / 604800)
second = int(total_second % 604800)
gps_str = "%d:%d" % (week, second)
print "GPS_time         => " + gps_str
