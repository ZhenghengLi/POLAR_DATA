#!/usr/bin/env python

from rootpy import ROOT
from rootpy.io import File
from rootpy.tree import Tree

class ppd_file_r:
    def __init__(self):
        self.__t_file_in  = None
        self.__t_tree_ppd = None
        self.__begin_entry = 0
        self.__end_entry   = 0

    def open_file(self, filename, begin, end):   # cut data by utc time, utc_week:utc_second
        print "open"
