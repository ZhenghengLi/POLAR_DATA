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

    bind_hk_obox_tree(t_hk_obox_tree_, t_hk_obox);
    bind_hk_ibox_tree(t_hk_ibox_tree_, t_hk_ibox);

    // find the firest and last entry
    char str_buffer[200];
    TEventList* cur_elist;
    if (gps_str_begin_ == "begin") {
        hk_obox_first_entry_ = 0;
        hk_ibox_first_entry_ = 0;
        if (gps_str_end_ == "end") {
            hk_obox_last_entry_ = t_hk_obox_tree_->GetEntries();
            hk_ibox_last_entry_ = t_hk_ibox_tree_->GetEntries();
        } else {

        }
    } else {
        if (gps_str_end_ == "end") {

        } else {

        }
    }
    
    
    return true;
}

void HkFileR::close() {

}

void HkFileR::print_file_info() {

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

    return "";
}

double HkFileR::get_gps_value_first() {

    return 0;
}

double HkFileR::get_gps_value_last() {

    return 0;
}

void HkFileR::hk_obox_set_start() {

}

Long64_t HkFileR::hk_obox_get_tot_entries() {

    return 0;
}

Long64_t HkFileR::hk_obox_get_cur_entry() {
    return 0;
}

bool HkFileR::hk_obox_next() {

    return true;
}

void HkFileR::hk_ibox_set_start() {

}

Long64_t HkFileR::hk_ibox_get_tot_entries() {

    return 0;
}

Long64_t HkFileR::hk_ibox_get_cur_entry() {

    return 0;
}

bool HkFileR::hk_ibox_next() {

    return true;
}
