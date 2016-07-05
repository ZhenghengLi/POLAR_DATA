#ifndef SPECDATAFILE_H
#define SPECDATAFILE_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "RootInc.hpp"

using namespace std;

class SpecDataFile {
public:
    struct SourceEvent_T {
        Int_t    type;
        Bool_t   trig_accepted[25];
        Bool_t   trigger_bit[1600];
        Int_t    trigger_n;
        Int_t    multiplicity[25];
        Float_t  energy_adc[1600];
    };

public:
    SourceEvent_T t_source_event;

private:
    TFile*  t_spec_file_;
    TTree*  t_source_event_tree_;
    TNamed* m_fromfile_;
    TNamed* m_gps_span_;

    bool is_first_created_;

    char name_[80];
    char title_[80];

    char mode_; // 'w' | 'r'

    Long64_t event_cur_entry_;
    bool     event_reach_end_;
    
public:
    SpecDataFile();
    ~SpecDataFile();

    bool open(const char* filename, char m);
    void close();
    void clear_cur_entry();

public: // write
    void event_fill();
    void write_all_tree();
    void write_meta(const char* key,
                    const char* value,
                    bool append_flag = true);
    void write_fromfile(const char* filename);
    void write_gps_span(const char* begin_gps,
                        const char* end_gps);
    void write_lasttime();

public: // read
    void     event_set_start();
    Long64_t event_get_cur_entry();
    Long64_t event_get_tot_entries();
    bool     event_next();
    string   get_fromfile_str();
    string   get_gps_span_str();
    char     get_mode();
    
};

#endif
