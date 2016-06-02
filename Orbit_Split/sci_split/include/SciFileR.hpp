#include <iostream>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <boost/regex.hpp>
#include "RootInc.hpp"
#include "SciType1P.hpp"

#define GPS_SPAN_MIN 120

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

public:
    Modules_T t_modules;
    Trigger_T t_trigger;
    Modules_T t_ped_modules;
    Trigger_T t_ped_trigger;
    
public:
    SciFileR();
    ~SciFileR();

    bool open(const char* filename, const char* gps_begin, const char* gps_end);
    void close();
    void print_file_info();
    
};
