#ifndef HkFileL1Conv_H
#define HkFileL1Conv_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "RootInc.hpp"

using namespace std;

class HkFileL1Conv {
public:
    struct POL_HK_T {
        Int_t     odd_is_bad;
        Int_t     even_is_bad;
        Int_t     obox_is_bad;
        ULong64_t gps_pps_count;
        Int_t     abs_gps_week;
        Double_t  abs_gps_second;
        Bool_t    abs_gps_valid;
        Double_t  abs_ship_second;
        UShort_t  packet_num;
        UChar_t   obox_mode;
        Float_t   ct_temp;
        Float_t   chain_temp;
        UChar_t   fe_status[25];
        Float_t   fe_temp[25];
        Float_t   fe_hv[25];
        Float_t   fe_thr[25];
        UShort_t  fe_rate[25];
        UShort_t  fe_cosmic[25];
        UShort_t  saa;
    };

public:
    POL_HK_T t_pol_hk;

private:
    TFile* t_file_in_;
    TFile* t_file_out_;
    TTree* t_hk_obox_tree_;
    TTree* t_pol_hk_tree_;

    Long64_t hk_obox_cur_entry_;
    bool     hk_obox_reach_end_;
    Long64_t hk_obox_bad_count_;

    Long64_t pol_hk_cur_index_;
    bool     pol_hk_first_gps_found_;
    Long64_t pol_hk_first_gps_index_;
    Long64_t pol_hk_first_gps_week_;
    Long64_t pol_hk_first_gps_second_;
    Long64_t pol_hk_last_gps_index_;
    Long64_t pol_hk_last_gps_week_;
    Long64_t pol_hk_last_gps_second_;
    Long64_t pol_hk_total_gps_count_;
    string   pol_hk_gps_result_str_;
    string   pol_hk_extra_info_str_;
    
public:
    HkFileL1Conv();
    ~HkFileL1Conv();

    bool open_r(const char* filename);
    bool open_w(const char* filename);
    void close_r();
    void close_w();

    void     hk_obox_set_start();
    Long64_t hk_obox_get_cur_entry();
    Long64_t hk_obox_get_tot_entries();
    bool     hk_obox_next();
    void     pol_hk_fill();
    void     pol_hk_write_tree();
    void     pol_hk_write_meta(const char* key, const char* value);
    void     pol_hk_gen_gps_span_str();
    void     pol_hk_gen_extra_info_str();
    void     pol_hk_write_gps_span();
    void     pol_hk_write_extra_info();
    void     pol_hk_print_gps_span();
    void     pol_hk_print_extra_info();
    
};

#endif
