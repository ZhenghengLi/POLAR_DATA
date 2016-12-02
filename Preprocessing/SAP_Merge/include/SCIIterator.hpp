#ifndef SCIITERATOR_H
#define SCIITERATOR_H

#include <iostream>
#include <boost/regex.hpp>
#include "RootInc.hpp"
#include "SciType.hpp"

using namespace std;
using boost::regex;
using boost::cmatch;
using boost::regex_match;

class SCIIterator: private SciType {
private:
    TFile* t_file_in_;
    TTree* t_trigger_tree_;
    TTree* t_modules_tree_;
    TTree* t_ped_trigger_tree_;
    TTree* t_ped_modules_tree_;

    bool   is_1p_file_;

    regex  re_gps_span_;
    double first_gps_time_;
    double last_gps_time_;

    bool ped_trigger_reach_end_;
    bool phy_trigger_reach_end_;

    Long64_t ped_trigger_cur_entry_;
    Long64_t phy_trigger_cur_entry_;
    Long64_t ped_modules_cur_entry_;
    Long64_t phy_modules_cur_entry_;

    bool     cur_is_ped_;
    Long64_t cur_ped_pkt_start_;
    Int_t    cur_ped_pkt_count_;
    Long64_t cur_phy_pkt_start_;
    Int_t    cur_phy_pkt_count_;

    Trigger_T phy_trigger_;
    Trigger_T ped_trigger_;
    Modules_T phy_modules_;
    Modules_T ped_modules_;

    Long64_t total_entries_;
    Long64_t bad_entries_;
    Long64_t bad_time_entries_;

    string m_version_value_;
    string m_gentime_value_;
    string m_rawfile_value_;
    string m_dcdinfo_value_;

public:
    Trigger_T cur_trigger;
    Modules_T cur_modules;

private:
    bool phy_trigger_next_();
    bool ped_trigger_next_();

public:
    SCIIterator();
    ~SCIIterator();

    bool open(const char* filename);
    void close();

    void set_start();
    bool next_event();
    bool next_packet();

    bool   get_is_1p();
    double get_first_gps_time();
    double get_last_gps_time();

    string get_bad_ratio_str();
    string get_m_version_str();
    string get_m_gentime_str();
    string get_m_rawfile_str();
    string get_m_dcdinfo_str();

};

#endif
