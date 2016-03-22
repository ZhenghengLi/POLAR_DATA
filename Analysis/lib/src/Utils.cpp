#include "Utils.hpp"

double Utils::week_of_gps(const uint64_t raw_gps) {
    return static_cast<double>((raw_gps >> 32) & 0xFFFF);
}

double Utils::second_of_gps(const uint64_t raw_gps) {
    return static_cast<double>((raw_gps >> 12) & 0xFFFFF);
}

double Utils::millisecond_of_gps(const uint64_t raw_gps) {
    return static_cast<double>(raw_gps & 0xFFF) * 0.5;
}

double Utils::week_of_gps_clkcount(const uint64_t raw_gps_clkcount) {
    return static_cast<double>((raw_gps_clkcount >> 48) & 0xFFFF);
}

double Utils::second_of_gps_clkcount(const uint64_t raw_gps_clkcount) {
    return static_cast<double>((raw_gps_clkcount >> 28) & 0xFFFFF);
}

double Utils::millisecond_of_gps_clkcount(const uint64_t raw_gps_clkcount) {
    return static_cast<double>((raw_gps_clkcount >> 16) & 0xFFF) * 0.5;
}

double Utils::clkcount_of_gps_clkcount(const uint64_t raw_gps_clkcount) {
    return static_cast<double>(raw_gps_clkcount & 0xFFFF) * 25;
}
