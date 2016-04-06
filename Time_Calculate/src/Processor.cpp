#include "Processor.hpp"

using namespace std;

Processor::Processor() {

}

Processor::~Processor() {

}

void Processor::initialize() {
    phy_error_count_ = 0;
    ped_error_count_ = 0;
}

bool Processor::logfile_open(const char* filename) {
    os_logfile_.open(filename);
    return os_logfile_.is_open();
}

void Processor::logfile_close() {
    os_logfile_.close();
}

void Processor::set_log(bool flag) {
    log_flag_ = flag;
}

bool Processor::can_log() {
    return log_flag_ && os_logfile_.is_open();
}

GPSTime Processor::calc_abs_time_(const pair<GPSTime, uint32_t>& gps_sync,
                                  const GPSTime&  trig_frm_gps,
                                  const uint32_t& trig_timestamp, 
                                  const double&   ticks_per_second) {
    GPSTime ret_gps_time = gps_sync.first;
    double gps_diff = trig_frm_gps - gps_sync.first;
    if (gps_diff > MAX_OFFSET) {
        ret_gps_time.week = -1;
        ret_gps_time.second = -1;
        return ret_gps_time;
    }
    double second_diff = (static_cast<double>(trig_timestamp)
                          - static_cast<double>(gps_sync.second)) / ticks_per_second;
    double diff_of_diff = gps_diff - second_diff;
    
    const double cycle = 4294967296 / ticks_per_second;
    if (diff_of_diff > MAX_DIFF) {
        if (abs(diff_of_diff - cycle) < MAX_DIFF) {
            ret_gps_time += second_diff + cycle;
            return ret_gps_time;
        } else {
            ret_gps_time.week = -1;
            ret_gps_time.second = -1;
            return ret_gps_time;
        }
    } else if (diff_of_diff < -1 * MAX_DIFF) {
        if (abs(diff_of_diff + cycle) < MAX_DIFF) {
            ret_gps_time += second_diff - cycle;
            return ret_gps_time;
        } else {
            ret_gps_time.week = -1;
            ret_gps_time.second = -1;
            return ret_gps_time;
        }
    } else {
        ret_gps_time += second_diff;
        return ret_gps_time;
    }
}

void Processor::copy_modules(SciTransfer& scitran) {

}

void Processor::copy_ped_modules(SciTransfer& scitran) {

}

void Processor::calc_time_trigger(SciTransfer& scitran) {

}

void Processor::calc_time_ped_trigger(SciTransfer& scitran) {

}

void Processor::write_meta_info(SciTransfer& scitran) {

}
