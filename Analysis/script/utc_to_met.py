#!/usr/bin/env python

import argparse
import re
from datetime import datetime, timedelta

parser = argparse.ArgumentParser(description='program to convert UTC time to MET time')
parser.add_argument("utc_str", help = "utc_str")
args = parser.parse_args()

print "UTC_time         => " + args.utc_str

cur_datetime = datetime.strptime(args.utc_str, "%Y-%m-%dT%H:%M:%S")
dt = cur_datetime - datetime(2016, 9, 15, 14, 13, 48)
total_second = dt.total_seconds()
if total_second > 9279972:
    total_second += 1
print "MET_time         => " + str(total_second)
