#include "GPSTime.hpp"

GPSTime& GPSTime::update6(const uint64_t raw_gps) {
    week = static_cast<double>((raw_gps >> 32) & 0xFFFF);
    second = static_cast<double>((raw_gps >> 12) & 0xFFFFF)
        + static_cast<double>(raw_gps & 0xFFF) * 0.5 * 1.0E-3;
    return *this;
}

GPSTime& GPSTime::update8(const uint64_t raw_gps_clkcount) {
    week = static_cast<double>((raw_gps_clkcount >> 48) & 0xFFFF);
    second = static_cast<double>((raw_gps_clkcount >> 28) & 0xFFFFF)
        + static_cast<double>((raw_gps_clkcount >> 16) & 0xFFF) * 0.5 * 1.0E-3;
    double clkcount = static_cast<double>(raw_gps_clkcount & 0xFFFF);
    if (clkcount < 32768) 
        second += clkcount * 25 * 1.0E-9;
    return *this;
}

double GPSTime::operator-(const GPSTime& right) const {
    return ((week - right.week) * 604800 + (second - right.second));
}

GPSTime GPSTime::operator+(const double right_second) const {
    double week_r = floor(right_second / 604800);
    double second_r = right_second - week_r * 604800;
    GPSTime gpsTime = *this;
    gpsTime.week += week_r;
    gpsTime.second += second_r;
    if (gpsTime.second >= 604800) {
        gpsTime.week += 1;
        gpsTime.second -= 604800;
    } else if (gpsTime.second < 0) {
        gpsTime.week -= 1;
        gpsTime.second += 604800;
    }
    return gpsTime;
}

void GPSTime::operator+=(const double right_second) {
    double week_r = floor(right_second / 604800);
    double second_r = right_second - week_r * 604800;
    week += week_r;
    second += second_r;
    if (second >= 604800) {
        week += 1;
        second -= 604800;
    } else if (second < 0) {
        week -= 1;
        second += 604800;
    }
}

bool GPSTime::operator<(const GPSTime& right) const {
    if (week > right.week)
        return false;
    else if (week < right.week)
        return true;
    else
        return (second < right.second);
}

bool GPSTime::operator<=(const GPSTime& right) const {
    if (week > right.week)
        return false;
    else if (week < right.week)
        return true;
    else
        return (second <= right.second);
}

bool GPSTime::operator>(const GPSTime& right) const {
    if (week > right.week)
        return true;
    else if (week < right.week)
        return false;
    else
        return (second > right.second);
}

bool GPSTime::operator>=(const GPSTime& right) const {
    if (week > right.week)
        return true;
    else if (week < right.week)
        return false;
    else
        return (second >= right.second);
}

bool GPSTime::operator==(const GPSTime& right) const {
    return (week == right.week && second == right.second);
}
