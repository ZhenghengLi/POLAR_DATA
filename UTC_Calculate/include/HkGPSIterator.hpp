#ifndef HKGPSITERATOR_H
#define HKGPSITERATOR_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <utility>
#include <stdint.h>
#include "RootInc.hpp"
#include "GPSTime.hpp"

using namespace std;

class HkGPSIterator {
private:
    TFile*    t_file_in_;
    TTree*    t_hk_obox_;
    UInt_t    b_timestamp_;
    ULong64_t b_gps_count_;
    Int_t     b_obox_is_bad_;

    Long64_t hk_obox_tot_entries_;
    Long64_t hk_obox_cur_index_;
    bool hk_obox_reach_end_;

    bool start_flag_;
    pair<GPSTime, uint32_t> pre_gps_sync_;
    pair<GPSTime, uint32_t> cur_gps_sync_;        
    pair<GPSTime, uint32_t> before_gps_sync_;
    pair<GPSTime, uint32_t> after_gps_sync_;
    pair<GPSTime, uint32_t> first_gps_sync_;
    pair<GPSTime, uint32_t> last_gps_sync_;

    double cur_ticks_per_second_;
    bool passed_last_;
    
private:
    bool next_pair_();
    bool set_first_();
    bool set_last_();
    
public:
    HkGPSIterator();
    ~HkGPSIterator();

    bool open(const char* filename);
    void close();
    bool initialize();
    bool next_minute();

    bool is_passed_last() {
        return passed_last_;
    }
};

#endif
