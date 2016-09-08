#ifndef SCIFILER_H
#define SCIFILER_H

#include <iostream>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <boost/regex.hpp>
#include "RootInc.hpp"
#include "SciType1P.hpp"

#define GPS_SPAN_MIN 120
#define GPS_DIFF_MIN 30

using namespace std;
using boost::regex;
using boost::cmatch;
using boost::regex_match;

class SciFileR: private SciType1P {
private:
    regex  re_gps_;
    regex  re_gps_span_;
    string gps_str_begin_;
    string gps_str_end_;
    double gps_value_begin_;
    double gps_value_end_;

    string  name_str_file_in_;
    TFile*  t_file_in_;
    TTree*  t_modules_tree_;
    TTree*  t_trigger_tree_;
    TTree*  t_ped_modules_tree_;
    TTree*  t_ped_trigger_tree_;
    TNamed* m_phy_gps_;
    TNamed* m_ped_gps_;
    
    string gps_str_first_phy_;
    string gps_str_first_ped_;
    string gps_str_last_phy_;
    string gps_str_last_ped_;
    double gps_value_first_phy_;
    double gps_value_first_ped_;
    double gps_value_last_phy_;
    double gps_value_last_ped_;

    Long64_t phy_modules_first_entry_;
    Long64_t phy_trigger_first_entry_;
    Long64_t ped_modules_first_entry_;
    Long64_t ped_trigger_first_entry_;
    Long64_t phy_modules_last_entry_;
    Long64_t phy_trigger_last_entry_;
    Long64_t ped_modules_last_entry_;
    Long64_t ped_trigger_last_entry_;

private:
    double value_of_gps_str_(const string gps_str);
    void find_trigger_first_();
    void find_trigger_last_();
    void find_modules_first_();
    void find_modules_last_();

    Long64_t find_entry_(TTree* t_tree, Trigger_T& t_branch, double gps_value);
    
public:
    Modules_T t_modules;
    Trigger_T t_trigger;
    Modules_T t_ped_modules;
    Trigger_T t_ped_trigger;

public:
    // phy_trigger_first
    Long64_t phy_trigger_first_trigg_num;
    Long64_t phy_trigger_first_trigg_num_g;
    UInt_t   phy_trigger_first_time_stamp;
    UInt_t   phy_trigger_first_time_period;
    Long64_t phy_trigger_first_pkt_start;

    // ped_trigger_first
    Long64_t ped_trigger_first_trigg_num;
    Long64_t ped_trigger_first_trigg_num_g;
    UInt_t   ped_trigger_first_time_stamp;
    UInt_t   ped_trigger_first_time_period;
    Long64_t ped_trigger_first_pkt_start;

    // all_trigger_first
    Long64_t all_trigger_first_trigg_num_g;
    UInt_t   all_trigger_first_time_stamp;
    UInt_t   all_trigger_first_time_period;

    // phy_trigger_last
    Long64_t phy_trigger_last_trigg_num;
    Long64_t phy_trigger_last_trigg_num_g;
    UInt_t   phy_trigger_last_time_stamp;
    UInt_t   phy_trigger_last_time_period;
    Long64_t phy_trigger_last_pkt_start;

    // ped_trigger_last
    Long64_t ped_trigger_last_trigg_num;
    Long64_t ped_trigger_last_trigg_num_g;
    UInt_t   ped_trigger_last_time_stamp;
    UInt_t   ped_trigger_last_time_period;
    Long64_t ped_trigger_last_pkt_start;

    // all_trigger_last
    Long64_t all_trigger_last_trigg_num_g;
    UInt_t   all_trigger_last_time_stamp;
    UInt_t   all_trigger_last_time_period;

    // phy_modules_first
    bool     phy_modules_first_found[25];
    Long64_t phy_modules_first_event_num[25];
    Long64_t phy_modules_first_event_num_g[25];
    UInt_t   phy_modules_first_time_stamp[25];
    UInt_t   phy_modules_first_time_period[25];

    // ped_modules_first
    bool     ped_modules_first_found[25];
    Long64_t ped_modules_first_event_num[25];
    Long64_t ped_modules_first_event_num_g[25];
    UInt_t   ped_modules_first_time_stamp[25];
    UInt_t   ped_modules_first_time_period[25];

    // all_modules_first
    Long64_t all_modules_first_event_num_g[25];
    UInt_t   all_modules_first_time_stamp[25];
    UInt_t   all_modules_first_time_period[25];

    // phy_modules_last
    bool     phy_modules_last_found[25];
    Long64_t phy_modules_last_event_num[25];
    Long64_t phy_modules_last_event_num_g[25];
    UInt_t   phy_modules_last_time_stamp[25];
    UInt_t   phy_modules_last_time_period[25];

    // ped_modules_last
    bool     ped_modules_last_found[25];
    Long64_t ped_modules_last_event_num[25];
    Long64_t ped_modules_last_event_num_g[25];
    UInt_t   ped_modules_last_time_stamp[25];
    UInt_t   ped_modules_last_time_period[25];

    // all_modules_last
    Long64_t all_modules_last_event_num_g[25];
    UInt_t   all_modules_last_time_stamp[25];
    UInt_t   all_modules_last_time_period[25];

private:
    Long64_t phy_trigger_cur_entry_;
    bool     phy_trigger_reach_end_;

    Long64_t phy_modules_cur_entry_;
    bool     phy_modules_reach_end_;

    Long64_t ped_trigger_cur_entry_;
    bool     ped_trigger_reach_end_;

    Long64_t ped_modules_cur_entry_;
    bool     ped_modules_reach_end_;
    
public:
    SciFileR();
    ~SciFileR();

    bool open(const char* filename, const char* gps_begin, const char* gps_end);
    void close();
    void print_file_info();

    string    get_filename();
    double    get_gps_value_first();
    double    get_gps_value_last();

    void     phy_trigger_set_start();
    Long64_t phy_trigger_get_tot_entries();
    Long64_t phy_trigger_get_cur_entry();
    bool     phy_trigger_next();

    void     phy_modules_set_start();
    Long64_t phy_modules_get_tot_entries();
    Long64_t phy_modules_get_cur_entry();
    bool     phy_modules_next();

    void     ped_trigger_set_start();
    Long64_t ped_trigger_get_tot_entries();
    Long64_t ped_trigger_get_cur_entry();
    bool     ped_trigger_next();

    void     ped_modules_set_start();
    Long64_t ped_modules_get_tot_entries();
    Long64_t ped_modules_get_cur_entry();
    bool     ped_modules_next();
    
};

#endif
