#!/usr/bin/python

import sys
import re
import numpy as np
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print "USAGE: " + sys.argv[0] + " <infile_name>"
    exit(1)

infile_name = sys.argv[1]

ref = re.compile(r'^\d+$')
first_flag = True
pre_num = 0
count_1 = 0
count_2 = 0
tot_cnt = 0
for (n, line) in enumerate(open(infile_name, 'r')):
    if not ref.match(line):
        continue
    tot_cnt = tot_cnt + 1
    if first_flag:
        first_flag = False
        pre_num = int(line)
    else:
        cur_num = int(line)
        diff = cur_num - pre_num
        pre_num = cur_num
        if diff < 0:
            diff = diff + 65536
        if diff > 1:
            count_1 = count_1 + diff - 1
            count_2 = count_2 + 1

print count_1
print count_2
print tot_cnt
print float(count_1) / float(tot_cnt)
print float(count_2) / float(tot_cnt)


