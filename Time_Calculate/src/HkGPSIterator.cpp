#include "HkGPSIterator.hpp"

HkGPSIterator::HkGPSIterator() {
    t_file_in_ = NULL;
    t_hk_obox_ = NULL;
}

HkGPSIterator::~HkGPSIterator() {
    if (t_file_in_ != NULL)
        close();
}

bool HkGPSIterator::open(const char* filename) {
    if (t_file_in_ != NULL)
        return false;
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_hk_obox_ = static_cast<TTree*>(t_file_in_->Get("t_hk_obox"));
    if (t_hk_obox_ == NULL)
        return false;

    hk_obox_tot_entries_ = t_hk_obox_->GetEntries();
    
    t_hk_obox_->SetBranchAddress("timestamp_sync",      &b_timestamp_);
    t_hk_obox_->SetBranchAddress("gps_sync_send_count", &b_gps_count_);
    t_hk_obox_->SetBranchAddress("gps_sync_gen_count",  &b_gps_count_gen_);
    t_hk_obox_->SetBranchAddress("gps_pps_count",       &b_gps_count_pps_);
    t_hk_obox_->SetBranchAddress("obox_is_bad",         &b_obox_is_bad_);
    
    return true;
}

void HkGPSIterator::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    t_file_in_ = NULL;
    t_hk_obox_ = NULL;
}

bool HkGPSIterator::check_cur_valid_() {
    return !static_cast<bool>((b_gps_count_pps_ >> 15) & 0x1);
}

bool HkGPSIterator::initialize() {
    if (t_file_in_ == NULL)
        return false;
    if (hk_obox_tot_entries_ < 100)
        return false;
    
    hk_obox_cur_index_ = -1;
    hk_obox_reach_end_ = false;
    passed_last_ = false;
    cur_is_valid_ = false;
    
    if (set_first_()) {
        before_gps_sync = first_gps_sync;
        before_valid_ = first_valid_;
        after_gps_sync = first_gps_sync;
        after_valid_ = first_valid_;
        cur_ticks_per_second = 12500000.0;
        if (set_last_())
            return true;
        else
            return false;
    } else {
        return false;
    }
}

bool HkGPSIterator::set_first_() {
    if (t_file_in_ == NULL)
        return false;
    // skip zeros
    cur_is_valid_ = false;
    while (true) {
        hk_obox_cur_index_++;
        if (hk_obox_cur_index_ < hk_obox_tot_entries_) {
            t_hk_obox_->GetEntry(hk_obox_cur_index_);
            if ((b_gps_count_ != 0 || b_timestamp_ != 0) && b_obox_is_bad_ == 0) {
                cur_gps_sync_ = make_pair(GPSTime().update8(b_gps_count_), b_timestamp_);
                cur_is_valid_ = check_cur_valid_();
                break;
            }
        } else {
            hk_obox_reach_end_ = true;
            break;
        }
    }
    if (hk_obox_reach_end_)
        return false;
    // find first
    int repeat_count = 0;
    bool found = false;
    while (true) {
        repeat_count = 0;
        while (true) {
            if (abs(cur_gps_sync_.first - pre_gps_sync_.first) > 25 * 1.0E-9)
                cur_is_valid_ = check_cur_valid_();
            if (!next_pair_()) {
                break;
            }
            if (abs(cur_gps_sync_.first - pre_gps_sync_.first) < 25 * 1.0E-9
                && cur_gps_sync_.second == pre_gps_sync_.second) {
                repeat_count++;
            } else {
                break;
            }
        }
        if (repeat_count > 15) {
            found = true;
            break;
        } else {
            if (hk_obox_reach_end_) {
                break;
            }
        }
    }
    if (found) {
        first_gps_sync = pre_gps_sync_;
        first_valid_ = cur_is_valid_;
        return true;
    } else {
        return false;
    }
}

bool HkGPSIterator::set_last_() {
    if (t_file_in_ == NULL)
        return false;
    // skip zeros
    Long64_t bak_cur_index = hk_obox_tot_entries_;
    bool bak_reach_end = false;
    pair<GPSTime, uint32_t> bak_pre_gps_sync_;
    pair<GPSTime, uint32_t> bak_cur_gps_sync_;        
    while (true) {
        bak_cur_index--;
        if (bak_cur_index >= 0) {
            t_hk_obox_->GetEntry(bak_cur_index);
            if ((b_gps_count_ != 0 || b_timestamp_ != 0) && b_obox_is_bad_ == 0) {
                bak_cur_gps_sync_ = make_pair(GPSTime().update8(b_gps_count_), b_timestamp_);
                break;
            }
        } else {
            bak_reach_end = true;
            break;
        }
    }
    if (bak_reach_end)
        return false;
    // find last
    int repeat_count = 0;
    bool found = false;
    while (true) {
        repeat_count = 0;
        while (true) {
            while (true) {
                bak_cur_index--;
                if (bak_cur_index >= 0) {
                    t_hk_obox_->GetEntry(bak_cur_index);
                    if (b_obox_is_bad_ == 0) {
                        bak_pre_gps_sync_ = bak_cur_gps_sync_;
                        bak_cur_gps_sync_ = make_pair(GPSTime().update8(b_gps_count_), b_timestamp_);
                        break;
                    }
                } else {
                    bak_reach_end = true;
                    break;
                }
            }
            if (bak_reach_end)
                break;
            if (abs(bak_cur_gps_sync_.first - bak_pre_gps_sync_.first) < 25 * 1.0E-9
                && bak_cur_gps_sync_.second == bak_pre_gps_sync_.second) {
                repeat_count++;
            } else {
                break;
            }
        }
        if (repeat_count > 15) {
            found = true;
            break;
        } else {
            if (bak_reach_end)
                break;
        }
    }
    if (found) {
        last_gps_sync = bak_pre_gps_sync_;
        return true;
    } else {
        return false;
    }
}

bool HkGPSIterator::next_pair_() {
    if (t_file_in_ == NULL)
        return false;
    if (hk_obox_reach_end_)
        return false;

    while (true) {
        hk_obox_cur_index_++;
        if (hk_obox_cur_index_ < hk_obox_tot_entries_) {
            t_hk_obox_->GetEntry(hk_obox_cur_index_);
            if (b_obox_is_bad_ == 0) {
                pre_gps_sync_ = cur_gps_sync_;
                cur_gps_sync_ = make_pair(GPSTime().update8(b_gps_count_), b_timestamp_);
                return true;
            }
        } else {
            hk_obox_reach_end_ = true;
            return false;
        }
    }
}

bool HkGPSIterator::next_minute() {
    if (t_file_in_ == NULL)
        return false;
    if (hk_obox_reach_end_) {
        if (!passed_last_) {
            before_gps_sync = after_gps_sync;
            before_valid_ = after_valid_;
            cur_ticks_per_second = 12500000.0;
            passed_last_ = true;
        }
        return false;
    }
    int repeat_count = 0;
    bool found = false;
    cur_is_valid_ = false;
    while (true) {
        repeat_count = 0;
        while (true) {
            if (abs(cur_gps_sync_.first - pre_gps_sync_.first) > 25 * 1.0E-9)
                cur_is_valid_ = check_cur_valid_();
            if (!next_pair_()) {
                break;
            }
            if (abs(cur_gps_sync_.first - pre_gps_sync_.first) < 25 * 1.0E-9
                && cur_gps_sync_.second == pre_gps_sync_.second) {
                repeat_count++;
            } else {
                break;
            }
        }
        if (repeat_count > 15) {
            found = true;
            break;
        } else {
            if (hk_obox_reach_end_) {
                break;
            }
        }
    }
    if (found) {
        before_gps_sync = after_gps_sync;
        before_valid_ = after_valid_;
        after_gps_sync = pre_gps_sync_;
        after_valid_ = cur_is_valid_;
        double time_diff = static_cast<double>(after_gps_sync.second) - static_cast<double>(before_gps_sync.second);
        if (time_diff < 0)
            time_diff += 4294967296;
        cur_ticks_per_second = time_diff / (after_gps_sync.first - before_gps_sync.first);
        return true;
    } else {
        before_gps_sync = after_gps_sync;
        before_valid_ = after_valid_;
        cur_ticks_per_second = 12500000.0;
        passed_last_ = true;
        return false;
    }
}
