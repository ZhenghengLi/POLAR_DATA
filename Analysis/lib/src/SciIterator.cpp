#include "SciIterator.hpp"

SciIterator::SciIterator() {
    t_file_in_ = NULL;
    t_trigger_ = NULL;
    t_modules_ = NULL;
    t_ped_trigger_ = NULL;
    t_ped_modules_ = NULL;
}

SciIterator::~SciIterator() {
    if (t_file_in_ != NULL)
        close();
}

bool SciIterator::open(const char* filename) {
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

    bind_trigger_tree(t_trigger_, trigger);
    bind_trigger_tree(t_ped_trigger_, ped_trigger);
    bind_modules_tree(t_modules_, modules);
    bind_modules_tree(t_ped_modules_, ped_modules);

    trigger_tot_entries_ = t_trigger_->GetEntries();
    trigger_set_start();
    modules_tot_entries_ = t_modules_->GetEntries();
    modules_set_start();
    ped_trigger_tot_entries_ = t_ped_trigger_->GetEntries();
    ped_trigger_set_start();
    ped_modules_tot_entries_ = t_ped_modules_->GetEntries();
    ped_modules_set_start();
    
    return true;
}

void SciIterator::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    t_file_in_ = NULL;
    t_trigger_ = NULL;
    t_modules_ = NULL;
    t_ped_trigger_ = NULL;
    t_ped_modules_ = NULL;
}

void SciIterator::trigger_set_start() {
    trigger_cur_index_ = -1;
    trigger_reach_end_ = false;
}

void SciIterator::modules_set_start() {
    modules_cur_index_ = -1;
    modules_reach_end_ = false;
}

void SciIterator::ped_trigger_set_start() {
    ped_trigger_cur_index_ = -1;
    ped_trigger_reach_end_ = false;
}

void SciIterator::ped_modules_set_start() {
    ped_modules_cur_index_ = -1;
    ped_trigger_reach_end_ = false;
}

bool SciIterator::trigger_next() {
    if (t_file_in_ == NULL)
        return false;
    if (trigger_reach_end_)
        return false;
    trigger_cur_index_++;
    if (trigger_cur_index_ < trigger_tot_entries_) {
        t_trigger_->GetEntry(trigger_cur_index_);
        return true;
    } else {
        trigger_reach_end_ = true;
        return false;
    }
}

bool SciIterator::trigger_select(Long64_t entry_index) {
    if (t_file_in_ == NULL)
        return false;
    if (entry_index < 0 || entry_index >= trigger_tot_entries_)
        return false;
    t_trigger_->GetEntry(entry_index);
    trigger_cur_index_ = entry_index;
    return true;
}

bool SciIterator::modules_next() {
    if (t_file_in_ == NULL)
        return false;
    if (modules_reach_end_)
        return false;
    modules_cur_index_++;
    if (modules_cur_index_ < modules_tot_entries_) {
        t_modules_->GetEntry(modules_cur_index_);
        return true;
    } else {
        modules_reach_end_ = true;
        return false;
    }
}

bool SciIterator::modules_select(Long64_t entry_index) {
    if (t_file_in_ == NULL)
        return false;
    if (entry_index < 0 || entry_index >= modules_tot_entries_)
        return false;
    t_modules_->GetEntry(entry_index);
    modules_cur_index_ = entry_index;
    return true;
}

bool SciIterator::ped_trigger_next() {
    if (t_file_in_ == NULL)
        return false;
    if (ped_trigger_reach_end_)
        return false;
    ped_trigger_cur_index_++;
    if (ped_trigger_cur_index_ < ped_trigger_tot_entries_) {
        t_ped_trigger_->GetEntry(ped_trigger_cur_index_);
        return true;
    } else {
        ped_trigger_reach_end_ = true;
        return false;
    }
}

bool SciIterator::ped_trigger_select(Long64_t entry_index) {
    if (t_file_in_ == NULL)
        return false;
    if (entry_index < 0 || entry_index >= ped_trigger_tot_entries_)
        return false;
    t_ped_trigger_->GetEntry(entry_index);
    ped_trigger_cur_index_ = entry_index;
    return true;
}

bool SciIterator::ped_modules_next() {
    if (t_file_in_ == NULL)
        return false;
    if (ped_modules_reach_end_)
        return false;
    ped_modules_cur_index_++;
    if (ped_modules_cur_index_ < ped_modules_tot_entries_) {
        t_ped_modules_->GetEntry(ped_modules_cur_index_);
        return true;
    } else {
        ped_modules_reach_end_ = true;
        return false;
    }
}

bool SciIterator::ped_modules_select(Long64_t entry_index) {
    if (t_file_in_ == NULL)
        return false;
    if (entry_index < 0 || entry_index >= ped_modules_tot_entries_)
        return false;
    t_ped_modules_->GetEntry(entry_index);
    ped_modules_cur_index_ = entry_index;
    return true;
}

