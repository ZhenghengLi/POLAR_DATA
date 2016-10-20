#ifndef ANGLEDATAFILE_H
#define ANGLEDATAFILE_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "RootInc.hpp"

using namespace std;

class AngleDataFile {
public:
    struct Angle_T {
        Int_t     abs_gps_week;
        Double_t  abs_gps_second;
        Bool_t    abs_gps_valid;
        Double_t  abs_ship_second;
        Int_t     first_ij[2];
        Int_t     second_ij[2];
        Float_t   rand_angle;
    };
    
public:
    Angle_T t_angle;

private:
    TFile*  t_angle_file_;
    TTree*  t_angle_tree_;
    TNamed* m_fromfile_;
    TNamed* m_gps_span_;

    bool is_first_created_;

    char name_[80];
    char title_[80];

    char mode_; // 'w' | 'r'

    Long64_t angle_cur_entry_;
    bool     angle_reach_end_;

public:
    AngleDataFile();
    ~AngleDataFile();

    bool open(const char* filename, char m);
    void close();

public: // write
    void angle_fill();
    void write_all_tree();
    void write_meta(const char* key,
                    const char* value,
                    bool append_flag = true);
    void write_fromfile(const char* filename);
    void write_gps_span(const char* gps_span);
    void write_lasttime();

public: // read
    void     angle_set_start();
    Long64_t angle_get_cur_entry();
    Long64_t angle_get_tot_entries();
    bool     angle_next();
    string   get_fromfile_str();
    string   get_gps_span_str();
    char     get_mode();
    
};

#endif
