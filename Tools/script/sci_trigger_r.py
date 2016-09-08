#!/usr/bin/env python

import re
from os.path import basename
from rootpy import ROOT
from rootpy.io import File
from rootpy.tree import Tree

_MIN_DIFF = 5

class sci_trigger_r:
    def __init__(self):
        self.t_file_name          = ''
        self.t_file_in            = None
        self.t_trigger            = None
        self.m_phy_gps            = ''
        self.begin_entry          = 0
        self.end_entry            = 0
        self.first_gps_time_sec   = 0.0
        self.last_gps_time_sec    = 0.0
        self.begin_gps_time_sec   = 0.0
        self.end_gps_time_sec     = 0.0
        self.start_week           = 0
        self.start_second         = 0.0
        self.stop_week            = 0
        self.stop_second          = 0.0
        self.gps_time_length      = 0.0

    def __find_entry(self, gps_time_sec):
        head_entry = 0 
        tail_entry = self.t_trigger.get_entries() - 1 
        while tail_entry - head_entry > 1:
            center_entry = int((head_entry + tail_entry) / 2)
            found_valid_center = False
            self.t_trigger.get_entry(center_entry)
            if self.t_trigger.abs_gps_valid:
                found_valid_center = True
            tmp_center_entry = center_entry
            while not found_valid_center and tail_entry - tmp_center_entry > 1:
                tmp_center_entry += 1
                self.t_trigger.get_entry(tmp_center_entry)
                if self.t_trigger.abs_gps_valid:
                    found_valid_center = True
            if not found_valid_center: tmp_center_entry = center_entry
            while not found_valid_center and tmp_center_entry - head_entry > 1:
                tmp_center_entry -= 1
                self.t_trigger.get_entry(tmp_center_entry)
                if self.t_trigger.abs_gps_valid:
                    found_valid_center = True
            if not found_valid_center: break
            if gps_time_sec == self.t_trigger.abs_gps_week * 604800 + self.t_trigger.abs_gps_second:
                return tmp_center_entry
            elif gps_time_sec > self.t_trigger.abs_gps_week * 604800 + self.t_trigger.abs_gps_second:
                head_entry = tmp_center_entry
            else:
                tail_entry = tmp_center_entry
        return head_entry

    def open_file(self, filename, begin, end):
        self.t_file_name = basename(filename)
        self.t_file_in = File(filename, 'read')
        self.t_trigger = self.t_file_in.get('t_trigger')
        self.t_trigger.activate(['abs_gps_week', 'abs_gps_second', 'abs_gps_valid', 'trig_accepted'], True)
        self.t_trigger.create_buffer()
        self.m_phy_gps = self.t_file_in.get('m_phy_gps').GetTitle()
        m = re.compile(r'(\d+):(\d+)\[\d+\] => (\d+):(\d+)\[\d+\]; \d+/\d+').match(self.m_phy_gps)
        self.first_gps_time_sec = float(m.group(1)) * 604800 + float(m.group(2))
        self.last_gps_time_sec  = float(m.group(3)) * 604800 + float(m.group(4))
        if begin != 'begin':
            m = re.compile(r'(\d+):(\d+)').match(begin)
            self.begin_gps_time_sec = float(m.group(1)) * 604800 + float(m.group(2))
            if self.begin_gps_time_sec - self.first_gps_time_sec < _MIN_DIFF:
                print 'WARNING: begin gps time is out of range: ' + str(self.begin_gps_time_sec - self.first_gps_time_sec )
                return False
        else:
            self.begin_gps_time_sec = -1
        if end != 'end':
            m = re.compile(r'(\d+):(\d+)').match(end)
            self.end_gps_time_sec = float(m.group(1)) * 604800 + float(m.group(2))
            if self.last_gps_time_sec - self.end_gps_time_sec < _MIN_DIFF:
                print 'WARNING: end gps time is out of range: ' + str(self.last_gps_time_sec - self.end_gps_time_sec)
                return False
        else:
            self.end_gps_time_sec = -1
        if self.begin_gps_time_sec > 0 and self.end_gps_time_sec > 0 and self.end_gps_time_sec - self.begin_gps_time_sec < _MIN_DIFF:
            print 'WARNING: time span between begin and end gps time is too small: ' + str(self.end_gps_time_sec - self.begin_gps_time_sec)
            return False
        if self.begin_gps_time_sec > 0:
            self.begin_entry = self.__find_entry(self.begin_gps_time_sec)
        else:
            self.begin_entry = 0
        if self.end_gps_time_sec > 0:
            self.end_entry = self.__find_entry(self.end_gps_time_sec)
        else:
            self.end_entry = self.t_trigger.get_entries()
        for idx in xrange(self.begin_entry, self.end_entry):
            self.t_trigger.get_entry(idx)
            if self.t_trigger.abs_gps_valid:
                self.start_week   = self.t_trigger.abs_gps_week
                self.start_second = self.t_trigger.abs_gps_second
                break
        for idx in xrange(self.end_entry - 1, self.begin_entry - 1, -1):
            self.t_trigger.get_entry(idx)
            if self.t_trigger.abs_gps_valid:
                self.stop_week   = self.t_trigger.abs_gps_week
                self.stop_second = self.t_trigger.abs_gps_second
                break
        self.gps_time_length = (self.stop_week - self.start_week) * 604800 + (self.stop_second - self.start_second)
        return True

    def print_file_info(self):
        actual_start_entry = 0
        for idx in xrange(self.begin_entry, self.end_entry):
            actual_start_entry = idx
            self.t_trigger.get_entry(idx)
            if self.t_trigger.abs_gps_valid: break
        actual_begin_gps_week   = self.t_trigger.abs_gps_week
        actual_begin_gps_second = self.t_trigger.abs_gps_second
        actual_end_entry = 0
        for idx in xrange(self.end_entry -1, self.begin_entry - 1, -1):
            actual_end_entry = idx
            self.t_trigger.get_entry(idx)
            if self.t_trigger.abs_gps_valid: break
        actual_end_gps_week     = self.t_trigger.abs_gps_week
        actual_end_gps_second   = self.t_trigger.abs_gps_second
        gps_time_span_str = '%d:%d[%d] => %d:%d[%d]' % (int(actual_begin_gps_week),
                                                        int(actual_begin_gps_second),
                                                        actual_start_entry,
                                                        int(actual_end_gps_week),
                                                        int(actual_end_gps_second),
                                                        actual_end_entry)
        print self.t_file_name
        print ' - GPS time span: { ' + gps_time_span_str + ' }'

    def close_file(self):
        self.t_file_in.close()
        self.t_file_in = None
        self.t_trigger  = None

