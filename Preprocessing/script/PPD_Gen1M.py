#!/usr/bin/python

import argparse
import csv
from rootpy.io import File
from rootpy.tree import Tree

parser = argparse.ArgumentParser(description='Convert platform parameters data from CSV file to ROOT file')
parser.add_argument("filename", help = "CSV file of platform parameters data")
parser.add_argument("-o", dest = "outfile", help = "ROOT file to store platform parameters data", default = "TG2_PPD_file.root")
args = parser.parse_args()

print args.filename
print args.outfile
