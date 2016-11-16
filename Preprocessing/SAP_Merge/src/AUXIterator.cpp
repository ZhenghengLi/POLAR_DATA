#include "AUXIterator.hpp"

using namespace std;

AUXIterator::AUXIterator() {
    t_file_in_ = NULL;
    t_hk_obox_tree_ = NULL;

    re_ship_span_ = "^ *(\\d+)\\[\\d+\\] => (\\d+)\\[\\d+\\]; \\d+/\\d+ *$";
}

AUXIterator::~AUXIterator() {
    if (t_file_in_ != NULL)
        close();
}

bool AUXIterator::open(const char* filename) {
    if (t_file_in_ != NULL)
        return false;
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_hk_obox_tree_ = static_cast<TTree*>(t_file_in_->Get("t_hk_obox"));
    if (t_hk_obox_tree_ == NULL)
        return false;

    // get first and last ship_second
    TNamed* m_oboxship = static_cast<TNamed*>(t_file_in_->Get("m_oboxship"));
    if (m_oboxship == NULL)
        return false;
    cmatch cm;
    if (regex_match(m_oboxship->GetTitle(), cm, re_ship_span_)) {
        first_ship_second_ = TString(cm[1]).Atof();
        last_ship_second_  = TString(cm[2]).Atof();
    } else {
        cerr << "ship time span match failed: " << m_oboxship->GetTitle() << endl;
        return false;
    }
    
    t_hk_obox_tree_->SetBranchAddress("packet_num",         &cur_hk_obox_.packet_num       );
    t_hk_obox_tree_->SetBranchAddress("abs_ship_second",    &cur_hk_obox_.abs_ship_second  );
    t_hk_obox_tree_->SetBranchAddress("obox_is_bad",        &cur_hk_obox_.obox_is_bad      );
    t_hk_obox_tree_->SetBranchAddress("fe_hv",               cur_hk_obox_.fe_hv            );
    t_hk_obox_tree_->SetBranchAddress("fe_thr",              cur_hk_obox_.fe_thr           );
    t_hk_obox_tree_->SetBranchAddress("fe_temp",             cur_hk_obox_.fe_temp          );

    // read the first hk_obox
     hk_obox_cur_entry_ = -1;
    do {
        hk_obox_cur_entry_++;
        if (hk_obox_cur_entry_ < t_hk_obox_tree_->GetEntries()) {
            t_hk_obox_tree_->GetEntry(hk_obox_cur_entry_);
        } else {
            return false;
        }
    } while (cur_hk_obox_.obox_is_bad > 0);
    hk_obox_before = cur_hk_obox_;
    hk_obox_after  = cur_hk_obox_;

    // find the first thr
    Long64_t tmp_entry = hk_obox_cur_entry_ -1;
    while (true) {
        tmp_entry++;
        if (tmp_entry < t_hk_obox_tree_->GetEntries()) {
            t_hk_obox_tree_->GetEntry(tmp_entry);
        } else {
            return false;
        }
        if (cur_hk_obox_.obox_is_bad == 0 && cur_hk_obox_.packet_num % 4 == 0) {
            for (int i = 0; i < 25; i++) {
                fe_thr_current[i] = cur_hk_obox_.fe_thr[i];
                fe_thr_next[i]    = cur_hk_obox_.fe_thr[i];
            }
            fe_thr_ship_second_current = cur_hk_obox_.abs_ship_second;
            fe_thr_ship_second_next    = cur_hk_obox_.abs_ship_second;
            break;
        }
    }

    hk_obox_reach_end_ = false;
    
    return true;
}

void AUXIterator::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    t_file_in_ = NULL;
    t_hk_obox_tree_ = NULL;
}

bool AUXIterator::next_obox() {
    if (t_file_in_ == NULL)
        return false;
    if (hk_obox_reach_end_)
        return false;
    do {
        hk_obox_cur_entry_++;
        if (hk_obox_cur_entry_ < t_hk_obox_tree_->GetEntries()) {
            t_hk_obox_tree_->GetEntry(hk_obox_cur_entry_);
        } else {
            hk_obox_reach_end_ = true;
            return false;
        }
    } while(cur_hk_obox_.obox_is_bad > 0);
    hk_obox_before = hk_obox_after;
    hk_obox_after  = cur_hk_obox_;
    // find next thr
    if (cur_hk_obox_.packet_num % 4 == 0) {
        for (int i = 0; i < 25; i++) {
            fe_thr_current[i] = cur_hk_obox_.fe_thr[i];
        }
        fe_thr_ship_second_current = cur_hk_obox_.abs_ship_second;
        Long64_t tmp_entry = hk_obox_cur_entry_;
        while (true) {
            tmp_entry++;
            if (tmp_entry < t_hk_obox_tree_->GetEntries()) {
                t_hk_obox_tree_->GetEntry(tmp_entry);
            } else {
                break;
            }
            if (cur_hk_obox_.obox_is_bad == 0 && cur_hk_obox_.packet_num % 4 == 0) {
                for (int i = 0; i < 25; i++) {
                    fe_thr_next[i]    = cur_hk_obox_.fe_thr[i];
                }
                fe_thr_ship_second_next    = cur_hk_obox_.abs_ship_second;
                break;
            }
        }
    }
    return true;
}

bool AUXIterator::get_reach_end() {
    return hk_obox_reach_end_;
}

double AUXIterator::get_first_ship_second() {
    return first_ship_second_;
}

double AUXIterator::get_last_ship_second() {
    return last_ship_second_;
}
