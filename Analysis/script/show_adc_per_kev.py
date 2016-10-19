#!/usr/bin/env python

import sys
from rootpy import ROOT
from rootpy.io import File
from rootpy.tree import Tree
from rootpy.plotting import Hist2D
from rootpy.interactive import wait
from cooconv import ijtox, ijtoy

if len(sys.argv) < 2:
    print "USAGE: show_adc_per_kev.py <adc_per_kev.root>"
    exit(1)

adc_per_kev_fn = sys.argv[1]
adc_per_kev_file = File(adc_per_kev_fn, 'read')
adc_per_kev = [None for i in xrange(25)]
adc_sigma   = [None for i in xrange(25)]
for i in xrange(25):
    adc_per_kev[i] = adc_per_kev_file.get("adc_per_kev_vec_ct_%02d" % (i + 1))
    adc_sigma[i]   = adc_per_kev_file.get("adc_sigma_vec_ct_%02d" % (i + 1))

adc_per_kev_file.close()

hist2d_adc_per_kev = Hist2D(40, 0, 40, 40, 0, 40)
hist2d_adc_per_kev.SetDirectory(None)
hist2d_adc_per_kev.SetName("hist2d_adc_per_kev")
hist2d_adc_per_kev.SetTitle("ADC/KeV of 1600 Channels")
hist2d_adc_per_kev.GetXaxis().SetNdivisions(40)
hist2d_adc_per_kev.GetYaxis().SetNdivisions(40)
for i in xrange(40):
    if (i % 8 == 0):
        hist2d_adc_per_kev.GetXaxis().SetBinLabel(i + 1, "%02d" % i);
        hist2d_adc_per_kev.GetYaxis().SetBinLabel(i + 1, "%02d" % i);

hist2d_adc_sigma = Hist2D(40, 0, 40, 40, 0, 40)
hist2d_adc_sigma.SetDirectory(None)
hist2d_adc_sigma.SetName("hist2d_adc_sigma")
hist2d_adc_sigma.SetTitle("ADC Sigma of 1600 Channels")
hist2d_adc_sigma.GetXaxis().SetNdivisions(40)
hist2d_adc_sigma.GetYaxis().SetNdivisions(40)
for i in xrange(40):
    if (i % 8 == 0):
        hist2d_adc_sigma.GetXaxis().SetBinLabel(i + 1, "%02d" % i);
        hist2d_adc_sigma.GetYaxis().SetBinLabel(i + 1, "%02d" % i);

ROOT.gStyle.SetOptStat(0)

for i in xrange(25):
    for j in xrange(64):
        hist2d_adc_per_kev.SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, adc_per_kev[i][j])
        hist2d_adc_sigma.SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, adc_sigma[i][j])

c1 = ROOT.TCanvas()
c1.SetWindowSize(1600, 800)
c1.Divide(2, 1)
c1.cd(1)
c1.GetPad(1).SetGrid()
hist2d_adc_per_kev.Draw("COLZ")
c1.cd(2)
c1.GetPad(2).SetGrid()
hist2d_adc_sigma.Draw("COLZ")

wait(True)
