#ifndef MODULEITERATOR_H
#define MODULEITERATOR_H

#include <cstdio>
#include "SciType.hpp"

using namespace std;

class ModuleIterator: private SciType {
private:
    TFile* t_file_in_;
    TTree* t_trigger_;
    TTree* t_modules_;
    TTree* t_ped_trigger_;
    TTree* t_ped_modules_;

    int      cur_ct_num_;
    string   cur_filter_;
    Long64_t cur_seq_num_;
    Long64_t pre_seq_num_;
    
    bool ped_trigg_reach_end_;
    bool ped_event_reach_end_;
    bool phy_trigg_reach_end_;
    bool phy_event_reach_end_;

    bool trigg_start_flag_;
    bool event_start_flag_;

    TEventList* ped_trigg_elist_;
    Int_t       ped_trigg_cur_index_;

    TEventList* ped_event_elist_;
    Int_t       ped_event_cur_index_;

    TEventList* phy_trigg_elist_;
    Int_t       phy_trigg_cur_index_;

    TEventList* phy_event_elist_;
    Int_t       phy_event_cur_index_;

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

    bool      open(const char* filename);
    void      close();
    bool      trigg_next();
    bool      event_next();
    bool      set_module(int ct_num, string filter = "");
    bool      set_trigger(string filter = "");
    void      set_start();
    Int_t     get_tot_N();
    int       get_cur_seq();
    int       get_bad_cnt();
    int       get_cur_ct_num();
    string    get_cur_filter();
    bool      cur_is_ped();

    Modules_T get_first_event();
    Modules_T get_last_event();
    Trigger_T get_first_trigg();
    Trigger_T get_last_trigg();

    const TTree* get_trigger_tree();
    const TTree* get_modules_tree();
    const TTree* get_ped_trigger_tree();
    const TTree* get_ped_modules_tree();
    
};

#endif
