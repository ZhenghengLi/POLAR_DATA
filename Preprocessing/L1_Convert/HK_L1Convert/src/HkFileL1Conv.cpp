#include "HkFileL1Conv.hpp"

using namespace std;

HkFileL1Conv::HkFileL1Conv() {
    t_file_in_       = NULL;
    t_file_out_      = NULL;
    t_hk_obox_tree_  = NULL;
    t_pol_hk_tree_   = NULL;
}

HkFileL1Conv::~HkFileL1Conv() {
    close_r();
    close_w();
}

bool HkFileL1Conv::open_r(const char* filename) {
    if (t_file_in_ != NULL)
        return false;
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_hk_obox_tree_ = static_cast<TTree*>(t_file_in_->Get("t_hk_obox"));
    if (t_hk_obox_tree_ == NULL)
        return false;
    // bind TTree
    t_hk_obox_tree_->SetBranchAddress("odd_is_bad",           &t_pol_hk.odd_is_bad       );
    t_hk_obox_tree_->SetBranchAddress("even_is_bad",          &t_pol_hk.even_is_bad      );
    t_hk_obox_tree_->SetBranchAddress("obox_is_bad",          &t_pol_hk.obox_is_bad      );
    t_hk_obox_tree_->SetBranchAddress("gps_pps_count",        &t_pol_hk.gps_pps_count    );
    t_hk_obox_tree_->SetBranchAddress("abs_gps_week",         &t_pol_hk.abs_gps_week     );
    t_hk_obox_tree_->SetBranchAddress("abs_gps_second",       &t_pol_hk.abs_gps_second   );
    t_hk_obox_tree_->SetBranchAddress("abs_ship_second",      &t_pol_hk.abs_ship_second  );
    t_hk_obox_tree_->SetBranchAddress("packet_num",           &t_pol_hk.packet_num       );
    t_hk_obox_tree_->SetBranchAddress("obox_mode",            &t_pol_hk.obox_mode        );
    t_hk_obox_tree_->SetBranchAddress("ct_temp",              &t_pol_hk.ct_temp          );
    t_hk_obox_tree_->SetBranchAddress("chain_temp",           &t_pol_hk.chain_temp       );
    t_hk_obox_tree_->SetBranchAddress("fe_status",             t_pol_hk.fe_status        );
    t_hk_obox_tree_->SetBranchAddress("fe_temp",               t_pol_hk.fe_temp          );
    t_hk_obox_tree_->SetBranchAddress("fe_hv",                 t_pol_hk.fe_hv            );
    t_hk_obox_tree_->SetBranchAddress("fe_thr",                t_pol_hk.fe_thr           );
    t_hk_obox_tree_->SetBranchAddress("fe_rate",               t_pol_hk.fe_rate          );
    t_hk_obox_tree_->SetBranchAddress("fe_cosmic",             t_pol_hk.fe_cosmic        );
    t_hk_obox_tree_->SetBranchAddress("saa",                  &t_pol_hk.saa              );

    hk_obox_set_start();
    
    return true;
}

bool HkFileL1Conv::open_w(const char* filename) {
    if (t_file_out_ != NULL)
        return false;
    t_file_out_ = new TFile(filename, "RECREATE");
    if (t_file_out_->IsZombie())
        return false;
    t_pol_hk_tree_ = new TTree("t_pol_hk", "POLAR housekeeping");
    t_pol_hk_tree_->SetDirectory(t_file_out_);
    t_pol_hk_tree_->Branch("abs_gps_week",       &t_pol_hk.abs_gps_week,       "abs_gps_week/I"      );
    t_pol_hk_tree_->Branch("abs_gps_second",     &t_pol_hk.abs_gps_second,     "abs_gps_second/D"    );
    t_pol_hk_tree_->Branch("abs_gps_valid",      &t_pol_hk.abs_gps_valid,      "abs_gps_valid/O"     );
    t_pol_hk_tree_->Branch("abs_ship_second",    &t_pol_hk.abs_ship_second,    "abs_ship_second/D"   );
    t_pol_hk_tree_->Branch("packet_num",         &t_pol_hk.packet_num,         "packet_num/s"        );
    t_pol_hk_tree_->Branch("obox_mode",          &t_pol_hk.obox_mode,          "obox_mode/b"         );
    t_pol_hk_tree_->Branch("ct_temp",            &t_pol_hk.ct_temp,            "ct_temp/F"           );
    t_pol_hk_tree_->Branch("chain_temp",         &t_pol_hk.chain_temp,         "chain_temp/F"        );
    t_pol_hk_tree_->Branch("fe_status",           t_pol_hk.fe_status,          "fe_status[25]/b"     );
    t_pol_hk_tree_->Branch("fe_temp",             t_pol_hk.fe_temp,            "fe_temp[25]/F"       );
    t_pol_hk_tree_->Branch("fe_hv",               t_pol_hk.fe_hv,              "fe_hv[25]/F"         );
    t_pol_hk_tree_->Branch("fe_thr",              t_pol_hk.fe_thr,             "fe_thr[25]/F"        );
    t_pol_hk_tree_->Branch("fe_rate",             t_pol_hk.fe_rate,            "fe_rate[25]/s"       );
    t_pol_hk_tree_->Branch("fe_cosmic",           t_pol_hk.fe_cosmic,          "fe_cosmic[25]/s"     );
    t_pol_hk_tree_->Branch("saa",                &t_pol_hk.saa,                "saa/s"               );

    pol_hk_cur_index_          = -1;
    pol_hk_first_gps_found_    = false;
    pol_hk_first_gps_index_    = -1;
    pol_hk_first_gps_week_     = -1;
    pol_hk_first_gps_second_   = -1;
    pol_hk_last_gps_index_     = -1;
    pol_hk_last_gps_week_      = -1;
    pol_hk_last_gps_second_    = -1;
    pol_hk_total_gps_count_    =  0;
    
    return true;
}

void HkFileL1Conv::close_r() {
    if (t_file_in_ == NULL)
        return;

    t_file_in_->Close();
    delete t_file_in_;
    t_file_in_ = NULL;
    t_hk_obox_tree_ = NULL;
}

void HkFileL1Conv::close_w() {
    if (t_file_out_ == NULL)
        return;

    delete t_pol_hk_tree_;
    t_pol_hk_tree_ = NULL;

    t_file_out_->Close();
    delete t_file_out_;
    t_file_out_ = NULL;
}

void HkFileL1Conv::hk_obox_set_start() {
    hk_obox_cur_entry_ = -1;
    hk_obox_reach_end_ = false;
    hk_obox_bad_count_ = 0;
}

Long64_t HkFileL1Conv::hk_obox_get_cur_entry() {
    return hk_obox_cur_entry_;
}

Long64_t HkFileL1Conv::hk_obox_get_tot_entries() {
    if (t_hk_obox_tree_ == NULL)
        return 0;
    else
        return t_hk_obox_tree_->GetEntries();
}

bool HkFileL1Conv::hk_obox_next() {
    if (t_file_in_ == NULL)
        return false;
    if (hk_obox_reach_end_)
        return false;
    do {
        hk_obox_cur_entry_++;
        if (hk_obox_cur_entry_ < t_hk_obox_tree_->GetEntries()) {
            t_hk_obox_tree_->GetEntry(hk_obox_cur_entry_);
            if (t_pol_hk.obox_is_bad > 0) {
                hk_obox_bad_count_++;
            }
        } else {
            hk_obox_reach_end_ = true;
            return false;
        }
    } while (t_pol_hk.obox_is_bad > 0);
    
    return true;
}

void HkFileL1Conv::pol_hk_fill() {
    if (t_file_out_ == NULL)
        return;
    t_pol_hk.abs_gps_valid = ((t_pol_hk.odd_is_bad == 0 || t_pol_hk.even_is_bad == 0) &&
                              ((t_pol_hk.gps_pps_count >> 15) & 1) == 0);
    t_pol_hk_tree_->Fill();
    // record first and last GPS time and entry
    pol_hk_cur_index_++;
    if (t_pol_hk.abs_gps_valid) {
        pol_hk_total_gps_count_++;
        if (!pol_hk_first_gps_found_) {
            pol_hk_first_gps_found_   = true;
            pol_hk_first_gps_index_   = pol_hk_cur_index_;
            pol_hk_first_gps_week_    = t_pol_hk.abs_gps_week;
            pol_hk_first_gps_second_  = t_pol_hk.abs_gps_second;
        }
        pol_hk_last_gps_index_        = pol_hk_cur_index_;
        pol_hk_last_gps_week_         = t_pol_hk.abs_gps_week;
        pol_hk_last_gps_second_       = t_pol_hk.abs_gps_second;
    }
}

void HkFileL1Conv::pol_hk_write_tree() {
    if (t_file_out_ == NULL)
        return;
    t_pol_hk_tree_->Write();
}

void HkFileL1Conv::pol_hk_write_meta(const char* key, const char* value) {
    if (t_file_out_ == NULL)
        return;
    TNamed* cur_meta = new TNamed(key, value);
    cur_meta->Write();
    delete cur_meta;
    cur_meta = NULL;
}

void HkFileL1Conv::pol_hk_gen_gps_span_str() {
    if (t_file_in_ == NULL || t_file_out_ == NULL)
        return;
    char str_buffer[200];
    sprintf(str_buffer, "%d:%d[%ld] => %d:%d[%ld]; %ld/%ld",
            static_cast<int>(pol_hk_first_gps_week_),
            static_cast<int>(pol_hk_first_gps_second_),
            static_cast<long int>(pol_hk_first_gps_index_),
            static_cast<int>(pol_hk_last_gps_week_),
            static_cast<int>(pol_hk_last_gps_second_),
            static_cast<long int>(pol_hk_last_gps_index_),
            static_cast<long int>(pol_hk_total_gps_count_),
            static_cast<long int>(t_pol_hk_tree_->GetEntries()));
    pol_hk_gps_result_str_.assign(str_buffer);
}

void HkFileL1Conv::pol_hk_gen_extra_info_str() {
    if (t_file_in_ == NULL || t_file_out_ == NULL)
        return;
    char str_buffer[200];
    sprintf(str_buffer, "bad: %ld/%ld",
            static_cast<long int>(hk_obox_bad_count_),
            static_cast<long int>(t_hk_obox_tree_->GetEntries()));
    pol_hk_extra_info_str_.assign(str_buffer);
}

void HkFileL1Conv::pol_hk_write_gps_span() {
    if (t_file_out_ == NULL)
        return;
    pol_hk_write_meta("m_gpsspan", pol_hk_gps_result_str_.c_str());
}

void HkFileL1Conv::pol_hk_write_extra_info() {
    if (t_file_out_ == NULL)
        return;
    pol_hk_write_meta("m_extrinf", pol_hk_extra_info_str_.c_str());
}

void HkFileL1Conv::pol_hk_print_gps_span() {
    cout << "================================================================================" << endl;
    cout << "GPS span: { " << pol_hk_gps_result_str_ << " }" << endl;
}

void HkFileL1Conv::pol_hk_print_extra_info() {
    cout << "extra info: { " << pol_hk_extra_info_str_ << " }" << endl;
    cout << "================================================================================" << endl;
}
