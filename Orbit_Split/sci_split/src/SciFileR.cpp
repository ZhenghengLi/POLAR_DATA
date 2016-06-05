#include "SciFileR.hpp"

SciFileR::SciFileR() {
    re_gps_      = "^ *(\\d+) *: *(\\d+) *$";
    re_gps_span_ = "^ *(\\d+:\\d+)\\[(\\d+)\\] => (\\d+:\\d+)\\[(\\d+)\\]; (\\d+)/(\\d+) *$";

    t_file_in_ = NULL;
    t_modules_tree_ = NULL;
    t_trigger_tree_ = NULL;
    t_ped_modules_tree_ = NULL;
    t_ped_trigger_tree_ = NULL;
    m_phy_gps_ = NULL;
    m_ped_gps_ = NULL;
}

SciFileR::~SciFileR() {

}

bool SciFileR::open(const char* filename, const char* gps_begin, const char* gps_end) {
    // record gps time
    gps_str_begin_.assign(gps_begin);
    if (gps_str_begin_ == "begin") {
        gps_value_begin_ = 0;
    } else {
        gps_value_begin_ = value_of_gps_str_(gps_str_begin_);
        if (gps_value_begin_ < 0) {
            cerr << "GPS string: " << gps_str_begin_ << " is invalid, it should be week:second." << endl;
            return false;
        }
    }
    gps_str_end_.assign(gps_end);    
    if (gps_str_end_ == "end") {
        gps_value_end_ = numeric_limits<double>::max();
    } else {
        gps_value_end_ = value_of_gps_str_(gps_str_end_);
        if (gps_value_end_ < 0) {
            cerr << "GPS string: " << gps_str_end_ << " is invalid, it should be week:second." << endl;
            return false;
        }
    }
    if (gps_value_end_ - gps_value_begin_ < GPS_SPAN_MIN) {
        cerr << "the GPS span of input is too small: " << gps_value_end_ - gps_value_begin_ << endl;
        return false;
    }
    
    // open file and check
    name_str_file_in_.assign(filename);
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_trigger_tree_ = static_cast<TTree*>(t_file_in_->Get("t_trigger"));
    if (t_trigger_tree_ == NULL)
        return false;
    if (t_trigger_tree_->FindBranch("abs_gps_week") == NULL ||
        t_trigger_tree_->FindBranch("abs_gps_second") == NULL ||
        t_trigger_tree_->FindBranch("abs_gps_valid") == NULL) {
        cerr << "File: " << filename << " may be not a 1P level SCI data file." << endl;
        return false;
    }
    t_modules_tree_ = static_cast<TTree*>(t_file_in_->Get("t_modules"));
    if (t_modules_tree_ == NULL)
        return false;
    t_ped_trigger_tree_ = static_cast<TTree*>(t_file_in_->Get("t_ped_trigger"));
    if (t_ped_trigger_tree_ == NULL)
        return false;
    t_ped_modules_tree_ = static_cast<TTree*>(t_file_in_->Get("t_ped_modules"));
    if (t_ped_modules_tree_ == NULL)
        return false;
    if (t_trigger_tree_->GetEntries() < 1 ||
        t_modules_tree_->GetEntries() < 1 ||
        t_ped_trigger_tree_->GetEntries() < 1 ||
        t_ped_modules_tree_->GetEntries() < 1) {
        cerr << "File: " << filename << " may be empty.";
        return false;
    }
    m_phy_gps_ = static_cast<TNamed*>(t_file_in_->Get("m_phy_gps"));
    if (m_phy_gps_ == NULL)
        return false;
    m_ped_gps_ = static_cast<TNamed*>(t_file_in_->Get("m_ped_gps"));
    if (m_ped_gps_ == NULL)
        return false;

    // find the first and last gps
    cmatch cm;
    if (regex_match(m_phy_gps_->GetTitle(), cm, re_gps_span_)) {
        gps_str_first_phy_.assign(cm[1]);
        gps_str_last_phy_.assign(cm[3]);
        gps_value_first_phy_ = value_of_gps_str_(gps_str_first_phy_);
        if (gps_value_first_phy_ < 0)
            return false;
        gps_value_last_phy_ = value_of_gps_str_(gps_str_last_phy_);
        if (gps_value_last_phy_ < 0)
            return false;
    } else {
        cerr << "GPS span regex match failed: " << m_phy_gps_->GetTitle() << endl;
        return false;
    }
    if (regex_match(m_ped_gps_->GetTitle(), cm, re_gps_span_)) {
        gps_str_first_ped_.assign(cm[1]);
        gps_str_last_ped_.assign(cm[3]);
        gps_value_first_ped_ = value_of_gps_str_(gps_str_first_ped_);
        if (gps_value_first_ped_ < 0)
            return false;
        gps_value_last_ped_ = value_of_gps_str_(gps_str_last_ped_);
        if (gps_value_last_ped_ < 0)
            return false;
    } else {
        cerr << "GPS span regex match failed: " << m_ped_gps_->GetTitle() << endl;
        return false;
    }

    // check GPS span matching
    if (gps_str_begin_ != "begin") {
        if (gps_value_begin_ < max(gps_value_first_ped_, gps_value_first_phy_)) {
            cerr << "GPS string of beginning is out of range: "
                 << max(gps_value_first_ped_, gps_value_first_phy_) - gps_value_begin_
                 << " seconds" << endl;
            return false;
        }
    }
    if (gps_str_end_ != "end") {
        if (gps_value_end_ > min(gps_value_last_ped_, gps_value_last_phy_)) {
            cerr << "GPS string of ending is out range: "
                 << gps_value_end_ - min(gps_value_last_ped_, gps_value_last_phy_)
                 << " seconds" << endl;
            return false;
        }
    }
    
    // bind TTree
    bind_trigger_tree(t_trigger_tree_, t_trigger);
    bind_modules_tree(t_modules_tree_, t_modules);
    bind_trigger_tree(t_ped_trigger_tree_, t_ped_trigger);
    bind_modules_tree(t_ped_modules_tree_, t_ped_modules);
    
    // find the first and last entry
    char str_buffer[200];
    bool found_last;
    TEventList* cur_elist;
    if (gps_str_begin_ == "begin") {
        phy_trigger_first_entry_ = 0;
        phy_modules_first_entry_ = 0;
        ped_trigger_first_entry_ = 0;
        ped_modules_first_entry_ = 0;
        if (gps_str_end_ == "end") {
            phy_trigger_last_entry_ = t_trigger_tree_->GetEntries();
            phy_modules_last_entry_ = t_modules_tree_->GetEntries();
            ped_trigger_last_entry_ = t_ped_trigger_tree_->GetEntries();
            ped_modules_last_entry_ = t_ped_modules_tree_->GetEntries();
        } else {
            sprintf(str_buffer,
                    "abs_gps_week * 604800 + abs_gps_second >= %ld &&"
                    "abs_gps_week >= 0 && abs_gps_second >= 0 && abs_gps_valid &&"
                    "pkt_start >= 0",
                    static_cast<long int>(gps_value_end_));
            // phy 
            t_trigger_tree_->Draw(">>elist", str_buffer);
            cur_elist = static_cast<TEventList*>(gDirectory->Get("elist"));
            if (cur_elist->GetN() < 1) {
                cerr << "Cannot find the last valid entry of t_trigger." << endl;
                return false;
            }
            phy_trigger_last_entry_ = cur_elist->GetEntry(0);
            t_trigger_tree_->GetEntry(phy_trigger_last_entry_);
            phy_modules_last_entry_ = t_trigger.pkt_start;
            // ped
            t_ped_trigger_tree_->Draw(">>elist", str_buffer);
            cur_elist = static_cast<TEventList*>(gDirectory->Get("elist"));
            if (cur_elist->GetN() < 1) {
                cerr << "Cannot find the last valid entry of t_ped_trigger." << endl;
                return false;
            }
            ped_trigger_last_entry_ = cur_elist->GetEntry(0);
            t_ped_trigger_tree_->GetEntry(ped_trigger_last_entry_);
            ped_modules_last_entry_ = t_ped_trigger.pkt_start;
        }
    } else {
        if (gps_str_end_ == "end") {
            phy_trigger_last_entry_ = t_trigger_tree_->GetEntries();
            phy_modules_last_entry_ = t_modules_tree_->GetEntries();
            ped_trigger_last_entry_ = t_ped_trigger_tree_->GetEntries();
            ped_modules_last_entry_ = t_ped_modules_tree_->GetEntries();
            sprintf(str_buffer,
                    "abs_gps_week * 604800 + abs_gps_second >= %ld &&"
                    "abs_gps_week >= 0 && abs_gps_second >= 0 && abs_gps_valid &&"
                    "pkt_start >= 0",
                    static_cast<long int>(gps_value_begin_));
            // phy 
            t_trigger_tree_->Draw(">>elist", str_buffer);
            cur_elist = static_cast<TEventList*>(gDirectory->Get("elist"));
            if (cur_elist->GetN() < 1) {
                cerr << "Cannot find the first valid entry of t_trigger." << endl;
                return false;
            }
            phy_trigger_first_entry_ = cur_elist->GetEntry(0);
            t_trigger_tree_->GetEntry(phy_trigger_first_entry_);
            phy_modules_first_entry_ = t_trigger.pkt_start;
            // ped
            t_ped_trigger_tree_->Draw(">>elist", str_buffer);
            cur_elist = static_cast<TEventList*>(gDirectory->Get("elist"));
            if (cur_elist->GetN() < 1) {
                cerr << "Cannot find the first valid entry of t_ped_trigger." << endl;
                return false;
            }
            ped_trigger_first_entry_ = cur_elist->GetEntry(0);
            t_ped_trigger_tree_->GetEntry(ped_trigger_first_entry_);
            ped_modules_first_entry_ = t_ped_trigger.pkt_start;
        } else {
            sprintf(str_buffer,
                    "abs_gps_week * 604800 + abs_gps_second >= %ld &&"
                    "abs_gps_week * 604800 + abs_gps_second < %ld &&"
                    "abs_gps_week >= 0 && abs_gps_second >= 0 && abs_gps_valid &&"
                    "pkt_start >= 0",
                    static_cast<long int>(gps_value_begin_),
                    static_cast<long int>(gps_value_end_));
            // phy first
            t_trigger_tree_->Draw(">>elist", str_buffer);
            cur_elist = static_cast<TEventList*>(gDirectory->Get("elist"));
            if (cur_elist->GetN() < GPS_SPAN_MIN) {
                cerr << "Cannot find enough valid entries of t_trigger." << endl;
                return false;
            }
            phy_trigger_first_entry_ = cur_elist->GetEntry(0);
            t_trigger_tree_->GetEntry(phy_trigger_first_entry_);
            phy_modules_first_entry_ = t_trigger.pkt_start;
            // phy last
            phy_trigger_last_entry_ = cur_elist->GetEntry(cur_elist->GetN() - 1);
            found_last = false;
            for (Long64_t i = phy_trigger_last_entry_ + 1; i < t_trigger_tree_->GetEntries(); i++) {
                t_trigger_tree_->GetEntry(i);
                if (t_trigger.abs_gps_week >= 0 && t_trigger.abs_gps_second >= 0 &&
                    t_trigger.abs_gps_valid && t_trigger.pkt_start >= 0) {
                    found_last = true;
                    phy_trigger_last_entry_ = i;
                    break;
                }
            }
            if (found_last) {
                t_trigger_tree_->GetEntry(phy_trigger_last_entry_);
                phy_modules_last_entry_ = t_trigger.pkt_start;
            } else {
                phy_trigger_last_entry_ = t_trigger_tree_->GetEntries();
                phy_modules_last_entry_ = t_modules_tree_->GetEntries();
            }
            // ped first
            t_ped_trigger_tree_->Draw(">>elist", str_buffer);
            cur_elist = static_cast<TEventList*>(gDirectory->Get("elist"));
            if (cur_elist->GetN() < GPS_SPAN_MIN) {
                cerr << "Cannot find enough valid entries of t_ped_trigger." << endl;
                return false;
            }
            ped_trigger_first_entry_ = cur_elist->GetEntry(0);
            t_ped_trigger_tree_->GetEntry(ped_trigger_first_entry_);
            ped_modules_first_entry_ = t_ped_trigger.pkt_start;
            // ped last
            ped_trigger_last_entry_ = cur_elist->GetEntry(cur_elist->GetN() - 1);
            found_last = false;
            for (Long64_t i = ped_trigger_last_entry_ + 1; i < t_ped_trigger_tree_->GetEntries(); i++) {
                t_ped_trigger_tree_->GetEntry(i);
                if (t_ped_trigger.abs_gps_week >= 0 && t_ped_trigger.abs_gps_second >= 0 &&
                    t_ped_trigger.abs_gps_valid && t_ped_trigger.pkt_start >= 0) {
                    found_last = true;
                    ped_trigger_last_entry_ = i;
                    break;
                }
            }
            if (found_last) {
                t_ped_trigger_tree_->GetEntry(ped_trigger_last_entry_);
                ped_modules_last_entry_ = t_ped_trigger.pkt_start;
            } else {
                ped_trigger_last_entry_ = t_ped_trigger_tree_->GetEntries();
                ped_modules_last_entry_ = t_ped_modules_tree_->GetEntries();
            }
        }
    }

    // find the first and last number, time_stamp, ets. of each module
    find_trigger_first_();
    find_trigger_last_();
    find_modules_first_();
    find_modules_last_();
    
    return true;
}

void SciFileR::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    t_file_in_ = NULL;
    t_modules_tree_ = NULL;
    t_trigger_tree_ = NULL;
    t_ped_modules_tree_ = NULL;
    t_ped_trigger_tree_ = NULL;
    m_phy_gps_ = NULL;
    m_ped_gps_ = NULL;
}

double SciFileR::value_of_gps_str_(const string gps_str) {
    cmatch cm;
    if (regex_match(gps_str.c_str(), cm, re_gps_)) {
        return TString(cm[1]).Atof() * 604800 + TString(cm[2]).Atof();
    } else {
        return -1;
    }
}

void SciFileR::print_file_info() {
    if (t_file_in_ == NULL)
        return;
    char str_buffer[200];
    string phy_first_gps;
    string phy_last_gps;
    string ped_first_gps;
    string ped_last_gps;
    string phy_result_str;
    string ped_result_str;
    if (gps_str_begin_ == "begin") {
        // phy
        phy_first_gps = gps_str_first_phy_;
        // ped
        ped_first_gps = gps_str_first_ped_;
    } else {
        // phy
        t_trigger_tree_->GetEntry(phy_trigger_first_entry_);
        sprintf(str_buffer, "%d:%d",
                static_cast<int>(t_trigger.abs_gps_week),
                static_cast<int>(t_trigger.abs_gps_second)
            );
        phy_first_gps.assign(str_buffer);
        // ped
        t_ped_trigger_tree_->GetEntry(ped_trigger_first_entry_);
        sprintf(str_buffer, "%d:%d",
                static_cast<int>(t_ped_trigger.abs_gps_week),
                static_cast<int>(t_ped_trigger.abs_gps_second)
            );
        ped_first_gps.assign(str_buffer);
    }
    if (gps_str_end_ == "end") {
        // phy
        phy_last_gps = gps_str_last_phy_;
        // ped
        ped_last_gps = gps_str_last_ped_;
    } else {
        // phy
        if (phy_trigger_last_entry_ == t_trigger_tree_->GetEntries()) {
            phy_last_gps = gps_str_last_phy_;
        } else {
            t_trigger_tree_->GetEntry(phy_trigger_last_entry_);
            sprintf(str_buffer, "%d:%d",
                    static_cast<int>(t_trigger.abs_gps_week),
                    static_cast<int>(t_trigger.abs_gps_second)
                );
            phy_last_gps.assign(str_buffer);
        }
        // ped
        if (ped_trigger_last_entry_ == t_ped_trigger_tree_->GetEntries()) {
            ped_last_gps = gps_str_last_ped_;
        } else {
            t_ped_trigger_tree_->GetEntry(ped_trigger_last_entry_);
            sprintf(str_buffer, "%d:%d",
                    static_cast<int>(t_ped_trigger.abs_gps_week),
                    static_cast<int>(t_ped_trigger.abs_gps_second)
                );
            ped_last_gps.assign(str_buffer);
        }
    }
    // phy
    sprintf(str_buffer,
            "%s [%ld, %ld] => %s [%ld, %ld] / [%ld, %ld]",
            phy_first_gps.c_str(),
            static_cast<long int>(phy_trigger_first_entry_),
            static_cast<long int>(phy_modules_first_entry_),
            phy_last_gps.c_str(),
            static_cast<long int>(phy_trigger_last_entry_),
            static_cast<long int>(phy_modules_last_entry_),
            static_cast<long int>(t_trigger_tree_->GetEntries()),
            static_cast<long int>(t_modules_tree_->GetEntries())
        );
    phy_result_str.assign(str_buffer);
    // ped
    sprintf(str_buffer,
            "%s [%ld, %ld] => %s [%ld, %ld] / [%ld, %ld]",
            ped_first_gps.c_str(),
            static_cast<long int>(ped_trigger_first_entry_),
            static_cast<long int>(ped_modules_first_entry_),
            ped_last_gps.c_str(),
            static_cast<long int>(ped_trigger_last_entry_),
            static_cast<long int>(ped_modules_last_entry_),
            static_cast<long int>(t_ped_trigger_tree_->GetEntries()),
            static_cast<long int>(t_ped_modules_tree_->GetEntries())
        );
    ped_result_str.assign(str_buffer);
    
    cout << name_str_file_in_ << endl;
    cout << " - phy GPS span: { " << phy_result_str << " }" << endl;
    cout << " - ped GPS span: { " << ped_result_str << " }" << endl;
}

void SciFileR::find_trigger_first_() {
    if (t_file_in_ == NULL)
        return;
    
    // find phy trigger first
    t_trigger_tree_->GetEntry(phy_trigger_first_entry_);
    phy_trigger_first_trigg_num      = t_trigger.trigg_num;
    phy_trigger_first_trigg_num_g    = t_trigger.trigg_num_g;
    phy_trigger_first_time_stamp     = t_trigger.time_stamp;
    phy_trigger_first_time_period    = t_trigger.time_period;
    phy_trigger_first_pkt_start      = t_trigger.pkt_start;

    // find ped trigger first
    t_ped_trigger_tree_->GetEntry(ped_trigger_first_entry_);
    ped_trigger_first_trigg_num      = t_ped_trigger.trigg_num;
    ped_trigger_first_trigg_num_g    = t_ped_trigger.trigg_num_g;
    ped_trigger_first_time_stamp     = t_ped_trigger.time_stamp;
    ped_trigger_first_time_period    = t_ped_trigger.time_period;
    ped_trigger_first_pkt_start      = t_ped_trigger.pkt_start;


    // find all trigger first
    all_trigger_first_trigg_num_g = min(phy_trigger_first_trigg_num_g, ped_trigger_first_trigg_num_g);
    if (phy_trigger_first_time_period > ped_trigger_first_time_period) {
        all_trigger_first_time_period = ped_trigger_first_time_period;
        all_trigger_first_time_stamp  = ped_trigger_first_time_stamp;
    } else if ( phy_trigger_first_time_period < ped_trigger_first_time_period) {
        all_trigger_first_time_period = phy_trigger_first_time_period;
        all_trigger_first_time_stamp  = phy_trigger_first_time_stamp;
    } else {
        all_trigger_first_time_period = phy_trigger_first_time_period;
        all_trigger_first_time_stamp  = min(phy_trigger_first_time_stamp, ped_trigger_first_time_stamp);
    }
}

void SciFileR::find_trigger_last_() {
    if (t_file_in_ == NULL)
        return;
    
    // find phy trigger last
    if (phy_trigger_last_entry_ < t_trigger_tree_->GetEntries()) {
        t_trigger_tree_->GetEntry(phy_trigger_last_entry_);
        phy_trigger_last_pkt_start = t_trigger.pkt_start;
    } else {
        phy_trigger_last_pkt_start = t_trigger_tree_->GetEntries();
    }
    for (Long64_t i = phy_trigger_last_entry_ - 1; i >= phy_trigger_first_entry_; i--) {
        t_trigger_tree_->GetEntry(i);
        if (t_trigger.is_bad <= 0) {
            phy_trigger_last_trigg_num    = t_trigger.trigg_num;
            phy_trigger_last_trigg_num_g  = t_trigger.trigg_num_g;
            phy_trigger_last_time_stamp   = t_trigger.time_stamp;
            phy_trigger_last_time_period  = t_trigger.time_period;
            break;
        }
    }

    // find ped trigger last
    if (ped_trigger_last_entry_ < t_ped_trigger_tree_->GetEntries()) {
        t_ped_trigger_tree_->GetEntry(ped_trigger_last_entry_);
        ped_trigger_last_pkt_start = t_ped_trigger.pkt_start;
    } else {
        ped_trigger_last_pkt_start = t_ped_trigger_tree_->GetEntries();
    }
    for (Long64_t i = ped_trigger_last_entry_ - 1; i >= ped_trigger_first_entry_; i--) {
        t_ped_trigger_tree_->GetEntry(i);
        if (t_ped_trigger.is_bad <= 0) {
            ped_trigger_last_trigg_num    = t_ped_trigger.trigg_num;
            ped_trigger_last_trigg_num_g  = t_ped_trigger.trigg_num_g;
            ped_trigger_last_time_stamp   = t_ped_trigger.time_stamp;
            ped_trigger_last_time_period  = t_ped_trigger.time_period;
            break;
        }
    }

    // find all trigger last
    all_trigger_last_trigg_num_g = max(phy_trigger_last_trigg_num_g, ped_trigger_last_trigg_num_g);
    if (phy_trigger_last_time_period < ped_trigger_last_time_period) {
        all_trigger_last_time_period = ped_trigger_last_time_period;
        all_trigger_last_time_stamp  = ped_trigger_last_time_stamp;
    } else if ( phy_trigger_last_time_period > ped_trigger_last_time_period) {
        all_trigger_last_time_period = phy_trigger_last_time_period;
        all_trigger_last_time_stamp  = phy_trigger_last_time_stamp;
    } else {
        all_trigger_last_time_period = phy_trigger_last_time_period;
        all_trigger_last_time_stamp  = max(phy_trigger_last_time_stamp, ped_trigger_last_time_stamp);
    }
    
}

void SciFileR::find_modules_first_() {
    if (t_file_in_ == NULL)
        return;

    // find phy modules first
    for (int i = 0; i < 25; i++) {
        phy_modules_first_found[i] = false;
    }
    for (Long64_t i = phy_modules_first_entry_; i < phy_modules_last_entry_; i++) {
        t_modules_tree_->GetEntry(i);
        int idx = t_modules.ct_num - 1;
        if (t_modules.is_bad <= 0 && !phy_modules_first_found[idx]) {
            phy_modules_first_found[idx]        = true;
            phy_modules_first_event_num[idx]    = t_modules.event_num;
            phy_modules_first_event_num_g[idx]  = t_modules.event_num_g;
            phy_modules_first_time_stamp[idx]   = t_modules.time_stamp;
            phy_modules_first_time_period[idx]  = t_modules.time_period;
        }
        int found_sum = 0;
        for (int i = 0; i < 25; i++) {
            found_sum += static_cast<int>(phy_modules_first_found[i]);
        }
        if (found_sum == 25)
            break;
    }
    for (int i = 0; i < 25; i++) {
        if (!phy_modules_first_found[i]) {
            cerr << "Cannot find the first physical event packet of module CT_" << i + 1 << ". " << endl;
        }
    }
    
    // find ped modules first
    for (int i = 0; i < 25; i++) {
        ped_modules_first_found[i] = false;
    }
    for (Long64_t i = ped_modules_first_entry_; i < ped_modules_last_entry_; i++) {
        t_ped_modules_tree_->GetEntry(i);
        int idx = t_ped_modules.ct_num - 1;
        if (t_ped_modules.is_bad <= 0 && !ped_modules_first_found[idx]) {
            ped_modules_first_found[idx]        = true;
            ped_modules_first_event_num[idx]    = t_ped_modules.event_num;
            ped_modules_first_event_num_g[idx]  = t_ped_modules.event_num_g;
            ped_modules_first_time_stamp[idx]   = t_ped_modules.time_stamp;
            ped_modules_first_time_period[idx]  = t_ped_modules.time_period;
        }
        int found_sum = 0;
        for (int i = 0; i < 25; i++) {
            found_sum += static_cast<int>(ped_modules_first_found[i]);
        }
        if (found_sum == 25)
            break;
    }
    for (int i = 0; i < 25; i++) {
        if (!ped_modules_first_found[i]) {
            cerr << "Cannot find the first pedestal event packet of module CT_" << i + 1 << ". " << endl;
        }
    }

    // find all modules first
    for (int i = 0; i < 25; i++) {
        if (!phy_modules_first_found[i] && !ped_modules_first_found[i]) {
            continue;
        } else if (!phy_modules_first_found[i]) {
            all_modules_first_event_num_g[i] = ped_modules_first_event_num_g[i];
            all_modules_first_time_period[i] = ped_modules_first_time_period[i];
            all_modules_first_time_stamp[i]  = ped_modules_first_time_stamp[i];
        } else if (!ped_modules_first_found[i]) {
            all_modules_first_event_num_g[i] = phy_modules_first_event_num_g[i];
            all_modules_first_time_period[i] = phy_modules_first_time_period[i];
            all_modules_first_time_stamp[i]  = phy_modules_first_time_stamp[i];
        } else {
            all_modules_first_event_num_g[i] = min(phy_modules_first_event_num_g[i], ped_modules_first_event_num_g[i]);
            if (phy_modules_first_time_period[i] > ped_modules_first_time_period[i]) {
                all_modules_first_time_period[i] = ped_modules_first_time_period[i];
                all_modules_first_time_stamp[i]  = ped_modules_first_time_stamp[i];
            } else if (phy_modules_first_time_period[i] < ped_modules_first_time_period[i]) {
                all_modules_first_time_period[i] = phy_modules_first_time_period[i];
                all_modules_first_time_stamp[i]  = phy_modules_first_time_stamp[i];
            } else {
                all_modules_first_time_period[i] = phy_modules_first_time_period[i];
                all_modules_first_time_stamp[i]  = min(phy_modules_first_time_stamp[i], ped_modules_first_time_stamp[i]);
            }
        }
    }
}

void SciFileR::find_modules_last_() {
    if (t_file_in_ == NULL)
        return;

    // find phy modules last
    for (int i = 0; i < 25; i++) {
        phy_modules_last_found[i] = false;
    }
    for (Long64_t i = phy_modules_last_entry_ - 1; i >= phy_modules_first_entry_; i--) {
        t_modules_tree_->GetEntry(i);
        int idx = t_modules.ct_num - 1;
        if (t_modules.is_bad <= 0 && !phy_modules_last_found[idx]) {
            phy_modules_last_found[idx]        = true;
            phy_modules_last_event_num[idx]    = t_modules.event_num;
            phy_modules_last_event_num_g[idx]  = t_modules.event_num_g;
            phy_modules_last_time_stamp[idx]   = t_modules.time_stamp;
            phy_modules_last_time_period[idx]  = t_modules.time_period;
        }
        int found_sum = 0;
        for (int i = 0; i < 25; i++) {
            found_sum += static_cast<int>(phy_modules_last_found[i]);
        }
        if (found_sum == 25)
            break;
    }
    for (int i = 0; i < 25; i++) {
        if (!phy_modules_last_found[i]) {
            cerr << "Cannot find the last physical event packet of module CT_" << i + 1 << ". " << endl;
        }
    }
    
    // find ped modules last
    for (int i = 0; i < 25; i++) {
        ped_modules_last_found[i] = false;
    }
    for (Long64_t i = ped_modules_last_entry_ - 1; i >= ped_modules_first_entry_; i--) {
        t_ped_modules_tree_->GetEntry(i);
        int idx = t_ped_modules.ct_num - 1;
        if (t_ped_modules.is_bad <= 0 && !ped_modules_last_found[idx]) {
            ped_modules_last_found[idx]        = true;
            ped_modules_last_event_num[idx]    = t_ped_modules.event_num;
            ped_modules_last_event_num_g[idx]  = t_ped_modules.event_num_g;
            ped_modules_last_time_stamp[idx]   = t_ped_modules.time_stamp;
            ped_modules_last_time_period[idx]  = t_ped_modules.time_period;
        }
        int found_sum = 0;
        for (int i = 0; i < 25; i++) {
            found_sum += static_cast<int>(ped_modules_last_found[i]);
        }
        if (found_sum == 25)
            break;
    }
    for (int i = 0; i < 25; i++) {
        if (!ped_modules_last_found[i]) {
            cerr << "Cannot find the last pedestal event packet of module CT_" << i + 1 << ". " << endl;
        }
    }

    // find all modules last
    for (int i = 0; i < 25; i++) {
        if (!phy_modules_last_found[i] && !ped_modules_last_found[i]) {
            continue;
        } else if (!phy_modules_last_found[i]) {
            all_modules_last_event_num_g[i] = ped_modules_last_event_num_g[i];
            all_modules_last_time_period[i] = ped_modules_last_time_period[i];
            all_modules_last_time_stamp[i]  = ped_modules_last_time_stamp[i];
        } else if (!ped_modules_last_found[i]) {
            all_modules_last_event_num_g[i] = phy_modules_last_event_num_g[i];
            all_modules_last_time_period[i] = phy_modules_last_time_period[i];
            all_modules_last_time_stamp[i]  = phy_modules_last_time_stamp[i];
        } else {
            all_modules_last_event_num_g[i] = max(phy_modules_last_event_num_g[i], ped_modules_last_event_num_g[i]);
            if (phy_modules_last_time_period[i] < ped_modules_last_time_period[i]) {
                all_modules_last_time_period[i] = ped_modules_last_time_period[i];
                all_modules_last_time_stamp[i]  = ped_modules_last_time_stamp[i];
            } else if (phy_modules_last_time_period[i] > ped_modules_last_time_period[i]) {
                all_modules_last_time_period[i] = phy_modules_last_time_period[i];
                all_modules_last_time_stamp[i]  = phy_modules_last_time_stamp[i];
            } else {
                all_modules_last_time_period[i] = phy_modules_last_time_period[i];
                all_modules_last_time_stamp[i]  = max(phy_modules_last_time_stamp[i], ped_modules_last_time_stamp[i]);
            }
        }
    }
}

void SciFileR::phy_trigger_set_start() {
    phy_trigger_cur_entry_ = phy_trigger_first_entry_ - 1;
    phy_trigger_reach_end_ = false;
}

Long64_t SciFileR::phy_trigger_get_tot_entries() {
    return phy_trigger_last_entry_ - phy_trigger_first_entry_;
}

Long64_t SciFileR::phy_trigger_get_cur_entry() {
    return phy_trigger_cur_entry_ - phy_trigger_first_entry_;
}

bool SciFileR::phy_trigger_next() {
    if (phy_trigger_reach_end_)
        return false;
    phy_trigger_cur_entry_++;
    if (phy_trigger_cur_entry_ < phy_trigger_last_entry_) {
        t_trigger_tree_->GetEntry(phy_trigger_cur_entry_);
        return true;
    } else {
        phy_trigger_reach_end_ = true;
        return false;
    }
}

void SciFileR::phy_modules_set_start() {
    phy_modules_cur_entry_ = phy_modules_first_entry_ - 1;
    phy_modules_reach_end_ = false;
}

Long64_t SciFileR::phy_modules_get_tot_entries() {
    return phy_modules_last_entry_ - phy_modules_first_entry_;
}

Long64_t SciFileR::phy_modules_get_cur_entry() {
    return phy_modules_cur_entry_ - phy_modules_first_entry_;
}

bool SciFileR::phy_modules_next() {
    if (phy_modules_reach_end_)
        return false;
    phy_modules_cur_entry_++;
    if (phy_modules_cur_entry_ < phy_modules_last_entry_) {
        t_modules_tree_->GetEntry(phy_modules_cur_entry_);
        return true;
    } else {
        phy_modules_reach_end_ = true;
        return false;
    }
}

void SciFileR::ped_trigger_set_start() {
    ped_trigger_cur_entry_ = ped_trigger_first_entry_ - 1;
    ped_trigger_reach_end_ = false;
}

Long64_t SciFileR::ped_trigger_get_tot_entries() {
    return ped_trigger_last_entry_ - ped_trigger_first_entry_;
}

Long64_t SciFileR::ped_trigger_get_cur_entry() {
    return ped_trigger_cur_entry_ - ped_trigger_first_entry_;
}

bool SciFileR::ped_trigger_next() {
    if (ped_trigger_reach_end_)
        return false;
    ped_trigger_cur_entry_++;
    if (ped_trigger_cur_entry_ < ped_trigger_last_entry_) {
        t_ped_trigger_tree_->GetEntry(ped_trigger_cur_entry_);
        return true;
    } else {
        ped_trigger_reach_end_ = true;
        return false;
    }
}

void SciFileR::ped_modules_set_start() {
    ped_modules_cur_entry_ = ped_modules_first_entry_ - 1;
    ped_modules_reach_end_ = false;
}

Long64_t SciFileR::ped_modules_get_tot_entries() {
    return ped_modules_last_entry_ - ped_modules_first_entry_;
}

Long64_t SciFileR::ped_modules_get_cur_entry() {
    return ped_modules_cur_entry_ - ped_modules_first_entry_;
}

bool SciFileR::ped_modules_next() {
    if (ped_modules_reach_end_)
        return false;
    ped_modules_cur_entry_++;
    if (ped_modules_cur_entry_ < ped_modules_last_entry_) {
        t_ped_modules_tree_->GetEntry(ped_modules_cur_entry_);
        return true;
    } else {
        ped_modules_reach_end_ = true;
        return false;
    }
}

