#!/usr/bin/env python

import argparse
import numpy as np

parser = argparse.ArgumentParser(description='calculate theta and pha of GRB')
parser.add_argument('grb_radec', help = '"RA(deg),DEC(deg)" of GRB')
parser.add_argument('-Z', dest = 'z_radec', required=True, help = '"RA(h),DEC(deg) of POLAR Z axis"')
parser.add_argument('-X', dest = 'x_radec', required=True, help = '"RA(h),DEC(deg) of POLAR X axis"')
args = parser.parse_args()

grb_radec_vec = [float(x) for x in args.grb_radec.split(',')]
z_radec_vec   = [float(x) for x in args.z_radec.split(',')]
x_radec_vec   = [float(x) for x in args.x_radec.split(',')]

if len(grb_radec_vec) != 2 or len(z_radec_vec) != 2 or len(x_radec_vec) != 2:
    print "Wrong RA/DEC input"
    exit(1)

det_x_ra, det_x_dec = x_radec_vec
det_z_ra, det_z_dec = z_radec_vec
grb_ra    = grb_radec_vec[0] / 360.0 * 24.0
grb_dec   = grb_radec_vec[1]

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
vec_s = normalize(radec_to_vec(grb_ra, grb_dec))

cos_theta = np.dot(vec_s, vec_z)
pha_x = np.dot(vec_s, vec_x)
pha_y = np.dot(vec_s, vec_y)

theta = rad_to_deg(np.arccos(cos_theta))
pha   = rad_to_deg(np.arctan2(pha_y, pha_x))

print "theta: " + str(theta)
print "pha:   " + str(pha)

