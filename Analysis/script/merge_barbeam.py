#!/usr/bin/env python

import sys
from os.path import basename
from rootpy.io import File
from rootpy.tree import Tree
from rootpy.matrix import Matrix

if len(sys.argv) < 4:
    print "USAGE: " + basename(sys.argv[0]) + " <time_win_mat.root> <decoded_file.root> <merged_file.root>"
    exit(1)

time_win_filename = sys.argv[1]
decoded_data_filename = sys.argv[2]
merged_filename = sys.argv[3]

t_file_time_win = File(time_win_filename, "read")
begin_time_mat = t_file_time_win.get("begin_time_mat")
end_time_mat   = t_file_time_win.get("end_time_mat")
max_count_mat  = t_file_time_win.get("max_count_mat")
t_file_time_win.close()

t_file_merged_out = File(merged_filename, "recreate")
t_beam_event_tree = Tree("t_beam_event", "Beam Event Data")
t_beam_event_tree.create_branches({
    'type': 'I',
    'trig_accepted': 'B[25]',
    'time_aligned': 'B[25]',
    'pkt_count': 'I',
    'lost_count': 'I',
    'trigger_bit': 'B[1600]',
    'trigger_n': 'I',
    'multiplicity': 'I[25]',
    'energy_adc': 'F[1600]',
    'compress': 'I[25]',
    'common_noise': 'F[25]',
    'bar_beam': 'B[1600]' })

t_file_merged_out.cd()
t_beam_event_tree.write()
t_file_merged_out.close()
