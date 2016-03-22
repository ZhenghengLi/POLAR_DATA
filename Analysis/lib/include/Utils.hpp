#ifndef UTILS
#define UTILS

#include <stdint.h>

class Utils {
public:
    static double week_of_gps(const uint64_t raw_gps);
    static double second_of_gps(const uint64_t raw_gps);
    static double millisecond_of_gps(const uint64_t raw_gps);

    static double week_of_gps_clkcount(const uint64_t raw_gps_clkcount);
    static double second_of_gps_clkcount(const uint64_t raw_gps_clkcount);
    static double millisecond_of_gps_clkcount(const uint64_t raw_gps_clkcount);
    static double clkcount_of_gps_clkcount(const uint64_t raw_gps_clkcount);
};

#endif
