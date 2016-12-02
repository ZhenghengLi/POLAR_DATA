#include "AUXIterator.hpp"

using namespace std;

AUXIterator::AUXIterator() {
    t_file_in_ = NULL;
    t_hk_obox_tree_ = NULL;

    re_gps_span_ = "^ *(\\d+):(\\d+)\\[\\d+\\] => (\\d+):(\\d+)\\[\\d+\\]; \\d+/\\d+ *$";
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

    // get first and last gps time
    TNamed* m_obox_gps = static_cast<TNamed*>(t_file_in_->Get("m_obox_gps"));
    if (m_obox_gps == NULL)
        return false;
    cmatch cm;
    if (regex_match(m_obox_gps->GetTitle(), cm, re_gps_span_)) {
        first_gps_time_ = TString(cm[1]).Atof() * 604800 + TString(cm[2]).Atof();
        last_gps_time_  = TString(cm[3]).Atof() * 604800 + TString(cm[4]).Atof();
    } else {
        cerr << "gps time span match failed: " << m_obox_gps->GetTitle() << endl;
        return false;
    }

    t_hk_obox_tree_->SetBranchAddress("packet_num",         &cur_hk_obox_.packet_num       );
    t_hk_obox_tree_->SetBranchAddress("abs_gps_week",       &cur_hk_obox_.abs_gps_week     );
    t_hk_obox_tree_->SetBranchAddress("abs_gps_second",     &cur_hk_obox_.abs_gps_second   );
    t_hk_obox_tree_->SetBranchAddress("obox_is_bad",        &cur_hk_obox_.obox_is_bad      );
    t_hk_obox_tree_->SetBranchAddress("obox_mode",          &cur_hk_obox_.obox_mode        );
    t_hk_obox_tree_->SetBranchAddress("fe_hv",               cur_hk_obox_.fe_hv            );
    t_hk_obox_tree_->SetBranchAddress("fe_thr",              cur_hk_obox_.fe_thr           );
    t_hk_obox_tree_->SetBranchAddress("fe_temp",             cur_hk_obox_.fe_temp          );

    // read the first hk_obox
     hk_obox_cur_entry_ = -1;
    do {
        hk_obox_cur_entry_++;
        if (hk_obox_cur_entry_ < t_hk_obox_tree_->GetEntries()) {
            t_hk_obox_tree_->GetEntry(hk_obox_cur_entry_);
            cur_hk_obox_.gps_time_sec = cur_hk_obox_.abs_gps_week * 604800 + cur_hk_obox_.abs_gps_second;
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
            cur_hk_obox_.gps_time_sec = cur_hk_obox_.abs_gps_week * 604800 + cur_hk_obox_.abs_gps_second;
        } else {
            return false;
        }
        if (cur_hk_obox_.obox_is_bad == 0 && cur_hk_obox_.packet_num % 4 == 0) {
            for (int i = 0; i < 25; i++) {
                fe_thr_current[i] = cur_hk_obox_.fe_thr[i];
                fe_thr_next[i]    = cur_hk_obox_.fe_thr[i];
            }
            fe_thr_gps_time_current = cur_hk_obox_.gps_time_sec;
            fe_thr_gps_time_next    = cur_hk_obox_.gps_time_sec;
            break;
        }
    }

    hk_obox_reach_end_ = false;
    next_obox();

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
            cur_hk_obox_.gps_time_sec = cur_hk_obox_.abs_gps_week * 604800 + cur_hk_obox_.abs_gps_second;
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
        fe_thr_gps_time_current = cur_hk_obox_.gps_time_sec;
        Long64_t tmp_entry = hk_obox_cur_entry_;
        while (true) {
            tmp_entry++;
            if (tmp_entry < t_hk_obox_tree_->GetEntries()) {
                t_hk_obox_tree_->GetEntry(tmp_entry);
                cur_hk_obox_.gps_time_sec = cur_hk_obox_.abs_gps_week * 604800 + cur_hk_obox_.abs_gps_second;
            } else {
                break;
            }
            if (cur_hk_obox_.obox_is_bad == 0 && cur_hk_obox_.packet_num % 4 == 0) {
                for (int i = 0; i < 25; i++) {
                    fe_thr_next[i]      = cur_hk_obox_.fe_thr[i];
                }
                fe_thr_gps_time_next    = cur_hk_obox_.gps_time_sec;
                break;
            }
        }
    }
    return true;
}

bool AUXIterator::get_reach_end() {
    return hk_obox_reach_end_;
}

double AUXIterator::get_first_gps_time() {
    return first_gps_time_;
}

double AUXIterator::get_last_gps_time() {
    return last_gps_time_;
}
