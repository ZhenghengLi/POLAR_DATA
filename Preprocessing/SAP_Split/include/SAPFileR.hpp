#ifndef SAPFILER_H
#define SAPFILER_H

#include <iostream>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <boost/regex.hpp>
#include "RootInc.hpp"
#include "SAPType.hpp"

#define TIME_SPAN_MIN 60
#define TIME_DIFF_MIN 30

using namespace std;
using boost::regex;
using boost::cmatch;
using boost::regex_match;

class SAPFileR: public SAPType {
private:
    TString time_begin_str_;
    TString time_end_str_;
    double  time_begin_value_;
    double  time_end_value_;

    string  filename_str_;
    TFile*  t_file_in_;
    TTree*  t_pol_event_tree_;

    TNamed* m_time_span_;
    regex   re_time_span_;
    double  time_first_;
    double  time_last_;

    Long64_t entry_begin_;
    Long64_t entry_end_;

private:
    Long64_t find_entry_(double event_time);

public:
    POLEvent_T t_pol_event;

private:
    Long64_t pol_event_cur_entry_;
    Long64_t pol_event_reach_end_;

public:
    SAPFileR();
    ~SAPFileR();

    bool open(const char* filename,
            const char* time_begin,
            const char* time_end);
    void close();
    void print_file_info();

    string get_filename();
    double get_time_first();
    double get_time_last();

    void     pol_event_set_start();
    bool     pol_event_next();
    Long64_t pol_event_get_cur_entry();
    Long64_t pol_event_get_tot_entries();
    Long64_t pol_event_get_entry_begin();
    Long64_t pol_event_get_entry_end();

};

#endif
