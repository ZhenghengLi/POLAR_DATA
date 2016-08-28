#!/usr/bin/env python

from time import strptime, strftime, mktime

class ppd_data:
    def __init__(self):
        self.pitch_angle    = 0.0
        self.yaw_angle      = 0.0
        self.roll_angle     = 0.0
        self.pitch_angle_v  = 0.0
        self.yaw_angle_v    = 0.0
        self.roll_angle_v   = 0.0
        self.orbit_agl_v    = 0.0
        self.longitude      = 0.0
        self.latitude       = 0.0
        self.geocentric_d   = 0.0
        self.ship_time_sec  = 0.0
        self.utc_time_sec   = 0.0
        self.utc_time_str   = ''
        self.flag_of_pos    = 0
        self.wgs84_x        = 0.0
        self.wgs84_y        = 0.0
        self.wgs84_z        = 0.0
        self.wgs84_x_v      = 0.0
        self.wgs84_y_v      = 0.0
        self.wgs84_z_v      = 0.0
        self.det_z_ra       = 0.0
        self.det_z_dec      = 0.0
        self.det_x_ra       = 0.0
        self.det_x_dec      = 0.0
        self.earth_ra       = 0.0
        self.earth_dec      = 0.0
        self.__utc_year     = 0
        self.__utc_month    = 0
        self.__utc_day      = 0
        self.__utc_hour     = 0
        self.__utc_minute   = 0   
        self.__utc_second   = 0


    def __lbtoi(self, block, begin, end):
        start_pos = 8
        return int(block[start_pos + begin : start_pos + end + 1][::-1].encode('hex'), 16)

    def __bbtoi(self, block, begin, end):
        start_pos = 8
        return int(block[start_pos + begin : start_pos + end + 1].encode('hex'), 16)

    def __calc_utc_time_sec(self):
        if self.__utc_year < 1 or self.__utc_month < 1 or self.__utc_day < 1: return -1
        t_fmt_str = "%Y-%m-%dT%H:%M:%S+0000"
        utc_gps_zero = "1980-01-06T00:00:00+0000"
        return mktime(strptime(self.utc_time_str, t_fmt_str)) - mktime(strptime(utc_gps_zero, t_fmt_str))

    def decode(self, block):
        tmp_value = 0
        tmp_value = self.__lbtoi(block, 0, 2)
        self.pitch_angle    = (tmp_value if tmp_value < 0x800000 else tmp_value - 2 * 0x800000) * 0.0001
        tmp_value = self.__lbtoi(block, 3, 5)
        self.yaw_angle      = (tmp_value if tmp_value < 0x800000 else tmp_value - 2 * 0x800000) * 0.0001
        tmp_value = self.__lbtoi(block, 6, 8)
        self.roll_angle     = (tmp_value if tmp_value < 0x800000 else tmp_value - 2 * 0x800000) * 0.0001
        tmp_value = self.__lbtoi(block, 9, 11)
        self.pitch_angle_v  = (tmp_value if tmp_value < 0x800000 else tmp_value - 2 * 0x800000) * 0.00002
        tmp_value = self.__lbtoi(block, 12, 14)
        self.yaw_angle_v    = (tmp_value if tmp_value < 0x800000 else tmp_value - 2 * 0x800000) * 0.00002
        tmp_value = self.__lbtoi(block, 15, 17)
        self.roll_angle_v   = (tmp_value if tmp_value < 0x800000 else tmp_value - 2 * 0x800000) * 0.00002
        tmp_value = self.__lbtoi(block, 18, 20)
        self.orbit_agl_v    = (tmp_value if tmp_value < 0x800000 else tmp_value - 2 * 0x800000) * 0.00000005
        tmp_value = self.__lbtoi(block, 21, 23)
        self.longitude      = (tmp_value if tmp_value < 0x800000 else tmp_value - 2 * 0x800000) * 0.00005
        tmp_value = self.__lbtoi(block, 24, 26)
        self.latitude       = (tmp_value if tmp_value < 0x800000 else tmp_value - 2 * 0x800000) * 0.00003
        tmp_value = self.__lbtoi(block, 27, 29)
        self.geocentric_d   = tmp_value * 10
        tmp_value = self.__lbtoi(block, 30, 33)
        self.ship_time_sec  = tmp_value * 1.0 / 32.0
        tmp_value = self.__bbtoi(block, 34, 35)
        self.__utc_year     = (tmp_value >> 4)
        self.__utc_month    = tmp_value & 0x0F
        tmp_value = self.__bbtoi(block, 36, 38)
        self.__utc_day      = (tmp_value >> 17)
        self.__utc_hour     = (tmp_value >> 12) & 0x1F
        self.__utc_minute   = (tmp_value >> 6) & 0x3F
        self.__utc_second   = tmp_value & 0x3F
        self.utc_time_str   = "%04d-%02d-%02dT%02d:%02d:%02d+0000" % (self.__utc_year, 
                self.__utc_month, self.__utc_day, self.__utc_hour, self.__utc_minute, self.__utc_second)
        self.utc_time_sec   = self.__calc_utc_time_sec()
        tmp_value = self.__bbtoi(block, 39, 39)
        self.flag_of_pos    = tmp_value
        tmp_value = self.__bbtoi(block, 40, 43)
        self.wgs84_x        = (tmp_value if tmp_value < 0x80000000 else tmp_value - 2 * 0x80000000) * 0.1
        tmp_value = self.__bbtoi(block, 44, 47)
        self.wgs84_y        = (tmp_value if tmp_value < 0x80000000 else tmp_value - 2 * 0x80000000) * 0.1
        tmp_value = self.__bbtoi(block, 48, 51)
        self.wgs84_z        = (tmp_value if tmp_value < 0x80000000 else tmp_value - 2 * 0x80000000) * 0.1
        tmp_value = self.__bbtoi(block, 52, 55)
        self.wgs84_x_v      = (tmp_value if tmp_value < 0x80000000 else tmp_value - 2 * 0x80000000) * 0.01
        tmp_value = self.__bbtoi(block, 56, 59)
        self.wgs84_y_v      = (tmp_value if tmp_value < 0x80000000 else tmp_value - 2 * 0x80000000) * 0.01
        tmp_value = self.__bbtoi(block, 60, 63)
        self.wgs84_z_v      = (tmp_value if tmp_value < 0x80000000 else tmp_value - 2 * 0x80000000) * 0.01

    def calc_j2000(self):
        print "test"
