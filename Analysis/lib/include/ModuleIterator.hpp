#ifndef MODULEITERATOR_H
#define MODULEITERATOR_H

#include "SciType.hpp"

class ModuleIterator: private SciType {
private:
    int cur_ct_num_;
    Long64_t cur_seq_num_;
    Long64_t pre_seq_num_;
    
    bool ped_trigg_reach_end_;
    bool ped_event_reach_end_;
    bool phy_trigg_reach_end_;
    bool phy_event_reach_end_;

    bool trigg_start_flag_;
    bool event_start_flag_;
    
    Long64_t ped_trigg_cur_entry_;
    Long64_t ped_trigg_tot_entries_;
    Long64_t ped_event_cur_entry_;
    Long64_t ped_event_tot_entries_;
    Long64_t phy_trigg_cur_entry_;
    Long64_t phy_trigg_tot_entries_;
    Long64_t phy_event_cur_entry_;
    Long64_t phy_event_tot_entries_;

    TFile* t_file_in_;
    TTree* t_trigger_;
    TTree* t_modules_;
    TTree* t_ped_trigger_;
    TTree* t_ped_modules_;

    Trigger_T phy_trigg_;
    Trigger_T ped_trigg_;
    Modules_T phy_event_;
    Modules_T ped_event_;
    
public:
    Trigger_T cur_trigg;    
    Modules_T cur_event;

private:
    bool ped_trigg_next_();
    bool phy_trigg_next_();
    bool ped_event_next_();
    bool phy_event_next_();
    
public:
    ModuleIterator();
    ~ModuleIterator();

    bool open(const char* filename);
    void close();
    bool trigg_next();
    bool event_next();
    bool set_module(int ct_num);
    bool set_trigger();
    bool set_start();
    int  get_cur_seq();
    int  get_bad_cnt();

};

#endif
