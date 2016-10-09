#!/usr/bin/env python

import re
from datetime import datetime, timedelta

leapsecond = 17
ref_weeksecond = re.compile(r'^(\d+):(\d+\.?\d*)$');

def gps_to_beijing(week_second):
    week = 0
    second = 0
    m = ref_weeksecond.match(week_second)
    if m:
        week = int(m.group(1))
        second = float(m.group(2))
    else:
        return 'bad_gps_time_format'
    total_second = week * 604800 + second
    utc_datetime = datetime(1980, 1, 6, 0, 0, 0) + timedelta(seconds = total_second - leapsecond);
    beijing_datetime = utc_datetime + timedelta(seconds = 28800);
    beijing_time_str = beijing_datetime.isoformat()
    return beijing_time_str

def gps_to_utc(week_second):
    week = 0
    second = 0
    m = ref_weeksecond.match(week_second)
    if m:
        week = int(m.group(1))
        second = float(m.group(2))
    else:
        return 'bad_gps_time_format'
    total_second = week * 604800 + second
    utc_datetime = datetime(1980, 1, 6, 0, 0, 0) + timedelta(seconds = total_second - leapsecond);
    utc_time_str = utc_datetime.isoformat()
    return utc_time_str
