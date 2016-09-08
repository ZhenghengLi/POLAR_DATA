#!/usr/bin/env python

import argparse
from rootpy import ROOT
from rootpy.plotting import Hist
from rootpy.plotting import Canvas
from rootpy.interactive import wait
from tqdm import tqdm
from sci_trigger_r import sci_trigger_r

delimeter = " " + "-" * 80

parser = argparse.ArgumentParser(description='program to draw event rate')
parser.add_argument("filename", help = "1P or 1R level SCI data file")
parser.add_argument('-B', dest = 'begin', help = "begin GPS time as form week:second", default = 'begin')
parser.add_argument('-E', dest = 'end',   help = 'end GPS time as form week:second', default = 'end')
parser.add_argument('-w', dest = "bw", help = 'bin width', type = float, default = 1.0)
args = parser.parse_args()

sci_trigger_r_obj = sci_trigger_r()
if not sci_trigger_r_obj.open_file(args.filename, args.begin, args.end):
    print 'ERROR: root file open failed: ' + args.filename
    exit(1)
sci_trigger_r_obj.print_file_info()
nbins = int(sci_trigger_r_obj.gps_time_length / args.bw)

print delimeter
print ' - time length:     ' + str(sci_trigger_r_obj.gps_time_length) + ' seconds'
print ' - bin width:       ' + str(args.bw) + ' seconds'
print ' - number of bins:  ' + str(nbins)
print delimeter

ROOT.gStyle.SetOptStat(0)

gps_time_span = '%d:%d => %d:%d' % (int(sci_trigger_r_obj.start_week), int(sci_trigger_r_obj.start_second), 
                                    int(sci_trigger_r_obj.stop_week),  int(sci_trigger_r_obj.stop_second))
trigger_hist = Hist(nbins, 0, sci_trigger_r_obj.gps_time_length, 
                    type = 'D', name = 'trigger_hist', title = 'trigger: { ' + gps_time_span + ' }')
modules_hist = [Hist(nbins, 0, sci_trigger_r_obj.gps_time_length, 
                     type = 'D', name = 'module_hist_' + str(i), title = 'module CT_' + str(i) + ': { ' + gps_time_span + ' }') 
                for i in xrange(1, 26)]
trigger_hist.SetDirectory(None)
trigger_hist.SetMinimum(0)
trigger_hist.color = 'red'
trigger_hist.markerstyle = 3
for i in xrange(25):
    modules_hist[i].SetDirectory(None)
    modules_hist[i].SetMinimum(0)
    modules_hist[i].color = 'blue'
    modules_hist[i].markerstyle = 3

print ' - reading data ... '
for i in tqdm(xrange(sci_trigger_r_obj.begin_entry, sci_trigger_r_obj.end_entry)):
    sci_trigger_r_obj.t_trigger.get_entry(i)
    if not sci_trigger_r_obj.t_trigger.abs_gps_valid: continue
    trigger_hist.fill((sci_trigger_r_obj.t_trigger.abs_gps_week   - sci_trigger_r_obj.start_week) * 604800 + 
                      (sci_trigger_r_obj.t_trigger.abs_gps_second - sci_trigger_r_obj.start_second))
    for j in xrange(25):
        if sci_trigger_r_obj.t_trigger.trig_accepted[j]:
            modules_hist[j].fill((sci_trigger_r_obj.t_trigger.abs_gps_week   - sci_trigger_r_obj.start_week) * 604800 +
                                 (sci_trigger_r_obj.t_trigger.abs_gps_second - sci_trigger_r_obj.start_second))

for i in xrange(1, nbins + 1):
    trigger_hist.SetBinContent(i, trigger_hist.GetBinContent(i) / args.bw)
    trigger_hist.SetBinError(i, trigger_hist.GetBinError(i) / args.bw)
    for j in xrange(25):
        modules_hist[j].SetBinContent(i, modules_hist[j].GetBinContent(i) / args.bw)
        modules_hist[j].SetBinError(i, modules_hist[j].GetBinError(i) / args.bw)
print ' - drawing ... '
y_max = 0
for i in xrange(25):
    if modules_hist[i].GetMaximum() > y_max:
        y_max = modules_hist[i].GetMaximum()
for i in xrange(25):
    modules_hist[i].SetMaximum(y_max * 1.2)

canvas_trigger = Canvas(1000, 800, name = "canvas_trigger", title = "rate of event trigger")
canvas_trigger.ToggleEventStatus()
canvas_trigger.cd()
trigger_hist.Draw('EH')
canvas_modules = Canvas(1500, 1000, name = "canvas_modules", title = "rate of 25 modules")
canvas_modules.ToggleEventStatus()
canvas_modules.Divide(5, 5)
for i in xrange(25):
    canvas_modules.cd(5 * (i % 5) + i / 5 + 1)
    modules_hist[i].Draw('EH')

sci_trigger_r_obj.close_file()
wait(True)
