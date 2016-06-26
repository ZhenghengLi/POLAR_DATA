#ifndef PEDDATAFILE_H
#define PEDDATAFILE_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "RootInc.hpp"

using namespace std;

class PedDataFile {
public:
    struct PedData_T {
        Float_t ped_adc[64];
    };

public:
    PedData_T t_ped_data[25];

private:
    TFile* t_ped_file_;
    TTree* t_ped_data_tree_[25];

    bool is_first_created_[25];
    
    char name_[80];
    char title_[80];

    char   mode_;  // 'w' | 'r'

    Long64_t mod_cur_entry_[25];
    bool     mod_reach_end_[25];
    
public:
    PedDataFile();
    ~PedDataFile();

    bool open(const char* filename, char m);
    void close();
    
public: // write
    void mod_fill(int ct_num);
    void write_all_tree();
    void write_meta(const char* key, const char* value);
    void write_fromfile(const char* filename);
    void write_gps_span(const char* gps_span);
    void write_lasttime();

public: // read
    void     mod_set_start(int ct_num);
    Long64_t mod_get_cur_entry(int ct_num);
    Long64_t mod_get_tot_entries(int ct_num);
    bool     mod_next(int ct_num);
    
};

#endif
