#!/usr/bin/env python

import argparse

parser = argparse.ArgumentParser(description='Split platfrom parameters data by UTC time')
parser.add_argument('filelist', metavar = 'filename', nargs = '+', help = 'list of filenames to open')
parser.add_argument('-B', dest = 'begin', help = 'utc time of beginning as gps time form, like utc_week:utc_second', default = 'begin')
parser.add_argument('-E', dest = 'end'  , help = 'utc time of ending as gps time form, like utc_week:utc_second', default = 'end')
parser.add_argument('-o', dest = 'outfile', help = 'ROOT file to stored splitted platform parameters data', default = 'TG2_PPD_file_split.root')
args = parser.parse_args()

print args.filelist
print args.begin
print args.end
print args.outfile
