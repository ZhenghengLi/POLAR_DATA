#ifndef SCIFILEL1W_H
#define SCIFILEL1W_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "RootInc.hpp"

using namespace std;

class SciFileL1W {
public:
    struct POL_Event_T {
        Int_t     abs_gps_week;
        Double_t  abs_gps_second;
        Bool_t    abs_gps_valid;
        Char_t    type[10];
        Bool_t    trig_accepted[25];
        Bool_t    time_aligned[25];
        Int_t     pkt_count;
        Int_t     lost_count;
        Bool_t    trigger_bit[1600];
        Int_t     trigger_n;
        Int_t     multiplicity[25];
        Float_t   energy_adc[1600];
        Int_t     compress[25];
        Float_t   common_noise[25];
    };

public:
    POL_Event_T t_pol_event;
    
private:
    TFile* t_file_out_;
    TTree* t_pol_event_tree_;

    Long64_t pol_event_cur_index_;
    
    bool     first_valid_found_;
    Long64_t first_valid_index_;
    Long64_t first_valid_week_;
    Long64_t first_valid_second_;
    Long64_t last_valid_index_;
    Long64_t last_valid_week_;
    Long64_t last_valid_second_;
    Long64_t total_valid_count_;
    string   gps_result_str_;

public:
    SciFileL1W();
    ~SciFileL1W();

    bool open(const char* filename);
    void close();
    void fill_event();
    void write_tree();
    void write_meta(const char* key, const char* value);
    void gen_gps_result_str();
    void write_gps_span();
    void print_gps_span();
};

#endif
