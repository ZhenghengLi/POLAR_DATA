#!/usr/bin/python

import sys
import re

if len(sys.argv) < 2:
    print "USAGE: " + sys.argv[0] + " <infile_name>"
infile_name = sys.argv[1]
error_mod = {}
short_mod = {}

ref_error = re.compile(r'^\[ \w\w \w\w \w\w (\w\w) .*\]$')
ref_short = re.compile(r'^\[ \w\w \w\w \w\w (\w\w) \]$')
line_num = 0
for line in open(infile_name, 'r'):
    line_num = line_num + 1
    m = ref_error.match(line)
    if m:
        mod_id = m.group(1)
        if mod_id in error_mod:
            error_mod[mod_id] = error_mod[mod_id] + 1
        else:
            error_mod[mod_id] = 1
    m = ref_short.match(line)
    if m:
        mod_id = m.group(1)
        if mod_id in short_mod:
            short_mod[mod_id] = short_mod[mod_id] + 1
        else:
            short_mod[mod_id] = 1
print "error_mod: ", error_mod
print "short_mod: ", short_mod
