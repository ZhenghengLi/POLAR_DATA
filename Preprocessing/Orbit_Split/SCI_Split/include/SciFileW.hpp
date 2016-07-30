#ifndef SCIFILEW_H
#define SCIFILEW_H

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include "RootInc.hpp"
#include "SciType1P.hpp"
#include "SciFileR.hpp"

#define TRIGG_MIN_TIMESTAMP_DIFF (-5 * 12500000)
#define EVENT_MIN_TIMESTAMP_DIFF (-5 * 24414)

using namespace std;

class SciFileW: private SciType1P {
private:
    TFile* t_file_out_;
    TTree* t_modules_tree_;
    TTree* t_trigger_tree_;
    TTree* t_ped_modules_tree_;
    TTree* t_ped_trigger_tree_;

    SciFileR* cur_scifile_r;
    Long64_t  cur_phy_trigg_num_offset_;
    Long64_t  cur_ped_trigg_num_offset_;
    Long64_t  cur_phy_event_num_offset_[25];
    Long64_t  cur_ped_event_num_offset_[25];
    
    Long64_t  cur_trigg_num_g_offset_;
    Long64_t  cur_event_num_g_offset_[25];

    Int_t     cur_trigger_time_period_offset_;
    Int_t     cur_modules_time_period_offset_[25];

    Long64_t  cur_phy_pkt_start_offset_;
    Long64_t  cur_ped_pkt_start_offset_;

    int64_t   cur_time_stamp_diff_;

private:
    Long64_t phy_trigger_cur_index_;
    Long64_t ped_trigger_cur_index_;
    
    bool     phy_first_valid_found_;
    Long64_t phy_first_valid_index_;
    Long64_t phy_first_valid_week_;
    Long64_t phy_first_valid_second_;
    Long64_t phy_last_valid_index_;
    Long64_t phy_last_valid_week_;
    Long64_t phy_last_valid_second_;
    Long64_t phy_total_valid_count_;
    string   phy_gps_result_str_;
    bool     ped_first_valid_found_;
    Long64_t ped_first_valid_index_;
    Long64_t ped_first_valid_week_;
    Long64_t ped_first_valid_second_;
    Long64_t ped_last_valid_index_;
    Long64_t ped_last_valid_week_;
    Long64_t ped_last_valid_second_;
    Long64_t ped_total_valid_count_;
    string   ped_gps_result_str_;
    
public:
    Modules_T t_modules;
    Trigger_T t_trigger;
    Modules_T t_ped_modules;
    Trigger_T t_ped_trigger;
    
public:
    SciFileW();
    ~SciFileW();

    bool open(const char* filename);
    void close();
    void set_scifile_r(SciFileR* scifile_r);
    void write_phy_trigger();
    void write_ped_trigger();
    void write_phy_modules();
    void write_ped_modules();
    void write_before_close();
    void write_meta(const char* key, const char* value);
    void gen_gps_result_str();
    void write_gps_span();
    void print_gps_span();
};

#endif
