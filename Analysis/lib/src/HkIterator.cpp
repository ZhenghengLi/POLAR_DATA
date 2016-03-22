#include "HkIterator.hpp"

HkIterator::HkIterator() {
    t_file_in_ = NULL;
    t_hk_obox_ = NULL;
    t_hk_ibox_ = NULL;
}

HkIterator::~HkIterator() {
    if (t_file_in_ != NULL)
        close();
}

bool HkIterator::open(const char* filename) {
    if (t_file_in_ != NULL)
        return false;
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_hk_obox_ = static_cast<TTree*>(t_file_in_->Get("t_hk_obox"));
    if (t_hk_obox_ == NULL)
        return false;
    t_hk_ibox_ = static_cast<TTree*>(t_file_in_->Get("t_hk_ibox"));
    if (t_hk_ibox_ == NULL)
        return false;

    bind_hk_obox_tree(t_hk_obox_, hk_obox);
    bind_hk_ibox_tree(t_hk_ibox_, hk_ibox);

    hk_obox_tot_entries_ = t_hk_obox_->GetEntries();
    hk_obox_set_start();
    hk_ibox_tot_entries_ = t_hk_ibox_->GetEntries();
    hk_ibox_set_start();

    return true;
}

void HkIterator::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    t_file_in_ = NULL;
    t_hk_obox_ = NULL;
    t_hk_ibox_ = NULL;
}

void HkIterator::hk_obox_set_start() {
    hk_obox_cur_index_ = -1;
    hk_obox_reach_end_ = false;
}

bool HkIterator::hk_obox_next() {
    if (t_file_in_ == NULL)
        return false;
    if (hk_obox_reach_end_)
        return false;
    hk_obox_cur_index_++;
    if (hk_obox_cur_index_ < hk_obox_tot_entries_) {
        t_hk_obox_->GetEntry(hk_obox_cur_index_);
        return true;
    } else {
        hk_obox_reach_end_ = true;
        return false;
    }
}

bool HkIterator::hk_obox_select(Long64_t entry_index) {
    if (t_file_in_ == NULL)
        return false;
    if (entry_index < 0 || entry_index >= hk_obox_tot_entries_)
        return false;
    t_hk_obox_->GetEntry(entry_index);
    hk_obox_cur_index_ = entry_index;
    return true;
}

void HkIterator::hk_ibox_set_start() {
    hk_ibox_cur_index_ = -1;
    hk_ibox_reach_end_ = false;
}

bool HkIterator::hk_ibox_next() {
    if (t_file_in_ == NULL)
        return false;
    if (hk_ibox_reach_end_)
        return false;
    hk_ibox_cur_index_++;
    if (hk_ibox_cur_index_ < hk_ibox_tot_entries_) {
        t_hk_ibox_->GetEntry(hk_ibox_cur_index_);
        return true;
    } else {
        hk_ibox_reach_end_ = true;
        return false;
    }
}

bool HkIterator::hk_ibox_select(Long64_t entry_index) {
    if (t_file_in_ == NULL)
        return false;
    t_hk_ibox_->GetEntry(entry_index);
    hk_ibox_cur_index_ = entry_index;
    return true;
}
