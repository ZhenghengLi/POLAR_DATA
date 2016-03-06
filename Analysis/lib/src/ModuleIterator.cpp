#include "ModuleIterator.hpp"

ModuleIterator::ModuleIterator() {
    cur_ct_num_ = 0;
    cur_seq_num_ = 0;
    ped_trigg_reach_end_ = true;
    ped_event_reach_end_ = true;
    phy_trigg_reach_end_ = true;
    phy_event_reach_end_ = true;
    ped_trigg_cur_entry_ = -1;
    ped_trigg_tot_entries_ = 0;
    ped_event_cur_entry_ = -1;
    ped_event_tot_entries_ = 0;
    phy_trigg_cur_entry_ = -1;
    phy_trigg_tot_entries_ = 0;
    phy_event_cur_entry_ = -1;
    phy_event_tot_entries_ = 0;
    t_file_in_ = NULL;
    t_trigger_ = NULL;
    t_modules_ = NULL;
    t_ped_trigger_ = NULL;
    t_ped_modules_ = NULL;
}

ModuleIterator::~ModuleIterator() {
    
}

bool ModuleIterator::open(const char* filename) {
    if (t_file_in_ != NULL)
        return false;
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_trigger_ = static_cast<TTree*>(t_file_in_->Get("t_trigger"));
    if (t_trigger_ == NULL)
        return false;
    t_modules_ = static_cast<TTree*>(t_file_in_->Get("t_modules"));
    if (t_modules_ == NULL)
        return false;
    t_ped_trigger_ = static_cast<TTree*>(t_file_in_->Get("t_ped_trigger"));
    if (t_ped_trigger_ == NULL)
        return false;
    t_ped_modules_ = static_cast<TTree*>(t_file_in_->Get("t_ped_modules"));
    if (t_ped_modules_ == NULL)
        return false;

    bind_trigger_tree(t_trigger_, phy_trigg_);
    bind_trigger_tree(t_ped_modules_, ped_trigg_);
    bind_modules_tree(t_modules_, phy_event_);
    bind_modules_tree(t_ped_modules_, ped_event_);

    ped_trigg_cur_entry_ = -1;
    ped_trigg_tot_entries_ = t_ped_trigger_->GetEntries();
    ped_event_cur_entry_ = -1;
    ped_event_tot_entries_ = t_ped_modules_->GetEntries();
    phy_trigg_cur_entry_ = -1;
    phy_trigg_tot_entries_ = t_trigger_->GetEntries();
    phy_event_cur_entry_ = -1;
    phy_event_tot_entries_ = t_modules_->GetEntries();
    
    return true;
}

void ModuleIterator::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    t_file_in_ = NULL;
    t_trigger_ = NULL;
    t_modules_ = NULL;
    t_ped_trigger_ = NULL;
    t_ped_modules_ = NULL;
}

bool ModuleIterator::ped_trigg_next_() {

    return true;
}

bool ModuleIterator::phy_trigg_next_() {

    return true;
}

bool ModuleIterator::ped_event_next_() {

    return true;
}

bool ModuleIterator::phy_event_next_() {

    return true;
}

bool ModuleIterator::trigg_next() {

    return true;
}

bool ModuleIterator::event_next() {

    return true;
}

bool ModuleIterator::set_module(int ct_num) {
    if (ct_num < 1 || ct_num > 25)
        ct_num = 1;
    cur_ct_num_ = ct_num;
    return set_start();
}

bool ModuleIterator::set_trigger() {
    cur_ct_num_ = 0;
    return set_start();
}

bool ModuleIterator::set_start() {
    if (cur_ct_num_ > 0) {
        
    } else {

    }
    return true;
}
