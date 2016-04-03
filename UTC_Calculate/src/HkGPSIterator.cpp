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
    if (hk_obox_tot_entries_ < 30)
        return false;
    
    t_hk_obox_->SetBranchAddress("timestamp_sync",      &b_timestamp_);
    t_hk_obox_->SetBranchAddress("gps_sync_send_count", &b_gps_count_);
    
    return true;
}

void HkGPSIterator::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    t_file_in_ = NULL;
    t_hk_obox_ = NULL;
}

void HkGPSIterator::set_start() {
    start_flag_ = true;
    hk_obox_cur_index_ = -1;
    hk_obox_reach_end_ = false;
}

bool HkGPSIterator::next_pair() {
    if (t_file_in_ == NULL)
        return false;
    if (hk_obox_reach_end_)
        return false;

    hk_obox_cur_index_++;
    if (hk_obox_cur_index_ < hk_obox_tot_entries_) {
        t_hk_obox_->GetEntry(hk_obox_cur_index_);
        cur_gps_sync_ = make_pair(GPSTime().update8(b_gps_count_), b_timestamp_);
        return true;
    } else {
        hk_obox_reach_end_ = true;
        return false;
    }
}

void HkGPSIterator::print_pair() {
    cout << left
         << setw(6) << "week:"
         << setw(10) << cur_gps_sync_.first.week
         << setw(8) << "second:"
         << setw(20) << cur_gps_sync_.first.second
         << setw(11) << "timestamp:"
         << setw(20) << cur_gps_sync_.second
         << right
         << endl;
}
