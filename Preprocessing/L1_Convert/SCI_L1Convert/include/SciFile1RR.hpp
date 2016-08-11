#ifndef SCIFILE1RR_H
#define SCIFILE1RR_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "RootInc.hpp"

using namespace std;

class SciFile1RR {
public:
    struct TriggerL1_T {
        Int_t      is_bad;        
        Long64_t   trigg_num;
        Int_t      abs_gps_week;
        Double_t   abs_gps_second;
        Bool_t     abs_gps_valid;
        Double_t   abs_ship_second;
        Int_t      type;
        Bool_t     trig_accepted[25];
        Long64_t   pkt_start;
        Int_t      pkt_count;
        Int_t      lost_count;
        Int_t      trigger_n;
    };

    struct ModulesL1_T {
        Long64_t   trigg_num;
        Long64_t   event_num;
        Int_t      ct_num;
        Bool_t     trigger_bit[64];
        Int_t      multiplicity;
        Float_t    energy_adc[64];
        Int_t      compress;
        Float_t    common_noise;        
    };

public:
    TriggerL1_T t_triggerl1;
    ModulesL1_T t_modulesl1;

private:
    TFile* t_file_in_;
    TTree* t_triggerl1_tree_;
    TTree* t_modulesl1_tree_;

    Long64_t trigger_cur_entry_;
    bool     trigger_reach_end_;

    Long64_t modules_event_start_entry_;
    Long64_t modules_event_stop_entry_;
    Long64_t modules_event_cur_entry_;
    bool     modules_event_reach_end_;

    Long64_t trigg_total_bad_count_;
    Long64_t event_total_intact_count_;    
    Long64_t modules_total_aligned_count_;
    Long64_t modules_total_match_err_count_;

    string   align_info_str_;
    
public:
    SciFile1RR();
    ~SciFile1RR();

    bool open(const char* filename);
    void close();

    void     trigger_set_start();
    Long64_t trigger_get_tot_entries();
    Long64_t trigger_get_cur_entry();
    bool     trigger_next_event();
    bool     modules_next_packet();
    bool     modules_cur_matched();
    Long64_t modules_get_tot_entries();

    Long64_t get_bad_trigger_cnt();
    Long64_t get_intact_event_cnt();
    Long64_t get_aligned_cnt();
    Long64_t get_match_err_cnt();

    void     gen_align_info_str();
    string   get_align_info_str();
    
    void print_align_info();
};

#endif
