#ifndef HKFILEW_H
#define HKFILEW_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include "HkType1M.hpp"
#include "HkFileR.hpp"

using namespace std;

class HkFileW: private HkType1M {
private:
    TFile* t_file_out_;
    TTree* t_hk_obox_tree_;
    TTree* t_hk_ibox_tree_;
    
    HkFileR* cur_hkfile_r;

private:
    Long64_t hk_obox_cur_index_;
    Long64_t hk_ibox_cur_index_;
    
    bool     hk_obox_first_gps_found_;
    Long64_t hk_obox_first_gps_index_;
    Long64_t hk_obox_first_gps_week_;
    Long64_t hk_obox_first_gps_second_;
    Long64_t hk_obox_first_ship_second_;
    Long64_t hk_obox_last_gps_index_;
    Long64_t hk_obox_last_gps_week_;
    Long64_t hk_obox_last_gps_second_;
    Long64_t hk_obox_last_ship_second_;
    Long64_t hk_obox_total_gps_count_;
    string   hk_obox_gps_result_str_;
    string   hk_obox_ship_result_str_;
    bool     hk_ibox_first_gps_found_;
    Long64_t hk_ibox_first_gps_index_;
    Long64_t hk_ibox_first_gps_week_;
    Long64_t hk_ibox_first_gps_second_;
    Long64_t hk_ibox_first_ship_second_;
    Long64_t hk_ibox_last_gps_index_;
    Long64_t hk_ibox_last_gps_week_;
    Long64_t hk_ibox_last_gps_second_;
    Long64_t hk_ibox_last_ship_second_;
    Long64_t hk_ibox_total_gps_count_;
    string   hk_ibox_gps_result_str_;
    string   hk_ibox_ship_result_str_;
    
public:
    Hk_Obox_T t_hk_obox;
    Hk_Ibox_T t_hk_ibox;
    
public:
    HkFileW();
    ~HkFileW();

    bool open(const char* filename);
    void close();
    void set_hkfile_r(HkFileR* hkfile_r);
    void write_hk_obox();
    void write_hk_ibox();
    void write_before_close();
    void write_meta(const char* key, const char* value);
    void gen_gps_result_str();
    void write_gps_span();
    void print_gps_span();
    
};

#endif
