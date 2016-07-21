#!/usr/bin/python

import sys
from rootpy.io import File
from rootpy.tree import Tree
from rootpy.matrix import Matrix

if len(sys.argv) < 3:
    print 'Usage: ' + sys.argv[0] + ' <rate_file.root> <time_win.root>'
    exit(1)

t_file_out = File(sys.argv[2], 'recreate')
begin_time_mat = Matrix(25, 64)
end_time_mat   = Matrix(25, 64)
max_count_mat  = Matrix(25, 64)
max_time_mat   = Matrix(25, 64)

max_index_mat  = [x[:] for x in [[0] * 64] * 25]

t_file_in = File(sys.argv[1], 'read')
t_rate = [None] * 25
for idx in xrange(25):
    t_rate[idx] = t_file_in.get('t_rate_ct_%02d' % (idx + 1))
    t_rate[idx].create_buffer()

# ===============================

max_time_mat.Zero()
for idx in xrange(25):
    print 'Processing CT_' + str(idx + 1) + ' ...'
    for it, entry in enumerate(t_rate[idx]):
        for j in xrange(64):
            if entry.cnts_ps[j] > max_count_mat[idx][j]:
                max_count_mat[idx][j] = entry.cnts_ps[j]
                max_time_mat[idx][j]  = entry.time_sec
                max_index_mat[idx][j] = it

for idx in xrange(25):
    for j in xrange(64):
        begin_time_mat[idx][j] = max_time_mat[idx][j]
        end_time_mat[idx][j]   = max_time_mat[idx][j]
        for t in xrange(1, 15):
            if max_index_mat[idx][j] - t < 0: break
            t_rate[idx].get_entry(max_index_mat[idx][j] - t)
            begin_time_mat[idx][j] = t_rate[idx].time_sec
            if t_rate[idx].cnts_ps[j] < max_count_mat[idx][j] * 0.5: break
        for t in xrange(1, 15):
            if max_index_mat[idx][j] + t > t_rate[idx].get_entries() - 1: break
            t_rate[idx].get_entry(max_index_mat[idx][j] + t)
            if t_rate[idx].cnts_ps[j] < max_count_mat[idx][j] * 0.5: break
            end_time_mat[idx][j] = t_rate[idx].time_sec

# ===============================

t_file_out.cd()
begin_time_mat.Write("begin_time_mat")
end_time_mat.Write("end_time_mat")
max_count_mat.Write("max_count_mat")
max_time_mat.Write("max_time_mat")

t_file_out.close()

t_file_in.close()

