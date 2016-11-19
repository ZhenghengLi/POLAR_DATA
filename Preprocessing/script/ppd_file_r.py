#!/usr/bin/env python

import re
from os.path import basename
from rootpy import ROOT
from rootpy.io import File
from rootpy.tree import Tree

_MIN_DIFF = 1

class ppd_file_r:
    def __init__(self):
        self.t_file_name           = ''
        self.t_file_in             = None
        self.t_tree_ppd            = None
        self.begin_entry           = 0
        self.end_entry             = 0
        self.utc_time_span         = ''
        self.first_utc_time_sec    = 0.0
        self.last_utc_time_sec     = 0.0
        self.begin_utc_time_sec    = 0.0
        self.end_utc_time_sec      = 0.0

    def __find_entry(self, utc_time_sec):
        head_entry = -1
        head_entry_found = False
        while head_entry < self.t_tree_ppd.get_entries():
            head_entry += 1
            self.t_tree_ppd.get_entry(head_entry)
            if self.t_tree_ppd.flag_of_pos == 0x55 and self.t_tree_ppd.utc_time_sec > 0:
                head_entry_found = True
                break
        if not head_entry_found:
            return -1
        if utc_time_sec < self.t_tree_ppd.utc_time_sec:
            return -1
        tail_entry = self.t_tree_ppd.get_entries()
        tail_entry_found = False
        while tail_entry >= 0:
            tail_entry -= 1
            self.t_tree_ppd.get_entry(tail_entry)
            if self.t_tree_ppd.flag_of_pos == 0x55 and self.t_tree_ppd.utc_time_sec > 0:
                tail_entry_found = True
                break
        if not tail_entry_found:
            return -1
        if utc_time_sec > self.t_tree_ppd.utc_time_sec:
            return -1
        while tail_entry - head_entry > 1:
            center_entry = int((head_entry + tail_entry) / 2)
            found_valid_center = False
            self.t_tree_ppd.get_entry(center_entry)
            if self.t_tree_ppd.flag_of_pos == 0x55 and self.t_tree_ppd.utc_time_sec > 0:
                found_valid_center = True
            tmp_center_entry = center_entry
            while not found_valid_center and tail_entry - tmp_center_entry > 1:
                tmp_center_entry += 1
                self.t_tree_ppd.get_entry(tmp_center_entry)
                if self.t_tree_ppd.flag_of_pos == 0x55 and self.t_tree_ppd.utc_time_sec > 0:
                    found_valid_center = True
            if not found_valid_center: tmp_center_entry = center_entry
            while not found_valid_center and tmp_center_entry - head_entry > 1:
                tmp_center_entry -= 1
                self.t_tree_ppd.get_entry(tmp_center_entry)
                if self.t_tree_ppd.flag_of_pos == 0x55 and self.t_tree_ppd.utc_time_sec > 0:
                    found_valid_center = True
            if not found_valid_center: break
            if utc_time_sec == self.t_tree_ppd.utc_time_sec:
                return tmp_center_entry
            elif utc_time_sec > self.t_tree_ppd.utc_time_sec:
                head_entry = tmp_center_entry
            else:
                tail_entry = tmp_center_entry
        return tail_entry

    def open_file(self, filename, begin, end):   # cut data by utc time, utc_week:utc_second
        self.t_file_name = basename(filename)
        self.t_file_in = File(filename, 'read')
        self.t_tree_ppd = self.t_file_in.get('t_ppd')
        self.t_tree_ppd.create_buffer()
        self.utc_time_span = self.t_file_in.get('m_utc_span').GetTitle()
        m = re.compile(r'(\d+):(\d+)\[\d+\] => (\d+):(\d+)\[\d+\]; \d+/\d+').match(self.utc_time_span)
        self.first_utc_time_sec = float(m.group(1)) * 604800 + float(m.group(2))
        self.last_utc_time_sec  = float(m.group(3)) * 604800 + float(m.group(4))
        if begin != 'begin':
            m = re.compile(r'(\d+):(\d+)').match(begin)
            self.begin_utc_time_sec = float(m.group(1)) * 604800 + float(m.group(2))
            if self.begin_utc_time_sec - self.first_utc_time_sec < _MIN_DIFF:
                print 'WARNING: begin utc time is out of range: ' + str(self.begin_utc_time_sec - self.first_utc_time_sec )
                return False
        else:
            self.begin_utc_time_sec = -1
        if end != 'end':
            m = re.compile(r'(\d+):(\d+)').match(end)
            self.end_utc_time_sec = float(m.group(1)) * 604800 + float(m.group(2))
            if self.last_utc_time_sec - self.end_utc_time_sec < _MIN_DIFF:
                print 'WARNING: end utc time is out of range: ' + str(self.last_utc_time_sec - self.end_utc_time_sec)
                return False
        else:
            self.end_utc_time_sec = -1
        if self.begin_utc_time_sec > 0 and self.end_utc_time_sec > 0 and self.end_utc_time_sec - self.begin_utc_time_sec < _MIN_DIFF * 5:
            print 'WARNING: time span between begin and end utc time is too small: ' + str(self.end_utc_time_sec - self.begin_utc_time_sec)
            return False
        if self.begin_utc_time_sec > 0:
            self.begin_entry = self.__find_entry(self.begin_utc_time_sec)
            if self.begin_entry < 0:
                print "WARNING: cannot find begin entry."
                return False
        else:
            self.begin_entry = 0
        if self.end_utc_time_sec > 0:
            self.end_entry = self.__find_entry(self.end_utc_time_sec)
            if self.end_entry < 0:
                print "WARNING: cannot find end entry."
                return False
        else:
            self.end_entry = self.t_tree_ppd.get_entries()
        return True

    def print_file_info(self):
        actual_start_entry = 0
        for idx in xrange(self.begin_entry, self.end_entry):
            actual_start_entry = idx
            self.t_tree_ppd.get_entry(idx)
            if self.t_tree_ppd.flag_of_pos == 0x55 and self.t_tree_ppd.utc_time_sec > 0: break
        actual_begin_utc_time_sec = self.t_tree_ppd.utc_time_sec
        actual_end_entry = 0
        for idx in xrange(self.end_entry - 1, self.begin_entry - 1, -1):
            actual_end_entry = idx
            self.t_tree_ppd.get_entry(idx)
            if self.t_tree_ppd.flag_of_pos == 0x55 and self.t_tree_ppd.utc_time_sec > 0: break
        actual_end_utc_time_sec   = self.t_tree_ppd.utc_time_sec
        utc_time_span_str = '%d:%d[%d] => %d:%d[%d]' % (int(actual_begin_utc_time_sec / 604800),
                                                        int(actual_begin_utc_time_sec % 604800),
                                                        actual_start_entry,
                                                        int(actual_end_utc_time_sec / 604800),
                                                        int(actual_end_utc_time_sec % 604800),
                                                        actual_end_entry)
        print self.t_file_name
        print ' - UTC time span: { ' + utc_time_span_str + ' }'

    def close_file(self):
        self.t_file_in.close()
        self.t_file_in = None
        self.t_tree_ppd  = None

