#ifndef XTALKDATAFILE_H
#define XTALKDATAFILE_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "RootInc.hpp"

using namespace std;

class XtalkDataFile {
public:
    struct XtalkData_T {
        Int_t   jx;
        Int_t   jy;
        Float_t x;
        Float_t y;
    };

public:
    XtalkData_T t_xtalk_data[25];

private:
    TFile*  t_xtalk_file_;
    TTree*  t_xtalk_data_tree_[25];
    TNamed* m_fromfile_;
    TNamed* m_gps_span_;

    bool is_first_created_[25];

    char name_[80];
    char title_[80];

    char mode_;  // 'w' | 'r'

    Long64_t mod_cur_entry_[25];
    bool     mod_reach_end_[25];

public:
    XtalkDataFile();
    ~XtalkDataFile();

    bool open(const char* filename, char m);
    void close();

public: // write
    void mod_fill(int ct_idx);
    void write_all_tree();
    void write_meta(const char* key,
                    const char* value,
                    bool append_flag = true);
    void write_fromfile(const char* filename);
    void write_gps_span(const char* begin_gps,
                        const char* end_gps);
    void write_lasttime();

public: // read
    void     mod_set_start(int ct_idx);
    Long64_t mod_get_cur_entry(int ct_idx);
    Long64_t mod_get_tot_entries(int ct_idx);
    bool     mod_next(int ct_idx);
    string   get_fromfile_str();
    string   get_gps_span_str();
    char     get_mode();
    
};

#endif
