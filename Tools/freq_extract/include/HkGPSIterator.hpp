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
    ULong64_t b_gps_count_pps_;
    ULong64_t b_gps_count_gen_;
    Int_t     b_obox_is_bad_;
    UChar_t   b_obox_mode_;

    Int_t     b_abs_gps_week_;
    Double_t  b_abs_gps_second_;
    Float_t   b_fe_temp_[25];
    Float_t   b_ct_temp_;

    Long64_t hk_obox_tot_entries_;
    Long64_t hk_obox_cur_index_;
    bool hk_obox_reach_end_;
    bool passed_last_;

    pair<GPSTime, uint32_t> pre_gps_sync_;
    pair<GPSTime, uint32_t> cur_gps_sync_;
    Int_t                   cur_obox_mode_;

    bool first_valid_;
    bool before_valid_;
    bool after_valid_;
    bool cur_is_valid_;

public:
    pair<GPSTime, uint32_t> before_gps_sync;
    pair<GPSTime, uint32_t> after_gps_sync;
    pair<GPSTime, uint32_t> first_gps_sync;
    pair<GPSTime, uint32_t> last_gps_sync;

    double cur_ticks_per_second;
    
private:
    bool next_pair_();
    bool set_first_();
    bool set_last_();
    bool check_cur_valid_();
    double current_mean_temp_();
    double current_met_time_();

public:
    struct Freq_Type {
        double met_time;
        double mean_temp;
        double freq;
        bool   found_init;
    } cur_freq;

public:
    HkGPSIterator();
    ~HkGPSIterator();

    bool open(const char* filename);
    void close();
    bool initialize();
    bool next_minute();

    bool is_before_valid() const {
        return before_valid_;
    }

    bool is_after_valid() const {
        return after_valid_;
    }

    bool is_passed_last() const {
        return passed_last_;
    }
};

#endif
