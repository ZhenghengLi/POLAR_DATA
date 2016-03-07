#include "ModuleIterator.hpp"

ModuleIterator::ModuleIterator() {
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
    bind_trigger_tree(t_ped_trigger_, ped_trigg_);
    bind_modules_tree(t_modules_, phy_event_);
    bind_modules_tree(t_ped_modules_, ped_event_);

    ped_trigg_tot_entries_ = t_ped_trigger_->GetEntries();
    ped_event_tot_entries_ = t_ped_modules_->GetEntries();
    phy_trigg_tot_entries_ = t_trigger_->GetEntries();
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
    if (t_file_in_ == NULL)
        return false;
    if (cur_ct_num_ > 0)
        return false;
    if (ped_trigg_reach_end_)
        return false;
    bool ret_value;
    while (true) {
        ped_trigg_cur_entry_++;
        if (ped_trigg_cur_entry_ < ped_trigg_tot_entries_) {
            t_ped_trigger_->GetEntry(ped_trigg_cur_entry_);
            if (ped_trigg_.is_bad <= 0) {
                ret_value = true;
                break;
            }
        } else {
            ped_trigg_reach_end_ = true;
            ret_value = false;
            break;
        }
    }
    return ret_value;
}

bool ModuleIterator::phy_trigg_next_() {
    if (t_file_in_ == NULL)
        return false;
    if (cur_ct_num_ > 0)
        return false;
    if (phy_trigg_reach_end_)
        return false;
    bool ret_value;
    while (true) {
        phy_trigg_cur_entry_++;
        if (phy_trigg_cur_entry_ < phy_trigg_tot_entries_) {
            t_trigger_->GetEntry(phy_trigg_cur_entry_);
            if (phy_trigg_.is_bad <= 0) {
                ret_value = true;
                break;
            }
        } else {
            phy_trigg_reach_end_ = true;
            ret_value = false;
            break;
        }
    }
    return ret_value;
}

bool ModuleIterator::ped_event_next_() {
    if (t_file_in_ == NULL)
        return false;
    if (cur_ct_num_ < 1 || cur_ct_num_ > 25)
        return false;
    if (ped_event_reach_end_)
        return false;
    bool ret_value;
    while (true) {
        ped_event_cur_entry_++;
        if (ped_event_cur_entry_ < ped_event_tot_entries_) {
            t_ped_modules_->GetEntry(ped_event_cur_entry_);
            if (ped_event_.is_bad <= 0 && ped_event_.ct_num == cur_ct_num_) {
                ret_value = true;
                break;
            }
        } else {
            ped_event_reach_end_ = true;
            ret_value = false;
            break;
        }
    }
    return ret_value;
}

bool ModuleIterator::phy_event_next_() {
    if (t_file_in_ == NULL)
        return false;
    if (cur_ct_num_ < 1 || cur_ct_num_ > 25)
        return false;
    if (phy_event_reach_end_)
        return false;
    bool ret_value;
    while (true) {
        phy_event_cur_entry_++;
        if (phy_event_cur_entry_ < phy_event_tot_entries_) {
            t_modules_->GetEntry(phy_event_cur_entry_);
            if (phy_event_.is_bad <= 0 && phy_event_.ct_num == cur_ct_num_) {
                ret_value = true;
                break;
            }
        } else {
            phy_event_reach_end_ = true;
            ret_value = false;
            break;
        }
    }
    return ret_value;
}

bool ModuleIterator::trigg_next() {
    if (ped_trigg_reach_end_ && phy_trigg_reach_end_)
        return false;
    bool ped_selected;
    if (ped_trigg_reach_end_) {
        cur_trigg = phy_trigg_;
        ped_selected = false;
    } else if (phy_trigg_reach_end_) {
        cur_trigg = ped_trigg_;
        ped_selected = true;
    } else {
        if (ped_trigg_.trigg_num_g < phy_trigg_.trigg_num_g) {
            cur_trigg = ped_trigg_;
            ped_selected = true;
        } else {
            cur_trigg = phy_trigg_;
            ped_selected = false;
        }
    }
    if (trigg_start_flag_) {
        trigg_start_flag_ = false;
        pre_seq_num_ = cur_trigg.trigg_num_g - 1;
        cur_seq_num_ = cur_trigg.trigg_num_g;
    } else {
        pre_seq_num_ = cur_seq_num_;
        cur_seq_num_ = cur_trigg.trigg_num_g;
    }
    if (ped_selected) {
        ped_trigg_next_();
    } else {
        phy_trigg_next_();
    }
    return true;
}

bool ModuleIterator::event_next() {
    if (ped_event_reach_end_ && phy_event_reach_end_)
        return false;
    bool ped_selected;
    if (ped_event_reach_end_) {
        cur_event = phy_event_;
        ped_selected = false;
    } else if (phy_event_reach_end_) {
        cur_event = ped_event_;
        ped_selected = true;
    } else {
        if (ped_event_.event_num_g < phy_event_.event_num_g) {
            cur_event = ped_event_;
            ped_selected = true;
        } else {
            cur_event = phy_event_;
            ped_selected = false;
        }
    }
    if (event_start_flag_) {
        pre_seq_num_ = cur_event.event_num_g - 1;
        cur_seq_num_ = cur_event.event_num_g;
    } else {
        pre_seq_num_ = cur_seq_num_;
        cur_seq_num_ = cur_event.event_num_g;
    }
    if (ped_selected) {
        ped_event_next_();
    } else {
        phy_event_next_();
    }
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
    if (t_file_in_ == NULL)
        return false;
    bool ped_res, phy_res;
    if (cur_ct_num_ > 0) {
        event_start_flag_ = true;
        ped_event_cur_entry_ = -1;
        ped_event_reach_end_ = false;
        ped_res = ped_event_next_();
        phy_event_cur_entry_ = -1;
        phy_event_reach_end_ = false;
        phy_res = phy_event_next_();
    } else {
        trigg_start_flag_ = true;
        ped_trigg_cur_entry_ = -1;
        ped_trigg_reach_end_ = false;
        ped_res = ped_trigg_next_();
        phy_trigg_cur_entry_ = -1;
        phy_trigg_reach_end_ = false;
        phy_res = phy_trigg_next_();
    }
    return (ped_res || phy_res);    
}

int ModuleIterator::get_cur_seq() {
    return cur_seq_num_;
}

int ModuleIterator::get_bad_cnt() {
    return cur_seq_num_ - pre_seq_num_ - 1;
}
