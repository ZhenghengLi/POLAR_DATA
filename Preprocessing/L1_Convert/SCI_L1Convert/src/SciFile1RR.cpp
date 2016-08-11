#include "SciFile1RR.hpp"

using namespace std;

SciFile1RR::SciFile1RR() {
    t_file_in_ = NULL;
    t_triggerl1_tree_ = NULL;
    t_modulesl1_tree_ = NULL;
}

SciFile1RR::~SciFile1RR() {
    close();
}

bool SciFile1RR::open(const char* filename) {
    if (t_file_in_ != NULL)
        return false;
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_triggerl1_tree_ = static_cast<TTree*>(t_file_in_->Get("t_trigger"));
    if (t_triggerl1_tree_ == NULL)
        return false;
    if (t_triggerl1_tree_->FindBranch("abs_gps_week") == NULL ||
        t_triggerl1_tree_->FindBranch("abs_gps_second") == NULL ||
        t_triggerl1_tree_->FindBranch("abs_gps_valid") == NULL) {
        cerr << "File: " << filename << " may be not a 1P/1R level SCI data file." << endl;
        return false;
    }
    t_modulesl1_tree_ = static_cast<TTree*>(t_file_in_->Get("t_modules"));
    if (t_modulesl1_tree_ == NULL)
        return false;
    if (t_triggerl1_tree_->GetEntries() < 1 ||
        t_modulesl1_tree_->GetEntries() < 1) {
        cerr << "File: " << filename << " may be empty." << endl;
        return false;
    }

    // bind t_triggerl1_tree_
    t_triggerl1_tree_->SetBranchAddress("is_bad",            &t_triggerl1.is_bad             );
    t_triggerl1_tree_->SetBranchAddress("trigg_num",         &t_triggerl1.trigg_num          );
    t_triggerl1_tree_->SetBranchAddress("abs_gps_week",      &t_triggerl1.abs_gps_week       );
    t_triggerl1_tree_->SetBranchAddress("abs_gps_second",    &t_triggerl1.abs_gps_second     );
    t_triggerl1_tree_->SetBranchAddress("abs_gps_valid",     &t_triggerl1.abs_gps_valid      );
    t_triggerl1_tree_->SetBranchAddress("abs_ship_second",   &t_triggerl1.abs_ship_second    );
    t_triggerl1_tree_->SetBranchAddress("type",              &t_triggerl1.type               );
    t_triggerl1_tree_->SetBranchAddress("trig_accepted",      t_triggerl1.trig_accepted      );
    t_triggerl1_tree_->SetBranchAddress("pkt_start",         &t_triggerl1.pkt_start          );
    t_triggerl1_tree_->SetBranchAddress("pkt_count",         &t_triggerl1.pkt_count          );
    t_triggerl1_tree_->SetBranchAddress("lost_count",        &t_triggerl1.lost_count         );
    t_triggerl1_tree_->SetBranchAddress("trigger_n",         &t_triggerl1.trigger_n          );

    // bind t_modulesl1_tree_
    t_modulesl1_tree_->SetBranchAddress("trigg_num",         &t_modulesl1.trigg_num          );
    t_modulesl1_tree_->SetBranchAddress("event_num",         &t_modulesl1.event_num          );
    t_modulesl1_tree_->SetBranchAddress("ct_num",            &t_modulesl1.ct_num             );
    t_modulesl1_tree_->SetBranchAddress("trigger_bit",        t_modulesl1.trigger_bit        );
    t_modulesl1_tree_->SetBranchAddress("multiplicity",      &t_modulesl1.multiplicity       );
    t_modulesl1_tree_->SetBranchAddress("energy_adc",         t_modulesl1.energy_adc         );
    t_modulesl1_tree_->SetBranchAddress("compress",          &t_modulesl1.compress           );
    t_modulesl1_tree_->SetBranchAddress("common_noise",      &t_modulesl1.common_noise       );

    trigger_set_start();
    
    return true;
}

void SciFile1RR::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    delete t_file_in_;
    t_file_in_ = NULL;
    t_triggerl1_tree_ = NULL;
    t_modulesl1_tree_ = NULL;
}



void SciFile1RR::trigger_set_start() {
    trigger_cur_entry_ = -1;
    trigger_reach_end_ = false;

    trigg_total_bad_count_            = 0;
    event_total_intact_count_         = 0;
    modules_total_aligned_count_      = 0;
    modules_total_match_err_count_    = 0;
}

Long64_t SciFile1RR::trigger_get_tot_entries() {
    return t_triggerl1_tree_->GetEntries();
}

Long64_t SciFile1RR::trigger_get_cur_entry() {
    return trigger_cur_entry_;
}

bool SciFile1RR::trigger_next_event() {
    if (trigger_reach_end_)
        return false;
    do {
        trigger_cur_entry_++;
        if (trigger_cur_entry_ < t_triggerl1_tree_->GetEntries()) {
            t_triggerl1_tree_->GetEntry(trigger_cur_entry_);
            if (t_triggerl1.is_bad > 0) {
                trigg_total_bad_count_++;
            }
        } else {
            trigger_reach_end_ = true;
            return false;
        }
    } while (t_triggerl1.is_bad > 0);

    if (t_triggerl1.lost_count == 0) {
        event_total_intact_count_++;
    }
    modules_total_aligned_count_ += t_triggerl1.pkt_count;

    modules_event_start_entry_ = t_triggerl1.pkt_start;
    modules_event_stop_entry_  = t_triggerl1.pkt_start + t_triggerl1.pkt_count;
    
    modules_event_cur_entry_   = modules_event_start_entry_ - 1;
    modules_event_reach_end_   = false;

    return true;
}

bool SciFile1RR::modules_next_packet() {
    if (modules_event_reach_end_)
        return false;
    modules_event_cur_entry_++;
    if (modules_event_cur_entry_ < modules_event_stop_entry_) {
        t_modulesl1_tree_->GetEntry(modules_event_cur_entry_);
        if (t_modulesl1.trigg_num != t_triggerl1.trigg_num) {
            modules_total_match_err_count_++;
        }
        return true;
    } else {
        modules_event_reach_end_ = true;
        return false;
    }
}

bool SciFile1RR::modules_cur_matched() {
    return (t_modulesl1.trigg_num == t_triggerl1.trigg_num);
}

Long64_t SciFile1RR::modules_get_tot_entries() {
    return t_modulesl1_tree_->GetEntries();
}

Long64_t SciFile1RR::get_bad_trigger_cnt() {
    return trigg_total_bad_count_;
}

Long64_t SciFile1RR::get_intact_event_cnt() {
    return event_total_intact_count_;
}

Long64_t SciFile1RR::get_aligned_cnt() {
    return modules_total_aligned_count_;
}

Long64_t SciFile1RR::get_match_err_cnt() {
    return modules_total_match_err_count_;
}

void SciFile1RR::gen_align_info_str() {
    char str_buffer[200];
    sprintf(str_buffer,
            "bad: %ld/%ld, intact: %ld/%ld, aligned: %ld/%ld, match_err: %ld/%ld",
            static_cast<long int>(trigg_total_bad_count_),
            static_cast<long int>(t_triggerl1_tree_->GetEntries()),
            static_cast<long int>(event_total_intact_count_),
            static_cast<long int>(t_triggerl1_tree_->GetEntries()),
            static_cast<long int>(modules_total_aligned_count_),
            static_cast<long int>(t_modulesl1_tree_->GetEntries()),
            static_cast<long int>(modules_total_match_err_count_),
            static_cast<long int>(t_modulesl1_tree_->GetEntries()));
    align_info_str_.assign(str_buffer);
}

string SciFile1RR::get_align_info_str() {
    return align_info_str_;
}

void SciFile1RR::print_align_info() {
    cout << "align_info: { " << align_info_str_ << " }" << endl;
    cout << "==========================================================================================================" << endl;
}
