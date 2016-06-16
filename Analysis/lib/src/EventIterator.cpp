#include "EventIterator.hpp"

EventIterator::EventIterator() {
    t_file_in_ = NULL;
    t_trigger_ = NULL;
    t_modules_ = NULL;
    t_ped_trigger_ = NULL;
    t_ped_modules_ = NULL;

    cur_is_1P_ = false;
}

EventIterator::~EventIterator() {
    if (t_file_in_ != NULL)
        close();
}

bool EventIterator::open(const char* filename) {
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

    cur_is_1P_ = check_1P(t_trigger_);
    bind_trigger_tree(t_trigger_, cur_trigg);
    bind_trigger_tree(t_ped_trigger_, cur_ped_trigg);
    bind_modules_tree(t_modules_, cur_module);
    bind_modules_tree(t_ped_modules_, cur_ped_module);

    trigg_tot_entries_ = t_trigger_->GetEntries();
    trigg_set_start();
    
    ped_trigg_tot_entries_ = t_ped_trigger_->GetEntries();
    ped_trigg_set_start();
    
    return true;
}

void EventIterator::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    t_file_in_ = NULL;
    t_trigger_ = NULL;
    t_modules_ = NULL;
    t_ped_trigger_ = NULL;
    t_ped_modules_ = NULL;
}

void EventIterator::trigg_set_start() {
    trigg_cur_index_ = -1;
    module_cur_index_ = -1;
    module_start_index_ = -1;
    module_end_index_ = -1;
    trigg_reach_end_ = false;
    trigg_is_bad_ = true;
    module_reach_end_ = false;
}

bool EventIterator::trigg_next() {
    if (t_file_in_ == NULL)
        return false;
    if (trigg_reach_end_)
        return false;
    do {
        trigg_cur_index_++;
        if (trigg_cur_index_ < trigg_tot_entries_) {
            t_trigger_->GetEntry(trigg_cur_index_);
        } else {
            trigg_reach_end_ = true;
            return false;
        }
    } while (cur_trigg.is_bad > 0);
    trigg_is_bad_ = false;
    module_start_index_ = cur_trigg.pkt_start;
    module_end_index_ = cur_trigg.pkt_start + cur_trigg.pkt_count;
    module_set_start();
    return true;
}

bool EventIterator::trigg_select(Long64_t entry_index) {
    if (t_file_in_ == NULL)
        return false;
    if (entry_index < 0 || entry_index >= trigg_tot_entries_)
        return false;
    t_trigger_->GetEntry(entry_index);
    trigg_cur_index_ = entry_index;
    if (cur_trigg.is_bad > 0) {
        trigg_is_bad_ = true;
        return false;
    } else {
        trigg_is_bad_ = false;
        module_start_index_ = cur_trigg.pkt_start;
        module_end_index_ = cur_trigg.pkt_start + cur_trigg.pkt_count;
        module_set_start();
        return true;
    }
}

void EventIterator::module_set_start() {
    module_cur_index_ = module_start_index_ - 1;
    if (cur_trigg.pkt_count < 1)
        module_reach_end_ = true;
    else
        module_reach_end_ = false;
}

bool EventIterator::module_next() {
    if (t_file_in_ == NULL)
        return false;
    if (trigg_cur_index_ < 0)
        return false;
    if (trigg_reach_end_)
        return false;
    if (trigg_is_bad_)
        return false;
    if (module_reach_end_)
        return false;
    module_cur_index_++;
    if (module_cur_index_ < module_end_index_) {
        t_modules_->GetEntry(module_cur_index_);
        return true;
    } else {
        module_reach_end_ = true;
        return false;
    }
}

void EventIterator::ped_trigg_set_start() {
    ped_trigg_cur_index_ = -1;
    ped_module_cur_index_ = -1;
    ped_module_start_index_ = -1;
    ped_module_end_index_ = -1;
    ped_trigg_reach_end_ = false;
    ped_trigg_is_bad_ = true;
    ped_module_reach_end_ = false;
}

bool EventIterator::ped_trigg_next() {
    if (t_file_in_ == NULL)
        return false;
    if (ped_trigg_reach_end_)
        return false;
    do {
        ped_trigg_cur_index_++;
        if (ped_trigg_cur_index_ < ped_trigg_tot_entries_) {
            t_ped_trigger_->GetEntry(ped_trigg_cur_index_);
        } else {
            ped_trigg_reach_end_ = true;
            return false;
        }
    } while (cur_ped_trigg.is_bad > 0);
    ped_trigg_is_bad_ = false;
    ped_module_start_index_ = cur_ped_trigg.pkt_start;
    ped_module_end_index_ = cur_ped_trigg.pkt_start + cur_ped_trigg.pkt_count;
    ped_module_set_start();
    return true;
}

bool EventIterator::ped_trigg_select(Long64_t entry_index) {
    if (t_file_in_ == NULL)
        return false;
    if (entry_index < 0 || entry_index >= ped_trigg_tot_entries_)
        return false;
    t_ped_trigger_->GetEntry(entry_index);
    ped_trigg_cur_index_ = entry_index;
    if (cur_ped_trigg.is_bad > 0) {
        ped_trigg_is_bad_ = true;
        return false;
    } else {
        ped_trigg_is_bad_ = false;
        ped_module_start_index_ = cur_ped_trigg.pkt_start;
        ped_module_end_index_ = cur_ped_trigg.pkt_start + cur_ped_trigg.pkt_count;
        ped_module_set_start();
        return true;
    }
}

void EventIterator::ped_module_set_start() {
    ped_module_cur_index_ = ped_module_start_index_ - 1;
    if (cur_ped_trigg.pkt_count < 1)
        ped_module_reach_end_ = true;
    else
        ped_module_reach_end_ = false;
}

bool EventIterator::ped_module_next() {
    if (t_file_in_ == NULL)
        return false;
    if (ped_trigg_cur_index_ < 0)
        return false;
    if (ped_trigg_reach_end_)
        return false;
    if (ped_trigg_is_bad_)
        return false;
    if (ped_module_reach_end_)
        return false;
    ped_module_cur_index_++;
    if (ped_module_cur_index_ < ped_module_end_index_) {
        t_ped_modules_->GetEntry(ped_module_cur_index_);
        return true;
    } else {
        ped_module_reach_end_ = true;
        return false;
    }
}
