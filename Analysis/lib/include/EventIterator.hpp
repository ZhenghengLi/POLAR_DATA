#ifndef EVENTITERATOR_H
#define EVENTITERATOR_H

#include "SciType.hpp"

class EventIterator: private SciType {
private:
    TFile* t_file_in_;
    TTree* t_trigger_;
    TTree* t_modules_;
    TTree* t_ped_trigger_;
    TTree* t_ped_modules_;

    bool   cur_is_1P_;

    Long64_t trigg_tot_entries_;
    Long64_t trigg_cur_index_;
    Long64_t module_cur_index_;
    Long64_t module_start_index_;
    Long64_t module_end_index_;
    bool     trigg_reach_end_;
    bool     trigg_is_bad_;
    bool     module_reach_end_;

    Long64_t ped_trigg_tot_entries_;
    Long64_t ped_trigg_cur_index_;
    Long64_t ped_module_cur_index_;
    Long64_t ped_module_start_index_;
    Long64_t ped_module_end_index_;
    bool     ped_trigg_reach_end_;
    bool     ped_trigg_is_bad_;
    bool     ped_module_reach_end_;

public:
    Trigger_T cur_trigg;
    Modules_T cur_module;
    Trigger_T cur_ped_trigg;
    Modules_T cur_ped_module;
    
public:
    EventIterator();
    ~EventIterator();

    bool open(const char* filename);
    void close();
    
    void trigg_set_start();
    bool trigg_next();
    bool trigg_select(Long64_t entry_index);
    void module_set_start();
    bool module_next();
    Long64_t get_trigg_cur_index() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return trigg_cur_index_;
    }
    Long64_t get_trigg_entries() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return trigg_tot_entries_;
    }
    
    void ped_trigg_set_start();
    bool ped_trigg_next();
    bool ped_trigg_select(Long64_t entry_index);
    void ped_module_set_start();
    bool ped_module_next();
    Long64_t get_ped_trigg_cur_index() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return ped_trigg_cur_index_;
    }
    Long64_t get_ped_trigg_entries() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return ped_trigg_tot_entries_;
    }

    bool is_1P() {
        return cur_is_1P_;
    }

};

#endif
