#!/usr/bin/env python

import sys
from os.path import basename
from rootpy.io import File
from rootpy.tree import Tree
from rootpy.matrix import Matrix
from tqdm import tqdm

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

t_file_decoded_data = File(decoded_data_filename, "read")
t_trigger = t_file_decoded_data.get("t_trigger")
t_trigger.deactivate(['status_bit', 'trig_sig_con_bit'], True)
t_trigger.create_buffer()
t_modules = t_file_decoded_data.get("t_modules")
t_modules.deactivate(['status_bit'], True)
t_modules.create_buffer()

t_file_merged_out = File(merged_filename, "recreate")
t_beam_event = Tree("t_beam_event", "Beam Event Data")
t_beam_event.create_branches({
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

for k in tqdm(xrange(t_trigger.get_entries())):
    t_trigger.get_entry(k)
    if t_trigger.is_bad != 0: continue
    t_beam_event.type = t_trigger.type
    t_beam_event.pkt_count = t_trigger.pkt_count
    t_beam_event.lost_count = t_trigger.lost_count
    t_beam_event.trigger_n = t_trigger.trigger_n
    for i in xrange(25):
        t_beam_event.trig_accepted[i] = t_trigger.trig_accepted[i]
        t_beam_event.time_aligned[i] = False
        t_beam_event.multiplicity[i] = 0
        t_beam_event.compress[i] = -1
        t_beam_event.common_noise[i] = 0
        for j in xrange(64):
            t_beam_event.trigger_bit[64 * i + j] = False
            t_beam_event.energy_adc[64 * i + j] = 0
            t_beam_event.bar_beam[64 * i + j] = False
    for q in xrange(t_trigger.pkt_start, t_trigger.pkt_start + t_trigger.pkt_count):
        t_modules.get_entry(q)
        idx = t_modules.ct_num - 1
        t_beam_event.time_aligned[idx] = True
        t_beam_event.multiplicity[idx] = t_modules.multiplicity
        t_beam_event.compress[idx] = t_modules.compress
        t_beam_event.common_noise[idx] = t_modules.common_noise
        for j in xrange(64):
            t_beam_event.trigger_bit[64 * idx + j] = t_modules.trigger_bit[j]
            t_beam_event.energy_adc[64 * idx + j] = t_modules.energy_adc[j]
            if t_modules.time_second >= begin_time_mat(idx, j) and t_modules.time_second <= end_time_mat(idx, j) and max_count_mat(idx, j) > 200:
                t_beam_event.trigger_bit[64 * idx + j] = True
    t_beam_event.fill()

t_file_merged_out.cd()
t_beam_event.write()
t_file_merged_out.close()
t_file_decoded_data.close()

