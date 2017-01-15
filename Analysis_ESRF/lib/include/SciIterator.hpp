#ifndef SCIITERATOR_H
#define SCIITERATOR_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <boost/regex.hpp>
#include "RootInc.hpp"
#include "SciType.hpp"

#define GPS_SPAN_MIN 10
#define GPS_DIFF_MIN 10

using namespace std;
using boost::regex;
using boost::cmatch;
using boost::regex_match;

class SciIterator: private SciType {
private:
    regex  re_gps_;
    regex  re_gps_span_;
    string gps_str_begin_;
    string gps_str_end_;
    double gps_value_begin_;
    double gps_value_end_;

    string  name_str_file_in_;
    TFile* t_file_in_;
    TTree* t_modules_tree_;
    TTree* t_trigger_tree_;
    TTree* t_ped_modules_tree_;
    TTree* t_ped_trigger_tree_;
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
    
    bool   cur_is_1P_;

    Long64_t phy_modules_first_entry_;
    Long64_t phy_trigger_first_entry_;
    Long64_t ped_modules_first_entry_;
    Long64_t ped_trigger_first_entry_;
    Long64_t phy_modules_last_entry_;
    Long64_t phy_trigger_last_entry_;
    Long64_t ped_modules_last_entry_;
    Long64_t ped_trigger_last_entry_;

private:
    
    Long64_t phy_trigger_cur_entry_;
    bool     phy_trigger_reach_end_;

    Long64_t phy_modules_cur_entry_;
    bool     phy_modules_reach_end_;

    Long64_t ped_trigger_cur_entry_;
    bool     ped_trigger_reach_end_;

    Long64_t ped_modules_cur_entry_;
    bool     ped_modules_reach_end_;

private:
    double value_of_gps_str_(const string gps_str);
    Long64_t find_entry_(TTree* t_tree, Trigger_T& t_branch, double gps_value);

public:
    Trigger_T t_trigger;
    Modules_T t_modules;
    Trigger_T t_ped_trigger;
    Modules_T t_ped_modules;
    
public:
    SciIterator();
    ~SciIterator();

    bool open(const char* filename);
    bool open(const char* filename,
              const char* gps_begin, const char* gps_end);
    void close();
    void print_file_info();

    string   get_filename();
    string   get_ped_first_gps();
    string   get_ped_last_gps();
    string   get_phy_first_gps();
    string   get_phy_last_gps();
    
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

    bool     file_is_1P();

};

#endif
