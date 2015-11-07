#!/usr/bin/python

import sys
if len(sys.argv) < 3:
    print "USAGE: " + sys.argv[0] + " <infile_name> <outfile_name>"
    exit(1)
infile_name = sys.argv[1]
outfile_name = sys.argv[2]

circle_time = 2**23
ped_circle = 24414
max_diff = 10
period_T = 0
period_E = [0 for it in range(25)]

trigg_curr_time = 0
trigg_next_time = 0
event_curr_time = [0 for it in range(25)]

trigg_events = []
trigg_events_ct = []

trigg_is_first = True
event_is_first = [True for it in range(25)]

mod_time_diff = [{} for it in range(25)]

def is_valid(events):
    if len(events) > 25 or len(events) < 5:
        return False
    flag = True
    cur_ev = events[0]
    for ev in events[1:]:
        if abs(ev[1] - cur_ev[1]) < max_diff:
            cur_ev = ev
        else:
            flag = False
            break
    return flag

count = {}
count['invalid'] = 0
count['valid'] = 0

outfile = open(outfile_name, 'w')

for (line_num, line) in enumerate(open(infile_name, 'r'), start = 1):
    (tp, info) = line.rstrip().split(' : ')
    if tp == 'NT' or tp == 'NE': continue
    if tp == 'PT':
        if trigg_is_first:
            trigg_is_first = False
            period_T = 0
            trigg_curr_time = int(info)
            trigg_events = []
            trigg_events_ct = []
        else:
            trigg_next_time = int(info)
            if is_valid(trigg_events):
                count['valid'] = count['valid'] + 1
                for event in trigg_events:
                    ct_num = event[0]
                    diff = (period_T - period_E[ct_num - 1]) * circle_time + (trigg_curr_time - event[1])
                    if diff in mod_time_diff[ct_num - 1]:
                        mod_time_diff[ct_num - 1][diff] = mod_time_diff[ct_num - 1][diff] + 1
                    else:
                        mod_time_diff[ct_num - 1][diff] = 1
                if trigg_next_time - trigg_curr_time < -1 * ped_circle: 
                    period_T = period_T + 1
                trigg_curr_time = trigg_next_time
                trigg_events = []
                trigg_events_ct = []
            else:
                count['invalid'] = count['invalid'] + 1
                outfile.write("-- " + str(line_num) + " --------------\n")
                outfile.write("PT : " + str(trigg_curr_time) + "\n")
                for event in trigg_events:
                    outfile.write("PE : " + str(event[1]) + " " + str(event[0]) + "\n")
                if trigg_next_time - trigg_curr_time < -1 * ped_circle:
                    period_T = period_T + 1
                trigg_curr_time = trigg_next_time
                trigg_events = []
                trigg_events_ct = []
    elif tp == 'PE':
        (time, ct_num) = [int(it) for it in info.split(' ')]
        if event_is_first[ct_num - 1]:
            event_is_first[ct_num - 1] = False
            period_E[ct_num - 1] = 0
            event_curr_time[ct_num - 1] = time
            if ct_num not in trigg_events_ct:
                trigg_events.append((ct_num, time))
                trigg_events_ct.append(ct_num)
        else:
            if time - event_curr_time[ct_num - 1] < -1 * ped_circle:
                period_E[ct_num - 1] = period_E[ct_num - 1] + 1
            event_curr_time[ct_num - 1] = time
            if ct_num not in trigg_events_ct:
                trigg_events.append((ct_num, time))
                trigg_events_ct.append(ct_num)

outfile.close()

for it in mod_time_diff:
    print it

print "---------------------------------"
print count

                    

                    



