#ifndef HKFILER_H
#define HKFILER_H

#include <iostream>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <boost/regex.hpp>
#include "RootInc.hpp"
#include "HkType1M.hpp"

#define GPS_SPAN_MIN 120
#define GPS_DIFF_MIN 30

using namespace std;
using boost::regex;
using boost::cmatch;
using boost::regex_match;

class HkFileR: private HkType1M {
private:
    regex  re_gps_;
    regex  re_gps_span_;
    string gps_str_begin_;
    string gps_str_end_;
    double gps_value_begin_;
    double gps_value_end_;

    string  name_str_file_in_;
    TFile*  t_file_in_;
    TTree*  t_hk_obox_tree_;
    TTree*  t_hk_ibox_tree_;
    TNamed* m_oboxgps_;
    TNamed* m_iboxgps_;

    string gps_str_first_hk_obox_;
    string gps_str_first_hk_ibox_;
    string gps_str_last_hk_obox_;
    string gps_str_last_hk_ibox_;
    double gps_value_first_hk_obox_;
    double gps_value_first_hk_ibox_;
    double gps_value_last_hk_obox_;
    double gps_value_last_hk_ibox_;

    Long64_t hk_obox_first_entry_;
    Long64_t hk_ibox_first_entry_;
    Long64_t hk_obox_last_entry_;
    Long64_t hk_ibox_last_entry_;

private:
    double value_of_gps_str_(const string gps_str);
    Long64_t find_entry_obox_(TTree* t_tree, Hk_Obox_T& t_branch, double gps_value);
    Long64_t find_entry_ibox_(TTree* t_tree, Hk_Ibox_T& t_branch, double gps_value);

private:
    Long64_t hk_obox_cur_entry_;
    bool     hk_obox_reach_end_;

    Long64_t hk_ibox_cur_entry_;
    bool     hk_ibox_reach_end_;
    
public:
    Hk_Obox_T t_hk_obox;
    Hk_Ibox_T t_hk_ibox;
    
public:
    HkFileR();
    ~HkFileR();

    bool open(const char* filename, const char* gps_begin, const char* gps_end);
    void close();
    void print_file_info();

    string get_filename();
    double get_gps_value_first();
    double get_gps_value_last();

    void     hk_obox_set_start();
    Long64_t hk_obox_get_tot_entries();
    Long64_t hk_obox_get_cur_entry();
    bool     hk_obox_next();

    void     hk_ibox_set_start();
    Long64_t hk_ibox_get_tot_entries();
    Long64_t hk_ibox_get_cur_entry();
    bool     hk_ibox_next();

};

#endif
