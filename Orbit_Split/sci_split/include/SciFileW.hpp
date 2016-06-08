#include <iostream>
#include <stdio.h>
#include "RootInc.hpp"
#include "SciType1P.hpp"
#include "SciFileR.hpp"

using namespace std;

class SciFileW: private SciType1P {
private:
    TFile* t_file_out_;
    TTree* t_modules_tree_;
    TTree* t_trigger_tree_;
    TTree* t_ped_modules_tree_;
    TTree* t_ped_trigger_tree_;

    SciFileR* cur_scifile_r;
    Long64_t  cur_phy_trigg_num_offset_;
    Long64_t  cur_ped_trigg_num_offset_;
    Long64_t  cur_phy_event_num_offset_[25];
    Long64_t  cur_ped_event_num_offset_[25];
    
    Long64_t  cur_trigg_num_g_offset_;
    Long64_t  cur_event_num_g_offset_[25];

    UInt_t    cur_trigger_last_time_stamp_;
    UInt_t    cur_modules_last_time_stamp_[25];

    Int_t     cur_trigger_time_period_offset_;
    Int_t     cur_modules_time_period_offset_[25];

public:
    Modules_T t_modules;
    Trigger_T t_trigger;
    Modules_T t_ped_modules;
    Trigger_T t_ped_trigger;
    
public:
    SciFileW();
    ~SciFileW();

    bool open(const char* filename);
    void close();

    void set_scifile_r(SciFileR* scifile_r);

};
