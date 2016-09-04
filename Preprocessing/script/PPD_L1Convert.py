#!/usr/bin/env python

import argparse
from rootpy import ROOT
from rootpy.io import File
from rootpy.tree import Tree

parser = argparse.ArgumentParser(description='Convert platform parameters data to Level 1')
parser.add_argument("filename", help = "ROOT file that stores decoded platform parameters data")
parser.add_argument("-o", dest = "outfile", help = "ROOT file to store platform parameters data of Level 1", default = "TG2_PPD_file_L1.root")
args = parser.parse_args()

t_file_in     = File(args.filename, 'read')
t_tree_ppd_in = t_file_in.get('t_ppd')

