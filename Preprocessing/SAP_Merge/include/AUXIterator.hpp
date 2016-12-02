#ifndef AUXITERATOR_H
#define AUXITERATOR_H

#include <iostream>
#include <boost/regex.hpp>
#include "RootInc.hpp"

using namespace std;
using boost::regex;
using boost::cmatch;
using boost::regex_match;

class AUXIterator {
public:
    struct Hk_Obox_T {
        double         gps_time_sec;
        UShort_t       packet_num;
        Int_t          abs_gps_week;
        Double_t       abs_gps_second;
        Int_t          obox_is_bad;
        UChar_t        obox_mode;
        Float_t        fe_hv[25];
        Float_t        fe_thr[25];
        Float_t        fe_temp[25];
    };

private:
    TFile* t_file_in_;
    TTree* t_hk_obox_tree_;

    regex  re_gps_span_;
    double first_gps_time_;
    double last_gps_time_;

    Long64_t hk_obox_cur_entry_;
    bool     hk_obox_reach_end_;

    Hk_Obox_T cur_hk_obox_;

public:
    Hk_Obox_T hk_obox_before;
    Hk_Obox_T hk_obox_after;
    Float_t   fe_thr_current[25];
    Float_t   fe_thr_next[25];
    Double_t  fe_thr_gps_time_current;
    Double_t  fe_thr_gps_time_next;

public:
    AUXIterator();
    ~AUXIterator();

    bool open(const char* filename);
    void close();

    bool next_obox();
    bool get_reach_end();

    double get_first_gps_time();
    double get_last_gps_time();

};

#endif
