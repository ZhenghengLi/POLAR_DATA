#include "EventIterator.hpp"

EventIterator::EventIterator() {
    re_gps_      = "^ *(\\d+) *: *(\\d+) *$";
    re_gps_span_ = "^ *(\\d+:\\d+)\\[(\\d+)\\] => (\\d+:\\d+)\\[(\\d+)\\]; (\\d+)/(\\d+) *$";
    
    t_file_in_ = NULL;
    t_modules_tree_ = NULL;
    t_trigger_tree_ = NULL;
    t_ped_modules_tree_ = NULL;
    t_ped_trigger_tree_ = NULL;
    m_phy_gps_ = NULL;
    m_ped_gps_ = NULL;
    
    cur_is_1P_ = false;

}

EventIterator::~EventIterator() {
    close();
}

double EventIterator::value_of_gps_str_(const string gps_str) {
    cmatch cm;
    if (regex_match(gps_str.c_str(), cm, re_gps_)) {
        return TString(cm[1]).Atof() * 604800 + TString(cm[2]).Atof();
    } else {
        return -1;
    }
}

Long64_t EventIterator::find_entry_(TTree* t_tree, Trigger_T& t_branch, double gps_value) {
    Long64_t head_entry          = -1;
    bool     head_entry_found    = false;
    while (head_entry < t_tree->GetEntries()) {
        head_entry += 1;
        t_tree->GetEntry(head_entry);
        if (t_branch.abs_gps_valid && t_branch.abs_gps_week > 0 && t_branch.pkt_start >= 0) {
            head_entry_found = true;
            break;
        }
    }
    if (!head_entry_found) {
        return -1;
    }
    if (gps_value < t_branch.abs_gps_week * 604800 + t_branch.abs_gps_second) {
        return -1;
    }
    Long64_t tail_entry          = t_tree->GetEntries();
    bool     tail_entry_found    = false;
    while (tail_entry >= 0) {
        tail_entry -= 1;
        t_tree->GetEntry(tail_entry);
        if (t_branch.abs_gps_valid && t_branch.abs_gps_week > 0 && t_branch.pkt_start >= 0) {
            tail_entry_found = true;
            break;
        }
    }
    if (!tail_entry_found) {
        return -1;
    }
    if (gps_value > t_branch.abs_gps_week * 604800 + t_branch.abs_gps_second) {
        return -1;
    }
    Long64_t center_entry        = 0;
    Long64_t tmp_center_entry    = 0;
    bool     found_valid_center  = false;
    while (tail_entry - head_entry > 1) {
        center_entry = (head_entry + tail_entry) / 2;
        found_valid_center = false;
        t_tree->GetEntry(center_entry);
        if (t_branch.abs_gps_valid && t_branch.abs_gps_week > 0 && t_branch.pkt_start >= 0) {
            found_valid_center = true;
        }
        tmp_center_entry = center_entry;
        while (!found_valid_center && tail_entry - tmp_center_entry > 1) {
            tmp_center_entry += 1;
            t_tree->GetEntry(tmp_center_entry);
            if (t_branch.abs_gps_valid && t_branch.abs_gps_week > 0 && t_branch.pkt_start >= 0) {
                found_valid_center = true;
            }
        }
        if (!found_valid_center) {
            tmp_center_entry = center_entry;
        }
        while (!found_valid_center && tmp_center_entry - head_entry > 1) {
            tmp_center_entry -= 1;
            t_tree->GetEntry(tmp_center_entry);
            if (t_branch.abs_gps_valid && t_branch.abs_gps_week > 0 && t_branch.pkt_start >= 0) {
                found_valid_center = true;
            }
        }
        if (!found_valid_center) {
            break;
        }
        if (gps_value == t_branch.abs_gps_week * 604800 + t_branch.abs_gps_second) {
            return tmp_center_entry;
        } else if (gps_value > t_branch.abs_gps_week * 604800 + t_branch.abs_gps_second) {
            head_entry = tmp_center_entry;
        } else {
            tail_entry = tmp_center_entry;
        }
    }
    return tail_entry;
}

bool EventIterator::open(const char* filename) {
    return open(filename, "begin", "end");
}

bool EventIterator::open(const char* filename, const char* gps_begin, const char* gps_end) {
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
        
    // open file and check
    name_str_file_in_.assign(filename);
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_trigger_tree_ = static_cast<TTree*>(t_file_in_->Get("t_trigger"));
    if (t_trigger_tree_ == NULL)
        return false;
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
        cerr << "File: " << filename << " may be empty." << endl;
        return false;
    }

    // bind TTree
    cur_is_1P_ = check_1P(t_trigger_tree_);
    bind_trigger_tree(t_trigger_tree_, t_trigger);
    bind_modules_tree(t_modules_tree_, t_modules);
    bind_trigger_tree(t_ped_trigger_tree_, t_ped_trigger);
    bind_modules_tree(t_ped_modules_tree_, t_ped_modules);

    if (cur_is_1P_) {
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
            if (gps_str_end_ == "end" && min(gps_value_last_ped_, gps_value_last_phy_) - gps_value_begin_ < GPS_DIFF_MIN) {
                cerr << "GPS string of beginning is too large: "
                     << min(gps_value_last_ped_, gps_value_last_phy_) - gps_value_begin_
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
            if (gps_str_begin_ == "begin" && gps_value_end_ - max(gps_value_first_ped_, gps_value_first_phy_) < GPS_DIFF_MIN) {
                cerr << "GPS string of ending is too small: "
                     << gps_value_end_ - max(gps_value_first_ped_, gps_value_first_phy_)
                     << " seconds" << endl;
                return false;
            }
        }
    }

    // find the first and last entry
    // for 1M Level
    if (!cur_is_1P_) {
        phy_trigger_first_entry_ = 0;
        phy_modules_first_entry_ = 0;
        ped_trigger_first_entry_ = 0;
        ped_modules_first_entry_ = 0;
        phy_trigger_last_entry_  = t_trigger_tree_->GetEntries();
        phy_modules_last_entry_  = t_modules_tree_->GetEntries();
        ped_trigger_last_entry_  = t_ped_trigger_tree_->GetEntries();
        ped_modules_last_entry_  = t_ped_modules_tree_->GetEntries();

        // find start and stop trigger packet
        // phy_begin_trigger
        bool found_valid = false;
        for (Long64_t i = phy_trigger_first_entry_; i < phy_trigger_last_entry_; i++) {
            t_trigger_tree_->GetEntry(i);
            if (t_trigger.is_bad == 0) {
                found_valid = true;
                phy_begin_trigger = t_trigger;
                break;
            }
        }
        if (!found_valid) {
            cout << "Cannot find phy_begin_trigger." << endl;
            return false;
        }
        // phy_end_trigger
        found_valid = false;
        for (Long64_t i = phy_trigger_last_entry_ - 1; i >= phy_trigger_first_entry_; i--) {
            t_trigger_tree_->GetEntry(i);
            if (t_trigger.is_bad == 0) {
                found_valid = true;
                phy_end_trigger = t_trigger;
                break;
            }
        }
        if (!found_valid) {
            cout << "Cannot find phy_end_trigger." << endl;
            return false;
        }
        // ped_begin_trigger
        found_valid = false;
        for (Long64_t i = ped_trigger_first_entry_; i < ped_trigger_last_entry_; i++) {
            t_ped_trigger_tree_->GetEntry(i);
            if (t_ped_trigger.is_bad == 0) {
                found_valid = true;
                ped_begin_trigger = t_ped_trigger;
                break;
            }
        }
        if (!found_valid) {
            cout << "Cannot find ped_begin_trigger." << endl;
            return false;
        }
        // ped_end_trigger
        found_valid = false;
        for (Long64_t i = ped_trigger_last_entry_; i >= ped_trigger_first_entry_; i--) {
            t_ped_trigger_tree_->GetEntry(i);
            if (t_ped_trigger.is_bad == 0) {
                found_valid = true;
                ped_end_trigger = t_ped_trigger;
                break;
            }
        }
        if (!found_valid) {
            cout << "Cannot find ped_end_trigger." << endl;
            return false;
        }
        
        return true;
    }

    // for 1P/1R Level
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
            // phy 
            phy_trigger_last_entry_ = find_entry_(t_trigger_tree_, t_trigger, gps_value_end_);
            if (phy_trigger_last_entry_ < 0) {
                cerr << "Cannot find the last valid entry of t_trigger." << endl;
                return false;
            }
            t_trigger_tree_->GetEntry(phy_trigger_last_entry_);
            phy_modules_last_entry_ = t_trigger.pkt_start;
            // ped
            ped_trigger_last_entry_ = find_entry_(t_ped_trigger_tree_, t_ped_trigger, gps_value_end_);
            if (ped_trigger_last_entry_ < 0) {
                cerr << "Cannot find the last valid entry of t_ped_trigger." << endl;
                return false;
            }
            t_ped_trigger_tree_->GetEntry(ped_trigger_last_entry_);
            ped_modules_last_entry_ = t_ped_trigger.pkt_start;
        }
    } else {
        if (gps_str_end_ == "end") {
            phy_trigger_last_entry_ = t_trigger_tree_->GetEntries();
            phy_modules_last_entry_ = t_modules_tree_->GetEntries();
            ped_trigger_last_entry_ = t_ped_trigger_tree_->GetEntries();
            ped_modules_last_entry_ = t_ped_modules_tree_->GetEntries();
            // phy 
            phy_trigger_first_entry_ = find_entry_(t_trigger_tree_, t_trigger, gps_value_begin_);
            if (phy_trigger_first_entry_ < 0) {
                cerr << "Cannot find the first valid entry of t_trigger." << endl;
                return false;
            }
            t_trigger_tree_->GetEntry(phy_trigger_first_entry_);
            phy_modules_first_entry_ = t_trigger.pkt_start;
            // ped
            ped_trigger_first_entry_ = find_entry_(t_ped_trigger_tree_, t_ped_trigger, gps_value_begin_);
            if (ped_trigger_first_entry_ < 0) {
                cerr << "Cannot find the first valid entry of t_ped_trigger." << endl;
                return false;
            }
            t_ped_trigger_tree_->GetEntry(ped_trigger_first_entry_);
            ped_modules_first_entry_ = t_ped_trigger.pkt_start;
        } else {
            // phy first
            phy_trigger_first_entry_ = find_entry_(t_trigger_tree_, t_trigger, gps_value_begin_);
            if (phy_trigger_first_entry_ < 0) {
                cerr << "Cannot find the first valid entry of t_trigger." << endl;
                return false;
            }
            t_trigger_tree_->GetEntry(phy_trigger_first_entry_);
            phy_modules_first_entry_ = t_trigger.pkt_start;
            // phy last
            phy_trigger_last_entry_ = find_entry_(t_trigger_tree_, t_trigger, gps_value_end_);
            if (phy_trigger_last_entry_ < 0) {
                cerr << "Cannot find the last valid entry of t_trigger." << endl;
                return false;
            }
            t_trigger_tree_->GetEntry(phy_trigger_last_entry_);
            phy_modules_last_entry_ = t_trigger.pkt_start;
            // ped first
            ped_trigger_first_entry_ = find_entry_(t_ped_trigger_tree_, t_ped_trigger, gps_value_begin_);
            if (ped_trigger_first_entry_ < 0) {
                cerr << "Cannot find the first valid entry of t_ped_trigger." << endl;
                return false;
            }
            t_ped_trigger_tree_->GetEntry(ped_trigger_first_entry_);
            ped_modules_first_entry_ = t_ped_trigger.pkt_start;
            // ped last
            ped_trigger_last_entry_ = find_entry_(t_ped_trigger_tree_, t_ped_trigger, gps_value_end_);
            if (ped_trigger_last_entry_ < 0) {
                cerr << "Cannot find the last valid entry of t_ped_trigger." << endl;
                return false;
            }
            t_ped_trigger_tree_->GetEntry(ped_trigger_last_entry_);
            ped_modules_last_entry_ = t_ped_trigger.pkt_start;
        }
    }

    // find start and stop trigger packet
    // phy_begin_trigger
    bool found_valid = false;
    for (Long64_t i = phy_trigger_first_entry_; i < phy_trigger_last_entry_; i++) {
        t_trigger_tree_->GetEntry(i);
        if (t_trigger.abs_gps_valid && t_trigger.abs_gps_week > 0) {
            found_valid = true;
            phy_begin_trigger = t_trigger;
            break;
        }
    }
    if (!found_valid) {
        cout << "Cannot find phy_begin_trigger." << endl;
        return false;
    }
    // phy_end_trigger
    found_valid = false;
    Long64_t cur_last_entry = (phy_trigger_last_entry_ >= t_trigger_tree_->GetEntries() ? t_trigger_tree_->GetEntries() - 1 : phy_trigger_last_entry_);
    for (Long64_t i = cur_last_entry; i >= phy_trigger_first_entry_; i--) {
        t_trigger_tree_->GetEntry(i);
        if (t_trigger.abs_gps_valid && t_trigger.abs_gps_week > 0) {
            found_valid = true;
            phy_end_trigger = t_trigger;
            break;
        }
    }
    if (!found_valid) {
        cout << "Cannot find phy_end_trigger." << endl;
        return false;
    }
    // ped_begin_trigger
    found_valid = false;
    for (Long64_t i = ped_trigger_first_entry_; i < ped_trigger_last_entry_; i++) {
        t_ped_trigger_tree_->GetEntry(i);
        if (t_ped_trigger.abs_gps_valid && t_trigger.abs_gps_week > 0) {
            found_valid = true;
            ped_begin_trigger = t_ped_trigger;
            break;
        }
    }
    if (!found_valid) {
        cout << "Cannot find ped_begin_trigger." << endl;
        return false;
    }
    // ped_end_trigger
    found_valid = false;
    cur_last_entry = (ped_trigger_last_entry_ >= t_ped_trigger_tree_->GetEntries() ? t_ped_trigger_tree_->GetEntries() - 1 : ped_trigger_last_entry_);
    for (Long64_t i = cur_last_entry; i >= ped_trigger_first_entry_; i--) {
        t_ped_trigger_tree_->GetEntry(i);
        if (t_ped_trigger.abs_gps_valid && t_trigger.abs_gps_week > 0) {
            found_valid = true;
            ped_end_trigger = t_ped_trigger;
            break;
        }
    }
    if (!found_valid) {
        cout << "Cannot find ped_end_trigger." << endl;
        return false;
    }
    
    return true;
}

void EventIterator::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    delete t_file_in_;
    t_file_in_ = NULL;
    t_modules_tree_ = NULL;
    t_trigger_tree_ = NULL;
    t_ped_modules_tree_ = NULL;
    t_ped_trigger_tree_ = NULL;
    m_phy_gps_ = NULL;
    m_ped_gps_ = NULL;
}

void EventIterator::print_file_info() {
    if (t_file_in_ == NULL)
        return;
    if (!cur_is_1P_) {
        cout << name_str_file_in_ << endl;
        cout << " - This may be 1M level SCI data, all data in this file will be iterated." << endl;
        return;
    }
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

string EventIterator::get_filename() {
    if (t_file_in_ == NULL)
        return "";
    TSystem sys;
    return string(sys.BaseName(name_str_file_in_.c_str()));
}

string EventIterator::get_ped_first_gps() {
    if (t_file_in_ == NULL)
        return "";
    char str_buffer[50];
    if (cur_is_1P_) {
        for (Long64_t i = ped_trigger_first_entry_; i < ped_trigger_last_entry_; i++) {
            t_ped_trigger_tree_->GetEntry(i);
            if (t_ped_trigger.abs_gps_week >= 0 && t_ped_trigger.abs_gps_second >= 0 &&
                t_ped_trigger.abs_gps_valid && t_ped_trigger.abs_gps_week > 0)
                break;
        }
        sprintf(str_buffer, "%d:%d",
                static_cast<int>(t_ped_trigger.abs_gps_week),
                static_cast<int>(t_ped_trigger.abs_gps_second));
        return string(str_buffer);
    } else {
        return "begin";
    }
}

string EventIterator::get_ped_last_gps() {
    if (t_file_in_ == NULL)
        return "";
    char str_buffer[50];
    if (cur_is_1P_) {
        for (Long64_t i = ped_trigger_last_entry_ - 1; i >= ped_trigger_first_entry_; i--) {
            t_ped_trigger_tree_->GetEntry(i);
            if (t_ped_trigger.abs_gps_week >= 0 && t_ped_trigger.abs_gps_second >= 0 &&
                t_ped_trigger.abs_gps_valid && t_ped_trigger.abs_gps_week > 0)
                break;
        }
        sprintf(str_buffer, "%d:%d",
                static_cast<int>(t_ped_trigger.abs_gps_week),
                static_cast<int>(t_ped_trigger.abs_gps_second));
        return string(str_buffer);
    } else {
        return "end";
    }
}

string EventIterator::get_phy_first_gps() {
    if (t_file_in_ == NULL)
        return "";
    char str_buffer[50];
    if (cur_is_1P_) {
        for (Long64_t i = phy_trigger_first_entry_; i < phy_trigger_last_entry_; i++) {
            t_trigger_tree_->GetEntry(i);
            if (t_trigger.abs_gps_week >= 0 && t_trigger.abs_gps_second >= 0 &&
                t_trigger.abs_gps_valid && t_ped_trigger.abs_gps_week > 0)
                break;
        }
        sprintf(str_buffer, "%d:%d",
                static_cast<int>(t_trigger.abs_gps_week),
                static_cast<int>(t_trigger.abs_gps_second));
        return string(str_buffer);
    } else {
        return "begin";
    }
}

string EventIterator::get_phy_last_gps() {
    if (t_file_in_ == NULL)
        return "";
    char str_buffer[50];
    if (cur_is_1P_) {
        for (Long64_t i = phy_trigger_last_entry_ - 1; i >= phy_trigger_first_entry_; i--) {
            t_trigger_tree_->GetEntry(i);
            if (t_trigger.abs_gps_week >= 0 && t_trigger.abs_gps_second >= 0 &&
                t_trigger.abs_gps_valid && t_trigger.abs_gps_week > 0)
                break;
        }
        sprintf(str_buffer, "%d:%d",
                static_cast<int>(t_trigger.abs_gps_week),
                static_cast<int>(t_trigger.abs_gps_second));
        return string(str_buffer);
    } else {
        return "end";
    }
}

void EventIterator::phy_trigger_set_start() {
    phy_trigger_cur_entry_ = phy_trigger_first_entry_ - 1;
    phy_trigger_reach_end_ = false;
}

Long64_t EventIterator::phy_trigger_get_tot_entries() {
    return phy_trigger_last_entry_ - phy_trigger_first_entry_;
}

Long64_t EventIterator::phy_trigger_get_cur_entry() {
    return phy_trigger_cur_entry_ - phy_trigger_first_entry_;
}

bool EventIterator::phy_trigger_next_event() {
    if (phy_trigger_reach_end_)
        return false;
    do {
        phy_trigger_cur_entry_++;
        if (phy_trigger_cur_entry_ < phy_trigger_last_entry_) {
            t_trigger_tree_->GetEntry(phy_trigger_cur_entry_);
        } else {
            phy_trigger_reach_end_ = true;
            return false;
        }
    } while (t_trigger.is_bad > 0);
    phy_modules_event_start_entry_ = t_trigger.pkt_start;
    phy_modules_event_stop_entry_  = t_trigger.pkt_start + t_trigger.pkt_count;
    phy_modules_set_start();
    return true;
}

void EventIterator::phy_modules_set_start() {
    phy_modules_event_cur_entry_ = phy_modules_event_start_entry_ - 1;
    phy_modules_event_reach_end_ = false;
}

bool EventIterator::phy_modules_next_packet() {
    if (phy_modules_event_reach_end_)
        return false;
    phy_modules_event_cur_entry_++;
    if (phy_modules_event_cur_entry_ < phy_modules_event_stop_entry_) {
        t_modules_tree_->GetEntry(phy_modules_event_cur_entry_);
        return true;
    } else {
        phy_modules_event_reach_end_ = true;
        return false;
    }
}

void EventIterator::ped_trigger_set_start() {
    ped_trigger_cur_entry_ = ped_trigger_first_entry_ - 1;
    ped_trigger_reach_end_ = false;
}

Long64_t EventIterator::ped_trigger_get_tot_entries() {
    return ped_trigger_last_entry_ - ped_trigger_first_entry_;
}

Long64_t EventIterator::ped_trigger_get_cur_entry() {
    return ped_trigger_cur_entry_ - ped_trigger_first_entry_;
}

bool EventIterator::ped_trigger_next_event() {
    if (ped_trigger_reach_end_)
        return false;
    do {
        ped_trigger_cur_entry_++;
        if (ped_trigger_cur_entry_ < ped_trigger_last_entry_) {
            t_ped_trigger_tree_->GetEntry(ped_trigger_cur_entry_);
        } else {
            ped_trigger_reach_end_ = true;
            return false;
        }
    } while (t_ped_trigger.is_bad > 0);
    ped_modules_event_start_entry_ = t_ped_trigger.pkt_start;
    ped_modules_event_stop_entry_  = t_ped_trigger.pkt_start + t_ped_trigger.pkt_count;
    ped_modules_set_start();
    return true;
}

void EventIterator::ped_modules_set_start() {
    ped_modules_event_cur_entry_ = ped_modules_event_start_entry_ - 1;
    ped_modules_event_reach_end_ = false;
}

bool EventIterator::ped_modules_next_packet() {
    if (ped_modules_event_reach_end_)
        return false;
    ped_modules_event_cur_entry_++;
    if (ped_modules_event_cur_entry_ < ped_modules_event_stop_entry_) {
        t_ped_modules_tree_->GetEntry(ped_modules_event_cur_entry_);
        return true;
    } else {
        ped_modules_event_reach_end_ = true;
        return false;
    }
}

bool EventIterator::file_is_1P() {
    return cur_is_1P_;
}
