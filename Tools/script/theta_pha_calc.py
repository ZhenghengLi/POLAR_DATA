#!/usr/bin/env python

det_x_ra  = 3.6289
det_x_dec = 36.715
det_z_ra  = 18.070
det_z_dec = 47.103
sun_ra    = 12.145
sun_dec   = -0.9439

import numpy as np

def deg_to_rad(deg):
    return deg / 180.0 * np.pi

def rad_to_deg(rad):
    return rad / np.pi * 180.0

def radec_to_vec(ra, dec):
    theta = deg_to_rad(90 - dec)
    pha   = deg_to_rad(ra / 24 * 360)
    return (np.sin(theta) * np.cos(pha), np.sin(theta) * np.sin(pha), np.cos(theta))

def normalize(xyz):
    return np.array(xyz) / np.sqrt(sum([x ** 2 for x in xyz]))

vec_x = radec_to_vec(det_x_ra, det_x_dec)
vec_z = radec_to_vec(det_z_ra, det_z_dec)
vec_y = np.cross(vec_z, vec_x)
vec_s = normalize(radec_to_vec(sun_ra, sun_dec))

cos_theta = np.dot(vec_s, vec_z)
pha_x = np.dot(vec_s, vec_x)
pha_y = np.dot(vec_s, vec_y)

theta = rad_to_deg(np.arccos(cos_theta))
pha   = rad_to_deg(np.arctan2(pha_y, pha_x))

print "theta: " + str(theta)
print "pha:   " + str(pha)
