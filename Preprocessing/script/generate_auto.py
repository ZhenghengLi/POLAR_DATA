#!/usr/bin/env python

from smtplib import SMTP_SSL
from email.MIMEMultipart import MIMEMultipart
from email.MIMEText import MIMEText
import argparse
import subprocess
import os
from datetime import datetime, timedelta

delimeter = " " + "-" * 80

def send_mail(tolist, subject, message):
    # server setting
    smtp_user = 'polardata@163.com'
    smtp_pass = 'polardata123456'
    smtp_host = 'smtp.163.com'
    smtp_port = 465
    fromaddr  = 'polardata@163.com'             
    # message
    msg = MIMEMultipart()
    msg['From']    = fromaddr
    msg['To']      = ', '.join(tolist)
    msg['Subject'] = subject
    msg.attach(MIMEText(message, 'plain'))
    # send
    server_ssl = SMTP_SSL(smtp_host, smtp_port)
    server_ssl.login(smtp_user, smtp_pass)
    server_ssl.sendmail(fromaddr, tolist, msg.as_string())
    server_ssl.quit()

parser = argparse.ArgumentParser(description='Generate 1M/1P data product automatically')
parser.add_argument('dirlist', metavar = 'filename', nargs = '+', help = 'list of filenames to open')
parser.add_argument("-p", dest = "pathprefix", default = '/hxmt/work/POLAR/PSDC/workspace/dataproduct/dailyprocess')
parser.add_argument('-r', dest = "dataprefix", default = '/hxmt/data/Mission/POLAR/data_in_orbit_test')
parser.add_argument('-d', dest = 'days', help = 'number of days of data to check', type = int, default = 2)
parser.add_argument('--sendmail', dest = 'sendmail', action='store_true', help = 'if send mail when new data processed')
args = parser.parse_args()

# check input
dirlist_all = ['normal', 'special', 'manual']
for x in args.dirlist:
    if x not in dirlist_all:
        print "illegal directory name."
        exit(1)
path_prefix = os.path.abspath(args.pathprefix)
if not os.path.isdir(path_prefix):
    print '"' + path_prefix + '" does not exist.'
    exit(1)
processlog_dir = os.path.join(path_prefix, 'processlog')
if not os.path.isdir(processlog_dir):
    print '"' + processlog_dir + '" does not exist.'
    exit(1)
mailmessage_dir = os.path.join(path_prefix, 'mailmessage')
if not os.path.isdir(mailmessage_dir):
    print '"' + mailmessage_dir + '" does not exist.'
    exit(1)
maillist_file = os.path.join(path_prefix, 'maillist.txt')
if not os.path.isfile(maillist_file):
    print '"' + maillist_file + '" does not exist.'
    exit(1)

# create directory
time_now = datetime.now()
nowdate = time_now.strftime('%Y%m%d')
processlog_dir_date = os.path.join(processlog_dir, nowdate)
if not os.path.isdir(processlog_dir_date): os.mkdir(processlog_dir_date)
mailmessage_dir_date = os.path.join(mailmessage_dir, nowdate)
if not os.path.isdir(mailmessage_dir_date): os.mkdir(mailmessage_dir_date)

print ' - path_prefix:     ' + path_prefix
print ' - processlog_dir:  ' + processlog_dir_date
print ' - mailmessage_dir: ' + mailmessage_dir_date
print ' - maillist_file:   ' + maillist_file
print delimeter

datelist = []
datelist.append(time_now.strftime('%Y%m%d'))
for x in xrange(1, args.days):
    datelist.append((time_now - timedelta(days = x)).strftime('%Y%m%d'))

message = {}
total_count = {}

for dirname in args.dirlist:
    cmd_1m = 'Generate_1M_date.py'
    cmd_1p = 'Generate_1P_update.py'
    if dirname in ['manual']:
        cmd_1m = 'Generate_1M_m_date.py'
        cmd_1p = 'Generate_1P_m_update.py'
    message[dirname] = ''
    total_count[dirname] = 0
    # 1M
    for onedate in datelist:
        cur_time = datetime.now().strftime('%Y%m%d%H%M%S')
        section = ' ***************** 1M * ' + dirname + '/' + onedate + ' * ' + cur_time + ' ****************** '
        print section + ' processing ... '
        message[dirname] += section + '\n'
        output_file = os.path.join(processlog_dir_date, dirname + '_' + onedate + '_1M_' + cur_time + '.log')
        ret_value = 0
        with open(output_file, 'w') as fout:
            ret_value = subprocess.call([cmd_1m, '--noconfirm', '-r', args.dataprefix, '-t', dirname, onedate], stdout = fout, stderr = fout)
        with open(output_file, 'r') as f: print f.read().rstrip('\n')
        if ret_value > 0:
            total_count[dirname] += ret_value
            with open(output_file, 'r') as f: message[dirname] += f.read()
        else:
            message[dirname] += ' - No new files found.\n'
    # 1P
    cur_time = datetime.now().strftime('%Y%m%d%H%M%S')
    section = ' ***************** 1P * ' + cur_time + ' ********************** '
    print section + ' processing ... '
    message[dirname] += section + '\n'
    output_file = os.path.join(processlog_dir_date, dirname + '_1P_' + cur_time + '.log')
    ret_value = 0
    with open(output_file, 'w') as fout:
        ret_value = subprocess.call([cmd_1p, '--noconfirm', '-r', args.dataprefix, '-t', dirname], stdout = fout, stderr = fout)
    with open(output_file, 'r') as f: print f.read().rstrip('\n')
    if ret_value > 0:
        with open(output_file, 'r') as f: message[dirname] += f.read()
    else:
        message[dirname] += ' - No new files found.\n'

# save and send message
message_all = ''
for key in args.dirlist:
    if total_count[key] > 0:
        message_all += ' ' + '%' * 40 + ' ' + key + ' ' + '%' * 40 + '\n'
        message_all += message[key]

if len(message_all) == 0:
    print " - No new data files found."
    exit(0)

cur_time = datetime.now().strftime('%Y%m%d%H%M%S')
message_file = os.path.join(mailmessage_dir_date, 'message' + '_' + cur_time + '.msg')
with open(message_file, 'w') as fout: fout.write(message_all)
if args.sendmail:
    maillist = [line.rstrip('\n') for line in open(maillist_file, 'r')]
    subject = "NEW_DATA_NOTIFICATION_" + cur_time
    send_mail(maillist, subject, message_all)
    print ' - mail sent to: '
    for x in maillist:
        print ' - ' + x

