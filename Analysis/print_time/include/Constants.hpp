#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

using namespace std;

const string SW_NAME = "draw_rate";

const string SW_VERSION = "v1.0.0";

const string RELEASE_DATE = "2016 Sep  8";

const double GPSStartTime   = 1157984044.5455000401;
const int    GPSStartWeek   = 1914;
const double GPSStartSecond = 396844.54550004005;

const double UTCStartTime = 1157984027.3750000000;

const int LeapSecond = GPSStartTime - UTCStartTime;

#endif
