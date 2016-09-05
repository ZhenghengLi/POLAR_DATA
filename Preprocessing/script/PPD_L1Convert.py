#!/usr/bin/env python

SW_NAME      = 'PPD_L1Convert.py'
SW_VERSION   = 'v1.0.0'
RELEASE_DATE = '2016 Sep  5'

import argparse
from os.path import basename
from rootpy import ROOT
from rootpy.io import File
from rootpy.tree import Tree
from datetime import datetime
from dateutil.tz import tzlocal
from tqdm import tqdm

parser = argparse.ArgumentParser(description='Convert platform parameters data to Level 1')
parser.add_argument("filename", help = "ROOT file that stores decoded platform parameters data")
parser.add_argument("-o", dest = "outfile", help = "ROOT file to store platform parameters data of Level 1", default = "TG2_PPD_file_L1.root")
args = parser.parse_args()

t_file_in      = File(args.filename, 'read')
t_tree_ppd_in  = t_file_in.get('t_ppd')
t_tree_ppd_in.create_buffer()
m_shipspan     = t_file_in.get('m_shipspan')
m_utc_span     = t_file_in.get('m_utc_span')

t_file_out     = File(args.outfile, 'recreate')
t_tree_ppd_out = Tree("t_ppd", "platform parameters data")
t_tree_ppd_out.create_branches({
    "longitude"            : "D"     ,
    "latitude"             : "D"     ,
    "geocentric_d"         : "D"     ,
    "ship_time_sec"        : "D"     ,
    "utc_time_sec"         : "D"     ,
    "utc_time_str"         : "C[32]" ,
    "flag_of_pos"          : "I"     ,
    "det_z_ra"             : "D"     ,
    "det_z_dec"            : "D"     ,
    "det_x_ra"             : "D"     ,
    "det_x_dec"            : "D"     ,
    "earth_ra"             : "D"     ,
    "earth_dec"            : "D"
})

print "Converting file: " + basename(args.filename) + " ..."
for i in tqdm(xrange(t_tree_ppd_in.get_entries())):
    t_tree_ppd_in.get_entry(i)
    t_tree_ppd_out.longitude       = t_tree_ppd_in.longitude
    t_tree_ppd_out.latitude        = t_tree_ppd_in.latitude
    t_tree_ppd_out.geocentric_d    = t_tree_ppd_in.geocentric_d
    t_tree_ppd_out.ship_time_sec   = t_tree_ppd_in.ship_time_sec
    t_tree_ppd_out.utc_time_sec    = t_tree_ppd_in.utc_time_sec
    t_tree_ppd_out.utc_time_str    = str(t_tree_ppd_in.utc_time_str)
    t_tree_ppd_out.flag_of_pos     = t_tree_ppd_in.flag_of_pos
    t_tree_ppd_out.det_z_ra        = t_tree_ppd_in.det_z_ra
    t_tree_ppd_out.det_z_dec       = t_tree_ppd_in.det_z_dec
    t_tree_ppd_out.det_x_ra        = t_tree_ppd_in.det_x_ra
    t_tree_ppd_out.det_x_dec       = t_tree_ppd_in.det_x_dec
    t_tree_ppd_out.earth_ra        = t_tree_ppd_in.earth_ra
    t_tree_ppd_out.earth_dec       = t_tree_ppd_in.earth_dec
    t_tree_ppd_out.fill()

t_file_in.close()

t_file_out.cd()
t_tree_ppd_out.write()
ROOT.TNamed('m_dattype',  'PLATFORM PARAMETERS DATA of LEVEL 1').Write()
ROOT.TNamed('m_version',  SW_NAME + " " + SW_VERSION).Write()
ROOT.TNamed('m_gentime',  datetime.now(tzlocal()).isoformat()).Write()
ROOT.TNamed('m_dcdfile',  basename(args.filename)).Write()
ROOT.TNamed('m_shipspan', m_shipspan.GetTitle()).Write()
ROOT.TNamed('m_utc_span', m_utc_span.GetTitle()).Write()

t_file_out.close()

print '====================================================================='
print 'shiptime span: { ' + m_shipspan.GetTitle() + ' }'
print 'UTC time span: { ' + m_utc_span.GetTitle() + ' }'
print '====================================================================='
