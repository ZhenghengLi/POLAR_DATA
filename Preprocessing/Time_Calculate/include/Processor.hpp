#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <stdint.h>
#include "RootInc.hpp"
#include "Constants.hpp"
#include "GPSTime.hpp"
#include "HkGPSIterator.hpp"
#include "SciTransfer.hpp"
#include "RootInc.hpp"

#define MAX_DIFF 5
#define MAX_OFFSET 300
#define MAX_ABS_DIFF 1

using namespace std;

class Processor {
private:
    bool log_flag_;
    ofstream os_logfile_;

    int64_t phy_error_count_;
    int64_t ped_error_count_;

    bool     phy_first_valid_found_;
    Long64_t phy_first_valid_index_;
    Long64_t phy_first_valid_week_;
    Long64_t phy_first_valid_second_;
    Long64_t phy_first_ship_second_;
    Long64_t phy_last_valid_index_;
    Long64_t phy_last_valid_week_;
    Long64_t phy_last_valid_second_;
    Long64_t phy_last_ship_second_;
    Long64_t phy_total_valid_count_;
    string   phy_gps_result_str_;
    string   phy_ship_result_str_;
    bool     ped_first_valid_found_;
    Long64_t ped_first_valid_index_;
    Long64_t ped_first_valid_week_;
    Long64_t ped_first_valid_second_;
    Long64_t ped_first_ship_second_;
    Long64_t ped_last_valid_index_;
    Long64_t ped_last_valid_week_;
    Long64_t ped_last_valid_second_;
    Long64_t ped_last_ship_second_;
    Long64_t ped_total_valid_count_;
    string   ped_gps_result_str_;
    string   ped_ship_result_str_;
    
private:
    GPSTime calc_abs_time_(const pair<GPSTime, uint32_t>& gps_sync,
                           const GPSTime&  trig_frm_gps,
                           const uint32_t& trig_timestamp,
                           const double&   ticks_per_second);

public:
    Processor();
    ~Processor();

    void initialize();
    
    void set_log(bool flag);
    bool can_log();
    bool logfile_open(const char* filename);
    void logfile_close();

    bool check_sci_hk_match(const SciTransfer& scitran,
                            const HkGPSIterator& hkgpsiter);

    void copy_modules(SciTransfer& scitran);
    void copy_ped_modules(SciTransfer& scitran);
    void calc_time_trigger(SciTransfer& scitran, HkGPSIterator& hkgpsiter);
    void calc_time_ped_trigger(SciTransfer& scitran, HkGPSIterator& hkgpsiter);
    void write_meta_info(SciTransfer& scitran);

    void print_error_count(const SciTransfer& scitran);
};

#endif
