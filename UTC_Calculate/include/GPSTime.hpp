#ifndef GPSTIME_H
#define GPSTIME_H

#include <stdint.h>
#include <cmath>

class GPSTime {
public:
    double week;
    double second;

public:
    void update6(const uint64_t raw_gps);
    void update8(const uint64_t raw_gps_clkcount);

    double operator-(const GPSTime& right) const;
    GPSTime operator+(const double right_second) const;
    void operator+=(const double right_second);
    bool operator<(const GPSTime& right) const;
    bool operator<=(const GPSTime& right) const;
    bool operator>(const GPSTime& right) const;
    bool operator>=(const GPSTime& right) const;
    bool operator==(const GPSTime& right) const;
};

#endif
