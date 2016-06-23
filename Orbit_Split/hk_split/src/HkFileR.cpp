#include"HkFileR.hpp"

using namespace std;

HkFileR::HkFileR() {
    re_gps_      = "^ *(\\d+) *: *(\\d+) *$";
    re_gps_span_ = "^ *(\\d+:\\d+)\\[(\\d+)\\] => (\\d+:\\d+)\\[(\\d+)\\]; (\\d+)/(\\d+) *$";

    t_file_in_ = NULL;
    t_hk_obox_tree_ = NULL;
    t_hk_ibox_tree_ = NULL;
    m_oboxgps_ = NULL;
    m_iboxgps_ = NULL;
    
}

HkFileR::~HkFileR() {
    close();
}

bool HkFileR::open(const char* filename, const char* gps_begin, const char* gps_end) {
    if (t_file_in_ != NULL)
        return false;

    // record gps time
    gps_str_begin_.assign(gps_begin);
    if (gps_str_begin_ == "begin") {
        gps_value_begin_ = -2 * GPS_SPAN_MIN;
    } else {
        gps_value_begin_ = value_of_gps_str_(gps_str_begin_);
        if (gps_value_begin_ < 0) {
            cerr << "GPS string: " << gps_str_begin_ << " is invalid, it should be week:second." << endl;
            return false;
        }
    }
    gps_str_end_.assign(gps_end);
    if (gps_str_end_ == "end") {
        gps_value_end_ = numeric_limits<double>::max() - 3 * GPS_SPAN_MIN;
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

    // open and check
    name_str_file_in_.assign(filename);
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_hk_obox_tree_ = static_cast<TTree*>(t_file_in_->Get("t_hk_obox"));
    if (t_hk_obox_tree_ == NULL)
        return false;
    t_hk_ibox_tree_ = static_cast<TTree*>(t_file_in_->Get("t_hk_ibox"));
    if (t_hk_ibox_tree_ == NULL)
        return false;
    if (t_hk_obox_tree_->GetEntries() < 1 || t_hk_ibox_tree_->GetEntries() < 1) {
        cerr << "File: " << filename << " may be empty." << endl;
        return false;
    }
    m_oboxgps_ = static_cast<TNamed*>(t_file_in_->Get("m_oboxgps"));
    if (m_oboxgps_ == NULL)
        return false;
    m_iboxgps_ = static_cast<TNamed*>(t_file_in_->Get("m_iboxgps"));
    if (m_iboxgps_ == NULL)
        return false;

    // find the first and last gps
    cmatch cm;
    if (regex_match(m_oboxgps_->GetTitle(), cm, re_gps_span_)) {
        gps_str_first_hk_obox_.assign(cm[1]);
        gps_str_last_hk_obox_.assign(cm[3]);
        gps_value_first_hk_obox_ = value_of_gps_str_(gps_str_first_hk_obox_);
        if (gps_value_first_hk_obox_ < 0)
            return false;
        gps_value_last_hk_obox_ = value_of_gps_str_(gps_str_last_hk_obox_);
        if (gps_value_last_hk_obox_ < 0)
            return false;
    } else {
        cerr << "GPS span regex match failed: " << m_oboxgps_->GetTitle() << endl;
        return false;
    }
    if (regex_match(m_iboxgps_->GetTitle(), cm, re_gps_span_)) {
        gps_str_first_hk_ibox_.assign(cm[1]);
        gps_str_last_hk_ibox_.assign(cm[3]);
        gps_value_first_hk_ibox_ = value_of_gps_str_(gps_str_first_hk_ibox_);
        if (gps_value_first_hk_ibox_ < 0)
            return false;
        gps_value_last_hk_ibox_ = value_of_gps_str_(gps_str_last_hk_ibox_);
        if (gps_value_last_hk_ibox_ < 0)
            return false;
    } else {
        cerr << "GPS span regex match failed: " << m_iboxgps_->GetTitle() << endl;
        return false;
    }

    // check GPS span matching
    if (gps_str_begin_ != "begin") {
        if (gps_value_begin_ < gps_value_first_hk_ibox_) {
            cerr << "GPS string of beginning is out of range: "
                 << gps_value_first_hk_ibox_ - gps_value_begin_
                 << " seconds" << endl;
            return false;
        }
        if (gps_str_end_ == "end" && gps_value_last_hk_ibox_ - gps_value_begin_ < GPS_DIFF_MIN) {
            cerr << "GPS string of beginning is too large: "
                 << gps_value_last_hk_ibox_ - gps_value_begin_
                 << " seconds" << endl;
            return false;
        }
    }
    if (gps_str_end_ != "end") {
        if (gps_value_end_ > gps_value_last_hk_ibox_) {
            cerr << "GPS string of ending is out range: "
                 << gps_value_end_ - gps_value_last_hk_ibox_
                 << " seconds" << endl;
            return false;
        }
        if (gps_str_begin_ == "begin" && gps_value_end_ - gps_value_first_hk_ibox_ < GPS_DIFF_MIN) {
            cerr << "GPS string of ending is too small: "
                 << gps_value_end_ - gps_value_first_hk_ibox_
                 << " seconds" << endl;
            return false;
        }
    }

    // bind TTree
    bind_hk_obox_tree(t_hk_obox_tree_, t_hk_obox);
    bind_hk_ibox_tree(t_hk_ibox_tree_, t_hk_ibox);

    // find the firest and last entry
    char str_buffer[200];
    bool found_last;
    TEventList* cur_elist;
    if (gps_str_begin_ == "begin") {
        hk_obox_first_entry_ = 0;
        hk_ibox_first_entry_ = 0;
        if (gps_str_end_ == "end") {
            hk_obox_last_entry_ = t_hk_obox_tree_->GetEntries();
            hk_ibox_last_entry_ = t_hk_ibox_tree_->GetEntries();
        } else {
            sprintf(str_buffer,
                    "(odd_is_bad == 0 || even_is_bad == 0) &&"
                    "abs_gps_week * 604800 + abs_gps_second >= %ld",
                    static_cast<long int>(gps_value_end_));
            t_hk_obox_tree_->Draw(">>elist", str_buffer);
            cur_elist = static_cast<TEventList*>(gDirectory->Get("elist"));
            if (cur_elist->GetN() < 1) {
                cerr << "Cannot find the last valid entry of t_hk_obox." << endl;
                return false;
            }
            hk_obox_last_entry_ = cur_elist->GetEntry(0);
            sprintf(str_buffer,
                    "is_bad == 0 &&"
                    "abs_gps_week * 604800 + abs_gps_second >= %ld",
                    static_cast<long int>(gps_value_end_));
            t_hk_ibox_tree_->Draw(">>elist", str_buffer);
            cur_elist = static_cast<TEventList*>(gDirectory->Get("elist"));
            if (cur_elist->GetN() < 1) {
                cerr << "Cannot find the last valid entry of t_hk_ibox." << endl;
                return false;
            }
            hk_ibox_last_entry_ = cur_elist->GetEntry(0);
        }
    } else {
        if (gps_str_end_ == "end") {
            hk_obox_last_entry_ = t_hk_obox_tree_->GetEntries();
            hk_ibox_last_entry_ = t_hk_ibox_tree_->GetEntries();
            sprintf(str_buffer,
                    "(odd_is_bad == 0 || even_is_bad == 0) &&"
                    "abs_gps_week * 604800 + abs_gps_second >= %ld",
                    static_cast<long int>(gps_value_begin_));
            t_hk_obox_tree_->Draw(">>elist", str_buffer);
            cur_elist = static_cast<TEventList*>(gDirectory->Get("elist"));
            if (cur_elist->GetN() < 1) {
                cerr << "Cannot find the first valid entry of t_hk_obox." << endl;
                return false;
            }
            hk_obox_first_entry_ = cur_elist->GetEntry(0);
            sprintf(str_buffer,
                    "is_bad == 0 &&"
                    "abs_gps_week * 604800 + abs_gps_second >= %ld",
                    static_cast<long int>(gps_value_begin_));
            t_hk_ibox_tree_->Draw(">>elist", str_buffer);
            cur_elist = static_cast<TEventList*>(gDirectory->Get("elist"));
            if (cur_elist->GetN() < 1) {
                cerr << "Cannot find the first valid entry of t_hk_ibox." << endl;
                return false;
            }
            hk_ibox_first_entry_ = cur_elist->GetEntry(0);
        } else {
            sprintf(str_buffer,
                    "(odd_is_bad == 0 || even_is_bad == 0) &&"
                    "abs_gps_week * 604800 + abs_gps_second >= %ld &&"
                    "abs_gps_week * 604800 + abs_gps_second < %ld",
                    static_cast<long int>(gps_value_begin_),
                    static_cast<long int>(gps_value_end_));
            t_hk_obox_tree_->Draw(">>elist", str_buffer);
            cur_elist = static_cast<TEventList*>(gDirectory->Get("elist"));
            if (cur_elist->GetN() < GPS_SPAN_MIN / 2) {
                cerr << "Cannot find enough valid entries of t_hk_obox." << endl;
                return false;
            }
            hk_obox_first_entry_ = cur_elist->GetEntry(0);
            hk_obox_last_entry_  = cur_elist->GetEntry(cur_elist->GetN() - 1);
            found_last = false;
            for (Long64_t i = hk_obox_last_entry_ + 1; i < t_hk_obox_tree_->GetEntries(); i++) {
                t_hk_obox_tree_->GetEntry(i);
                if (t_hk_obox.odd_is_bad == 0 || t_hk_obox.even_is_bad == 0) {
                    found_last = true;
                    hk_obox_last_entry_ = i;
                    break;
                }
            }
            if (!found_last) {
                hk_obox_last_entry_ = t_hk_obox_tree_->GetEntries();
            }
            sprintf(str_buffer,
                    "is_bad == 0&&"
                    "abs_gps_week * 604800 + abs_gps_second >= %ld &&"
                    "abs_gps_week * 604800 + abs_gps_second < %ld",
                    static_cast<long int>(gps_value_begin_),
                    static_cast<long int>(gps_value_end_));
            t_hk_ibox_tree_->Draw(">>elist", str_buffer);
            cur_elist = static_cast<TEventList*>(gDirectory->Get("elist"));
            if (cur_elist->GetN() < GPS_SPAN_MIN) {
                cerr << "Cannot find enough valid entries of t_hk_ibox." << endl;
                return false;
            }
            hk_ibox_first_entry_ = cur_elist->GetEntry(0);
            hk_ibox_last_entry_  = cur_elist->GetEntry(cur_elist->GetN() - 1);
            found_last = false;
            for (Long64_t i = hk_ibox_last_entry_ + 1; i < t_hk_ibox_tree_->GetEntries(); i++) {
                t_hk_ibox_tree_->GetEntry(i);
                if (t_hk_ibox.is_bad == 0) {
                    found_last = true;
                    hk_ibox_last_entry_ = i;
                    break;
                }
            }
            if (!found_last) {
                hk_ibox_last_entry_ = t_hk_ibox_tree_->GetEntries();
            }
        }
    }
        
    return true;
}

void HkFileR::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    delete t_file_in_;
    t_file_in_ = NULL;
    t_hk_obox_tree_ = NULL;
    t_hk_ibox_tree_ = NULL;
    m_oboxgps_ = NULL;
    m_iboxgps_ = NULL;
}

void HkFileR::print_file_info() {
    if (t_file_in_ == NULL)
        return;
    char str_buffer[200];
    string hk_obox_first_gps;
    string hk_obox_last_gps;
    string hk_ibox_first_gps;
    string hk_ibox_last_gps;
    string hk_obox_result_str;
    string hk_ibox_result_str;
    if (gps_str_begin_ == "begin") {
        hk_obox_first_gps = gps_str_first_hk_obox_;
        hk_ibox_first_gps = gps_str_first_hk_ibox_;
    } else {
        t_hk_obox_tree_->GetEntry(hk_obox_first_entry_);
        sprintf(str_buffer, "%d:%d",
                static_cast<int>(t_hk_obox.abs_gps_week),
                static_cast<int>(t_hk_obox.abs_gps_second));
        hk_obox_first_gps.assign(str_buffer);
        t_hk_ibox_tree_->GetEntry(hk_ibox_first_entry_);
        sprintf(str_buffer, "%d:%d",
                static_cast<int>(t_hk_ibox.abs_gps_week),
                static_cast<int>(t_hk_ibox.abs_gps_second));
        hk_ibox_first_gps.assign(str_buffer);
    }
    if (gps_str_end_ == "end") {
        hk_obox_last_gps = gps_str_last_hk_obox_;
        hk_ibox_last_gps = gps_str_last_hk_ibox_;
    } else {
        if (hk_obox_last_entry_ == t_hk_obox_tree_->GetEntries()) {
            hk_obox_last_gps = gps_str_last_hk_obox_;
        } else {
            t_hk_obox_tree_->GetEntry(hk_obox_last_entry_);
            sprintf(str_buffer, "%d:%d",
                    static_cast<int>(t_hk_obox.abs_gps_week),
                    static_cast<int>(t_hk_obox.abs_gps_second));
            hk_obox_last_gps.assign(str_buffer);
        }
        if (hk_ibox_last_entry_ == t_hk_ibox_tree_->GetEntries()) {
            hk_ibox_last_gps = gps_str_last_hk_ibox_;
        } else {
            t_hk_ibox_tree_->GetEntry(hk_ibox_last_entry_);
            sprintf(str_buffer, "%d:%d",
                    static_cast<int>(t_hk_ibox.abs_gps_week),
                    static_cast<int>(t_hk_ibox.abs_gps_second));
            hk_ibox_last_gps.assign(str_buffer);
        }
    }
    sprintf(str_buffer,
            "%s [%ld] => %s [%ld] / [%ld]",
            hk_obox_first_gps.c_str(),
            static_cast<long int>(hk_obox_first_entry_),
            hk_obox_last_gps.c_str(),
            static_cast<long int>(hk_obox_last_entry_),
            static_cast<long int>(t_hk_obox_tree_->GetEntries())
        );
    hk_obox_result_str.assign(str_buffer);
    sprintf(str_buffer,
            "%s [%ld] => %s [%ld] / [%ld]",
            hk_ibox_first_gps.c_str(),
            static_cast<long int>(hk_ibox_first_entry_),
            hk_ibox_last_gps.c_str(),
            static_cast<long int>(hk_ibox_last_entry_),
            static_cast<long int>(t_hk_ibox_tree_->GetEntries())
        );
    hk_ibox_result_str.assign(str_buffer);

    cout << name_str_file_in_ << endl;
    cout << " - hk_obox GPS span: { " << hk_obox_result_str << " }" << endl;
    cout << " - hk_ibox GPS span: { " << hk_ibox_result_str << " }" << endl;
}

double HkFileR::value_of_gps_str_(const string gps_str) {
    cmatch cm;
    if (regex_match(gps_str.c_str(), cm, re_gps_)) {
        return TString(cm[1]).Atof() * 604800 + TString(cm[2]).Atof();
    } else {
        return -1;
    }
}

string HkFileR::get_filename() {
    return string(TSystem().BaseName(name_str_file_in_.c_str()));
}

double HkFileR::get_gps_value_first() {
    return gps_value_first_hk_ibox_;
}

double HkFileR::get_gps_value_last() {
    return gps_value_last_hk_ibox_;
}

void HkFileR::hk_obox_set_start() {
    hk_obox_cur_entry_ = hk_obox_first_entry_ - 1;
    hk_obox_reach_end_ = false;
}

Long64_t HkFileR::hk_obox_get_tot_entries() {
    return hk_obox_last_entry_ - hk_obox_first_entry_;
}

Long64_t HkFileR::hk_obox_get_cur_entry() {
    return hk_obox_cur_entry_ - hk_obox_first_entry_;
}

bool HkFileR::hk_obox_next() {
    if (hk_obox_reach_end_)
        return false;
    hk_obox_cur_entry_++;
    if (hk_obox_cur_entry_ < hk_obox_last_entry_) {
        t_hk_obox_tree_->GetEntry(hk_obox_cur_entry_);
        return true;
    } else {
        hk_obox_reach_end_ = true;
        return false;
    }
}

void HkFileR::hk_ibox_set_start() {
    hk_ibox_cur_entry_ = hk_ibox_first_entry_ - 1;
    hk_ibox_reach_end_ = false;
}

Long64_t HkFileR::hk_ibox_get_tot_entries() {
    return hk_ibox_last_entry_ - hk_ibox_first_entry_;
}

Long64_t HkFileR::hk_ibox_get_cur_entry() {
    return hk_ibox_cur_entry_ - hk_ibox_first_entry_;
}

bool HkFileR::hk_ibox_next() {
    if (hk_ibox_reach_end_)
        return false;
    hk_ibox_cur_entry_++;
    if (hk_ibox_cur_entry_ < hk_ibox_last_entry_) {
        t_hk_ibox_tree_->GetEntry(hk_ibox_cur_entry_);
        return true;
    } else {
        hk_ibox_reach_end_ = true;
        return false;
    }
}
