#!/usr/bin/env python

from split_time import find_maintenance
import sys

if len(sys.argv) < 2:
    print "USAGE: " + sys.argv[0] + " <ppd_file.root>"
    exit(2)

maintenance_list = find_maintenance(sys.argv[1])
print maintenance_list

