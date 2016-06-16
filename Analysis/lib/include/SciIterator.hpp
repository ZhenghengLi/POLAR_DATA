#ifndef SCIITERATOR_H
#define SCIITERATOR_H

#include "SciType.hpp"

class SciIterator: private SciType {
private:
    TFile* t_file_in_;
    TTree* t_trigger_;
    TTree* t_modules_;
    TTree* t_ped_trigger_;
    TTree* t_ped_modules_;

    bool   cur_is_1P_;

    Long64_t trigger_tot_entries_;
    Long64_t trigger_cur_index_;
    bool     trigger_reach_end_;
    Long64_t modules_tot_entries_;
    Long64_t modules_cur_index_;
    bool     modules_reach_end_;

    Long64_t ped_trigger_tot_entries_;
    Long64_t ped_trigger_cur_index_;
    bool     ped_trigger_reach_end_;
    Long64_t ped_modules_tot_entries_;
    Long64_t ped_modules_cur_index_;
    bool     ped_modules_reach_end_;

public:
    Trigger_T trigger;
    Modules_T modules;
    Trigger_T ped_trigger;
    Modules_T ped_modules;
    
public:
    SciIterator();
    ~SciIterator();

    bool open(const char* filename);
    void close();

    void trigger_set_start();
    bool trigger_next();
    bool trigger_select(Long64_t entry_index);
    Long64_t get_trigger_cur_index() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return trigger_cur_index_;
    }
    Long64_t get_trigger_entries() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return trigger_tot_entries_;
    }
    void modules_set_start();
    bool modules_next();
    bool modules_select(Long64_t entry_index);
    Long64_t get_modules_cur_index() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return modules_cur_index_;
    }
    Long64_t get_modules_entries() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return modules_tot_entries_;
    }

    void ped_trigger_set_start();
    bool ped_trigger_next();
    bool ped_trigger_select(Long64_t entry_index);
    Long64_t get_ped_trigger_cur_index() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return ped_trigger_cur_index_;
    }
    Long64_t get_ped_trigger_entries() {
        if (t_file_in_ == NULL) 
            return -1;
        else
            return ped_trigger_tot_entries_;
    }
    void ped_modules_set_start();
    bool ped_modules_next();
    bool ped_modules_select(Long64_t entry_index);
    Long64_t get_ped_modules_cur_index() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return ped_modules_cur_index_;
    }
    Long64_t get_ped_modules_entries() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return ped_modules_tot_entries_;
    }

    bool is_1P() {
        return cur_is_1P_;
    }

};

#endif
