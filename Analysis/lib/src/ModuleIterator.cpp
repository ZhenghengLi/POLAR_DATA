#include "ModuleIterator.hpp"

using namespace std;

ModuleIterator::ModuleIterator() {
    t_file_in_ = NULL;
    t_trigger_ = NULL;
    t_modules_ = NULL;
    t_ped_trigger_ = NULL;
    t_ped_modules_ = NULL;

    ped_trigg_elist_ = NULL;
    ped_event_elist_ = NULL;
    phy_trigg_elist_ = NULL;
    phy_event_elist_ = NULL;
}

ModuleIterator::~ModuleIterator() {
    if (t_file_in_ != NULL)
        close();
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
    
    ped_trigg_elist_ = NULL;
    ped_event_elist_ = NULL;
    phy_trigg_elist_ = NULL;
    phy_event_elist_ = NULL;
}

bool ModuleIterator::ped_trigg_next_() {
    if (t_file_in_ == NULL)
        return false;
    if (cur_ct_num_ > 0)
        return false;
    if (get_tot_N() < 1)
        return false;
    if (ped_trigg_reach_end_)
        return false;
    ped_trigg_cur_index_++;
    if (ped_trigg_cur_index_ < ped_trigg_elist_->GetN()) {
        t_ped_trigger_->GetEntry(ped_trigg_elist_->GetEntry(ped_trigg_cur_index_));
        return true;
    } else {
        ped_trigg_reach_end_ = true;
        return false;
    }
}

bool ModuleIterator::phy_trigg_next_() {
    if (t_file_in_ == NULL)
        return false;
    if (cur_ct_num_ > 0)
        return false;
    if (get_tot_N() < 1)
        return false;
    if (phy_trigg_reach_end_)
        return false;
    phy_trigg_cur_index_++;
    if (phy_trigg_cur_index_ < phy_trigg_elist_->GetN()) {
        t_trigger_->GetEntry(phy_trigg_elist_->GetEntry(phy_trigg_cur_index_));
        return true;
    } else {
        phy_trigg_reach_end_ = true;
        return false;
    }
}

bool ModuleIterator::ped_event_next_() {
    if (t_file_in_ == NULL)
        return false;
    if (cur_ct_num_ < 1 || cur_ct_num_ > 25)
        return false;
    if (get_tot_N() < 1)
        return false;
    if (ped_event_reach_end_)
        return false;
    ped_event_cur_index_++;
    if (ped_event_cur_index_ < ped_event_elist_->GetN()) {
        t_ped_modules_->GetEntry(ped_event_elist_->GetEntry(ped_event_cur_index_));
        return true;
    } else {
        ped_event_reach_end_ = true;
        return false;
    }
}

bool ModuleIterator::phy_event_next_() {
    if (t_file_in_ == NULL)
        return false;
    if (cur_ct_num_ < 1 || cur_ct_num_ > 25)
        return false;
    if (get_tot_N() < 1)
        return false;
    if (phy_event_reach_end_)
        return false;
    phy_event_cur_index_++;
    if (phy_event_cur_index_ < phy_event_elist_->GetN()) {
        t_modules_->GetEntry(phy_event_elist_->GetEntry(phy_event_cur_index_));
        return true;
    } else {
        phy_event_reach_end_ = true;
        return false;
    }
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

Int_t ModuleIterator::get_tot_N() {
    if (cur_ct_num_ > 0) {
        if (phy_event_elist_ == NULL || ped_event_elist_ == NULL)
            return 0;
        else
            return (phy_event_elist_->GetN() + ped_event_elist_->GetN());
    } else {
        if (phy_trigg_elist_ == NULL || ped_trigg_elist_ == NULL)
            return 0;
        else
            return (phy_trigg_elist_->GetN() + ped_trigg_elist_->GetN());
    }

}

bool ModuleIterator::set_module(int ct_num, string filter) {
    if (t_file_in_ == NULL)
        return false;
    if (ct_num < 1 || ct_num > 25) {
        cur_ct_num_ = 1;
    } else {
        cur_ct_num_ = ct_num;
    }

    char buffer[80];
    sprintf(buffer, "is_bad <= 0 && ct_num == %d", cur_ct_num_);
    cur_filter_.assign(buffer);
    if (filter.length() > 0) {
        cur_filter_ += " && (";
        cur_filter_ += filter;
        cur_filter_ += ")";
    }
    if (t_modules_->Draw(">>phy_event_elist", cur_filter_.c_str()) < 0)
        return false;
    phy_event_elist_ = static_cast<TEventList*>(gDirectory->Get("phy_event_elist"));
    t_modules_->SetEventList(phy_event_elist_);
    if (t_ped_modules_->Draw(">>ped_event_elist", cur_filter_.c_str()) < 0)
        return false;
    ped_event_elist_ = static_cast<TEventList*>(gDirectory->Get("ped_event_elist"));
    t_ped_modules_->SetEventList(ped_event_elist_);

    if (get_tot_N() < 1)
        return false;

    if (phy_trigg_elist_ != NULL) {
        delete phy_trigg_elist_;
        phy_trigg_elist_ = NULL;
    }
    if (ped_trigg_elist_ != NULL) {
        delete ped_trigg_elist_;
        ped_trigg_elist_ = NULL;
    }
    phy_trigg_reach_end_ = true;
    ped_trigg_reach_end_ = true;

    set_start();
    return true;
}

bool ModuleIterator::set_trigger(string filter) {
    if (t_file_in_ == NULL)
        return false;
    cur_ct_num_ = 0;

    cur_filter_.assign("is_bad <= 0");
    if (filter.length() > 0) {
        cur_filter_ += " && (";
        cur_filter_ += filter;
        cur_filter_ += ")";
    }
    if (t_trigger_->Draw(">>phy_trigg_elist", cur_filter_.c_str()) < 0)
        return false;
    phy_trigg_elist_ = static_cast<TEventList*>(gDirectory->Get("phy_trigg_elist"));
    t_trigger_->SetEventList(phy_trigg_elist_);
    if (t_ped_trigger_->Draw(">>ped_trigg_elist", cur_filter_.c_str()) < 0)
        return false;
    ped_trigg_elist_ = static_cast<TEventList*>(gDirectory->Get("ped_trigg_elist"));
    t_ped_trigger_->SetEventList(ped_trigg_elist_);

    if (get_tot_N() < 1)
        return false;

    if (phy_event_elist_ != NULL) {
        delete phy_event_elist_;
        phy_event_elist_ = NULL;
    }
    if (ped_event_elist_ != NULL) {
        delete ped_event_elist_;
        ped_event_elist_ = NULL;
    }
    phy_event_reach_end_ = true;
    ped_event_reach_end_ = true;
    
    set_start();
    return true;
}

void ModuleIterator::set_start() {
    if (t_file_in_ == NULL)
        return;
    if (get_tot_N() < 1)
        return;
    if (cur_ct_num_ > 0) {
        event_start_flag_ = true;
        ped_event_cur_index_ = -1;
        ped_event_reach_end_ = false;
        ped_event_next_();
        phy_event_cur_index_ = -1;
        phy_event_reach_end_ = false;
        phy_event_next_();
    } else {
        trigg_start_flag_ = true;
        ped_trigg_cur_index_ = -1;
        ped_trigg_reach_end_ = false;
        ped_trigg_next_();
        phy_trigg_cur_index_ = -1;
        phy_trigg_reach_end_ = false;
        phy_trigg_next_();
    }
}

int ModuleIterator::get_cur_seq() {
    return cur_seq_num_;
}

int ModuleIterator::get_bad_cnt() {
    if (cur_filter_.find("(") != string::npos)
        return -1;
    else
        return (cur_seq_num_ - pre_seq_num_ - 1);
}

SciType::Modules_T ModuleIterator::get_first_event() {
    Modules_T ret_event;
    if (t_file_in_ == NULL)
        return ret_event;
    if (get_tot_N() < 1)
        return ret_event;

    Modules_T tmp_phy_event = phy_event_;
    Modules_T tmp_ped_event = ped_event_;

    if (ped_event_elist_->GetN() < 1) {
        t_modules_->GetEntry(phy_event_elist_->GetEntry(0));
        ret_event = phy_event_;
    } else if (phy_event_elist_->GetN() < 1) {
        t_ped_modules_->GetEntry(ped_event_elist_->GetEntry(0));
        ret_event = ped_event_;
    } else {
        t_modules_->GetEntry(phy_event_elist_->GetEntry(0));
        t_ped_modules_->GetEntry(ped_event_elist_->GetEntry(0));
        if (phy_event_.event_num_g < ped_event_.event_num_g) {
            ret_event = phy_event_;
        } else {
            ret_event = ped_event_;
        }
    }

    phy_event_ = tmp_phy_event;
    ped_event_ = tmp_ped_event;
    return ret_event;
}

SciType::Modules_T ModuleIterator::get_last_event() {
    Modules_T ret_event;
    if (t_file_in_ == NULL)
        return ret_event;
    if (get_tot_N() < 1)
        return ret_event;
    
    Modules_T tmp_phy_event = phy_event_;
    Modules_T tmp_ped_event = ped_event_;

    if (ped_event_elist_->GetN() < 1) {
        t_modules_->GetEntry(phy_event_elist_->GetEntry(phy_event_elist_->GetN() - 1));
        ret_event = phy_event_;
    } else if (phy_event_elist_->GetN() < 1) {
        t_ped_modules_->GetEntry(ped_event_elist_->GetEntry(ped_event_elist_->GetN() - 1));
        ret_event = ped_event_;
    } else {
        t_modules_->GetEntry(phy_event_elist_->GetEntry(phy_event_elist_->GetN() - 1));
        t_ped_modules_->GetEntry(ped_event_elist_->GetEntry(ped_event_elist_->GetN() - 1));
        if (phy_event_.event_num_g > ped_event_.event_num_g) {
            ret_event = phy_event_;
        } else {
            ret_event = ped_event_;
        }
    }

    phy_event_ = tmp_phy_event;
    ped_event_ = tmp_ped_event;
    return ret_event;
}

SciType::Trigger_T ModuleIterator::get_first_trigg() {
    Trigger_T ret_trigg;
    if (t_file_in_ == NULL)
        return ret_trigg;
    if (get_tot_N() < 1)
        return ret_trigg;

    Trigger_T tmp_phy_trigg = phy_trigg_;
    Trigger_T tmp_ped_trigg = ped_trigg_;

    if (ped_trigg_elist_->GetN() < 1) {
        t_trigger_->GetEntry(phy_trigg_elist_->GetEntry(0));
        ret_trigg = phy_trigg_;
    } else if (phy_trigg_elist_->GetN() < 1) {
        t_ped_trigger_->GetEntry(ped_trigg_elist_->GetEntry(0));
        ret_trigg = ped_trigg_;
    } else {
        t_trigger_->GetEntry(phy_trigg_elist_->GetEntry(0));
        t_ped_trigger_->GetEntry(ped_trigg_elist_->GetEntry(0));
        if (phy_trigg_.trigg_num_g < ped_trigg_.trigg_num_g) {
            ret_trigg = phy_trigg_;
        } else {
            ret_trigg = ped_trigg_;
        }
    }

    phy_trigg_ = tmp_phy_trigg;
    ped_trigg_ = tmp_ped_trigg;
    return ret_trigg;
}

SciType::Trigger_T ModuleIterator::get_last_trigg() {
    Trigger_T ret_trigg;
    if (t_file_in_ == NULL)
        return ret_trigg;
    if (get_tot_N() < 1)
        return ret_trigg;

    Trigger_T tmp_phy_trigg = phy_trigg_;
    Trigger_T tmp_ped_trigg = ped_trigg_;

    if (ped_trigg_elist_->GetN() < 1) {
        t_trigger_->GetEntry(phy_trigg_elist_->GetEntry(phy_trigg_elist_->GetN() - 1));
        ret_trigg = phy_trigg_;
    } else if (phy_trigg_elist_->GetN() < 1) {
        t_ped_trigger_->GetEntry(ped_trigg_elist_->GetEntry(ped_trigg_elist_->GetN() - 1));
        ret_trigg = ped_trigg_;
    } else {
        t_trigger_->GetEntry(phy_trigg_elist_->GetEntry(phy_trigg_elist_->GetN() - 1));
        t_ped_trigger_->GetEntry(ped_trigg_elist_->GetEntry(ped_trigg_elist_->GetN() - 1));
        if (phy_trigg_.trigg_num_g > ped_trigg_.trigg_num_g) {
            ret_trigg = phy_trigg_;
        } else {
            ret_trigg = ped_trigg_;
        }
    }

    phy_trigg_ = tmp_phy_trigg;
    ped_trigg_ = tmp_ped_trigg;
    return ret_trigg;
}

int ModuleIterator::get_cur_ct_num() {
    return cur_ct_num_;
}

string ModuleIterator::get_cur_filter() {
    return cur_filter_;
}

bool ModuleIterator::cur_is_ped() {
    if (cur_ct_num_ > 0)
        return (cur_event.compress == 2);
    else
        return (cur_trigg.type == 0x00F0);
}

TTree* ModuleIterator::get_trigger_tree() {
    return t_trigger_;
}

TTree* ModuleIterator::get_modules_tree() {
    return t_modules_;
}

TTree* ModuleIterator::get_ped_trigger_tree() {
    return t_ped_trigger_;
}

TTree* ModuleIterator::get_ped_modules_tree() {
    return t_ped_modules_;
}
