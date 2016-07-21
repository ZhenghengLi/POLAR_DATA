#!/usr/bin/python

import sys
from rootpy.io import File
from rootpy.tree import Tree

if len(sys.argv) < 3:
    print 'Usage: ' + sys.argv[0] + ' <decoded_data.root> <rate_file.root>'
    exit(1)

t_file_out = File(sys.argv[2], 'recreate')
t_rate = []
for idx in xrange(25):
    tree = Tree('t_rate_ct_%02d' % (idx + 1), 'rate of module CT_%02d' % (idx + 1))
    tree.create_branches({'time_sec': 'D', 'cnts_ps': 'F[64]'})
    t_rate.append(tree)
    
t_file_in = File(sys.argv[1], 'read')
t_modules = t_file_in.get('t_modules')
t_modules.activate(['is_bad', 'ct_num', 'trigger_bit', 'time_second'], True)
#t_modules.create_buffer()

#### read and fill data #####
counts = []
for idx in xrange(25):
    counts.append([0] * 64)
first_flag = [True] * 25
pre_time = [0] * 25

for i,entry in enumerate(t_modules):
    if i % 10000 == 0:
        print i
    if entry.is_bad > 0:
        continue
    idx = entry.ct_num - 1
    if first_flag[idx]:
        first_flag[idx] = False
        pre_time[idx] = entry.time_second
        t_rate[idx].time_sec = entry.time_second
        for j in xrange(64):
            t_rate[idx].cnts_ps[j] = 0
        t_rate[idx].fill()
        continue
    for j in xrange(64):
        if entry.trigger_bit[j]:
            counts[idx][j] += 1
    if entry.time_second - pre_time[idx] > 0.1:
        t_rate[idx].time_sec = entry.time_second
        for j in xrange(64):
            t_rate[idx].cnts_ps[j] = float(counts[idx][j]) / (entry.time_second - pre_time[idx])
            counts[idx][j] = 0
        t_rate[idx].fill()
        pre_time[idx] = entry.time_second

#### read and fill data #####

t_file_out.cd()
for idx in xrange(25):
    t_rate[idx].write()

t_file_out.close()

t_file_in.close()
