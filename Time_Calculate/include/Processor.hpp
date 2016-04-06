#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <stdint.h>
#include "Constants.hpp"
#include "GPSTime.hpp"
#include "HkGPSIterator.hpp"
#include "SciTransfer.hpp"

#define MAX_DIFF 5
#define MAX_OFFSET 300

using namespace std;

class Processor {
private:
    bool log_flag_;
    ofstream os_logfile_;

    int64_t phy_error_count_;
    int64_t ped_error_count_;

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

    void copy_modules(SciTransfer& scitran);
    void copy_ped_modules(SciTransfer& scitran);
    void calc_time_trigger(SciTransfer& scitran);
    void calc_time_ped_trigger(SciTransfer& scitran);
    void write_meta_info(SciTransfer& scitran);
};

#endif
