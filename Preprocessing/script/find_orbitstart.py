#!/usr/bin/env python

from split_time import find_orbitstart
import sys

if len(sys.argv) < 2:
    print "USAGE: " + sys.argv[0] + " <ppd_file.root>"
    exit(2)

orbitstart_list = find_orbitstart(sys.argv[1])
print orbitstart_list

