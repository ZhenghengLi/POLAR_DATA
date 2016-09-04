#!/usr/bin/env python

from time import strptime, strftime, mktime
import numpy as np
import ephem as ep

_det_angle_gnc = 11.0 / 180.0 * np.pi
_det_z_gnc = [0, -np.sin(_det_angle_gnc), -np.cos(_det_angle_gnc)]
_det_x_gnc = [0, -np.cos(_det_angle_gnc),  np.sin(_det_angle_gnc)]

def _gnc_to_orbit_mat(pitch_yaw_roll):
    alpha = pitch_yaw_roll[1] / 180.0 * np.pi
    beta  = pitch_yaw_roll[2] / 180.0 * np.pi
    gamma = pitch_yaw_roll[0] / 180.0 * np.pi
    s1, s2, s3 = np.sin(alpha), np.sin(beta), np.sin(gamma)
    c1, c2, c3 = np.cos(alpha), np.cos(beta), np.cos(gamma)
    return [[c1 * c3 - s1 * s2 * s3, -c2 * s1, c1 * s3 + c3 * s1 * s2],
            [c3 * s1 + c1 * s2 * s3,  c1 * c2, s1 * s3 - c1 * c3 * s2],
            [-c2 * s3              ,  s2     , c2 * c3               ]]

def _xyz_to_latlon(x, y, z):
    r = np.sqrt(x**2 + y**2 + z**2)
    return [90 - np.arccos(z / r) / np.pi * 180, np.arctan2(y, x) / np.pi * 180]

def _latlon_to_xyz(lat, lon):
    theta = (90 - lat) / 180 * np.pi
    pha = (lon if lon >= 0 else lon + 360) / 180 * np.pi
    return [np.sin(theta) * np.cos(pha), np.sin(theta) * np.sin(pha), np.cos(theta)]

def _normalize(xyz):
    return np.array(xyz) / np.sqrt(sum([x ** 2 for x in xyz]))

def _orbit_to_wgs84_mat(wgs84_xyz, wgs84_xyz_v):
    ko = _normalize(np.array([-x for x in wgs84_xyz]))
    jo = _normalize(np.cross(ko, wgs84_xyz_v))
    io = _normalize(np.cross(jo, ko))
    iw, jw, kw = [1, 0, 0], [0, 1, 0], [0, 0, 1]
    return [[np.dot(iw, io), np.dot(iw, jo), np.dot(iw, ko)],
            [np.dot(jw, io), np.dot(jw, jo), np.dot(jw, ko)],
            [np.dot(kw, io), np.dot(kw, jo), np.dot(kw, ko)]]

def _wgs84_to_j2000(wgs84_xyz, utc):
    lat, lon = _xyz_to_latlon(*wgs84_xyz)
    location = ep.Observer()
    location.lat  = lat / 180 * np.pi
    location.lon  = lon / 180 * np.pi
    location.date = utc
    ra_now  = location.sidereal_time()
    dec_now = lat / 180 * np.pi
    pos_now   = ep.Equatorial(ra_now, dec_now, epoch = utc)
    pos_j2000 = ep.Equatorial(pos_now, epoch = ep.J2000)
    return [float(pos_j2000.ra) / np.pi * 12, float(pos_j2000.dec) / np.pi * 180]

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
        self.det_z_lat      = 0.0
        self.det_z_lon      = 0.0
        self.det_z_ra       = 0.0
        self.det_z_dec      = 0.0
        self.det_x_lat      = 0.0
        self.det_x_lon      = 0.0
        self.det_x_ra       = 0.0
        self.det_x_dec      = 0.0
        self.earth_lat      = 0.0
        self.earth_lon      = 0.0
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
        if self.__utc_year < 1 or self.__utc_year > 2030 or\
           self.__utc_month < 1 or self.__utc_month > 12 or\
           self.__utc_day < 1 or self.__utc_day > 31 or\
           self.__utc_hour > 23 or self.__utc_minute > 59 or self.__utc_second > 59:
            return -1
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
        if self.flag_of_pos != 0x55:
            self.earth_lat, self.earth_lon    = [-1, -1]
            self.earth_ra, self.earth_dec     = [-1, -1]
            self.det_z_lat, self.det_z_lon    = [-1, -1]
            self.det_z_ra, self.det_z_dec     = [-1, -1]
            self.det_x_lat, self.det_x_lon    = [-1, -1]
            self.det_x_ra, self.det_x_dec     = [-1, -1]
            return
        utc = "%04d/%02d/%02d %02d:%02d:%02d" % (self.__utc_year, self.__utc_month, self.__utc_day,
                                                 self.__utc_hour, self.__utc_minute, self.__utc_second)
        pitch_yaw_roll = [self.pitch_angle, self.yaw_angle, self.roll_angle]
        wgs84_xyz   = [self.wgs84_x, self.wgs84_y, self.wgs84_z]
        wgs84_xyz_v = [self.wgs84_x_v, self.wgs84_y_v, self.wgs84_z_v]
        wgs84_earth = -1 * np.array(wgs84_xyz)
        gto_mat = _gnc_to_orbit_mat(pitch_yaw_roll)
        otw_mat = _orbit_to_wgs84_mat(wgs84_xyz, wgs84_xyz_v)
        wgs84_det_z = np.dot(otw_mat, np.dot(gto_mat, _det_z_gnc))
        wgs84_det_x = np.dot(otw_mat, np.dot(gto_mat, _det_x_gnc))
        self.earth_lat, self.earth_lon = _xyz_to_latlon(*wgs84_earth)
        self.earth_ra, self.earth_dec = _wgs84_to_j2000(wgs84_earth, utc)
        self.det_z_lat, self.det_z_lon = _xyz_to_latlon(*wgs84_det_z)
        self.det_z_ra, self.det_z_dec = _wgs84_to_j2000(wgs84_det_z, utc)
        self.det_x_lat, self.det_x_lon = _xyz_to_latlon(*wgs84_det_x)
        self.det_x_ra, self.det_x_dec = _wgs84_to_j2000(wgs84_det_x, utc)
        
