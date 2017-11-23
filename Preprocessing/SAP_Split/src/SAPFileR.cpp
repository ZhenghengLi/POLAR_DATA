#include "SAPFileR.hpp"

using namespace std;

SAPFileR::SAPFileR() {
    re_time_span_ = "^ *(\\d+)\\[\\d+\\] => (\\d+)\\[\\d+\\] *$";
    t_file_in_ = NULL;
    t_pol_event_tree_ = NULL;
    m_time_span_ = NULL;
}

SAPFileR::~SAPFileR() {
    if (t_file_in_ != NULL)
        close();
}

Long64_t SAPFileR::find_entry_(double event_time) {
    if (t_file_in_ == NULL || t_pol_event_tree_ == NULL)
        return -1;
    Long64_t head_entry = 0;
    Long64_t tail_entry = t_pol_event_tree_->GetEntries() - 1;
    Long64_t center_entry = 0;
    while (tail_entry - head_entry > 1) {
        center_entry = (head_entry + tail_entry) / 2;
        t_pol_event_tree_->GetEntry(center_entry);
        if (t_pol_event.event_time == event_time) {
            return center_entry;
        } else if (t_pol_event.event_time < event_time) {
            head_entry = center_entry;
        } else {
            tail_entry = center_entry;
        }
    }
    return tail_entry;
}

bool SAPFileR::open(const char* filename, const char* time_begin, const char* time_end) {
    if (t_file_in_ != NULL)
        return false;

    // record time span
    time_begin_str_ = time_begin;
    if (time_begin_str_ == "begin") {
        time_begin_value_ = -2 * TIME_SPAN_MIN;
    } else if (time_begin_str_.IsFloat()) {
        time_begin_value_ = time_begin_str_.Atof();
    } else {
        cout << "the input begin time is invalid: " << time_begin_str_ << endl;
        return false;
    }
    time_end_str_ = time_end;
    if (time_end_str_ == "end") {
        time_end_value_ = numeric_limits<double>::max() - 3 * TIME_SPAN_MIN;
    } else if (time_end_str_.IsFloat()) {
        time_end_value_ = time_end_str_.Atof();
    } else {
        cout << "the input end time is invalid: " << time_end_str_ << endl;
        return false;
    }
    if (time_end_value_ - time_begin_value_ < TIME_SPAN_MIN) {
        cout << "the input time span is too small: "
             << time_end_value_ - time_begin_value_ << " seconds" << endl;
        return false;
    }

    // open file and check
    filename_str_ = filename;
    t_file_in_ = new TFile(filename_str_.c_str(), "read");
    if (t_file_in_->IsZombie())
        return false;
    t_pol_event_tree_ = static_cast<TTree*>(t_file_in_->Get("t_pol_event"));
    if (t_pol_event_tree_ == NULL)
        return false;
    if (t_pol_event_tree_->GetEntries() < 1) {
        cout << "File: " << filename << " may be empty." << endl;
        return false;
    }
    // find the first and last time
    m_time_span_ = static_cast<TTree*>(t_file_in_->Get("m_time_span"));
    if (m_time_span_ == NULL)
        return false;
    cmatch cm;
    if (regex_match(m_time_span_->GetTitle(), cm, re_time_span_)) {
        time_first_ = TString(cm[1]).Atof();
        time_last_  = TString(cm[2]).Atof();
    } else {
        cout << "time span regex match failed." << endl;
        return false;
    }
    // check time span selection
    if (time_begin_str_ != "begin") {
        if (time_begin_value_ < time_first_) {
            cout << "time string of beginning is out of range: "
                 << time_first_ - time_begin_value_ << " seconds" << endl;
            return false;
        }
        if (time_end_str_ == "end" && time_last_ - time_begin_value_ < TIME_DIFF_MIN) {
            cout << "time string of beginning is too large: "
                 << time_last_ - time_begin_value_ << " seconds" << endl;
            return false;
        }
    }
    if (time_end_str_ != "end") {
        if (time_end_value_ > time_last_) {
            cout << "time string of ending is out of range: "
                 << time_end_value_ - time_last_ << " seconds" << endl;
            return false;
        }
        if (time_begin_str_ == "begin" && time_end_value_ - time_first_ < TIME_DIFF_MIN) {
            cout << "time string of ending is too small: "
                 << time_end_value_ - time_first_ << " seconds" << endl;
            return false;
        }
    }

    // read energy_unit and level_num
    TNamed* tmp_tnamed;
    tmp_tnamed = static_cast<TNamed*>(t_file_in_->Get("m_energy_unit"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed m_energy_unit." << endl;
        return false;
    } else {
        energy_unit_ = tmp_tnamed->GetTitle();
    }
    tmp_tnamed = static_cast<TNamed*>(t_file_in_->Get("m_level_num"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed m_level_num." << endl;
        return false;
    } else {
        level_num_ = tmp_tnamed->GetTitle();
    }

    // bind tree
    bind_pol_event_tree(t_pol_event_tree_, t_pol_event);
    // find entry_begin_ and entry_end_
    if (time_begin_str_ == "begin") {
        entry_begin_ = 0;
    } else {
        entry_begin_ = find_entry_(time_begin_value_);
        if (entry_begin_ < 0) {
            cout << "Cannot find the begin entry of t_pol_event." << endl;
            return false;
        }
    }
    if (time_end_str_ == "end") {
        entry_end_ = t_pol_event_tree_->GetEntries();
    } else {
        entry_end_ = find_entry_(time_end_value_);
        if (entry_end_ < 0) {
            cout << "Cannot find the end entry of t_pol_event." << endl;
            return false;
        }
    }
    if (entry_end_ - entry_begin_ < 10) {
        cout << "Data in the time range maybe empty." << endl;
        return false;
    }

    return true;
}

void SAPFileR::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    delete t_file_in_;
    t_file_in_ = NULL;
    t_pol_event_tree_ = NULL;
}

void SAPFileR::print_file_info() {
    if (t_file_in_ == NULL || t_pol_event_tree_ == NULL) {
        cout << "SAP file is not open yet." << endl;
        return;
    }
    t_pol_event_tree_->GetEntry(entry_begin_);
    double begin_time = t_pol_event.event_time;
    t_pol_event_tree_->GetEntry(entry_end_ - 1);
    double end_time   = t_pol_event.event_time;
    char time_span_buffer[100];
    sprintf(time_span_buffer, "%d[%ld] => %d[%ld] / [%ld]",
            static_cast<int>(begin_time),
            static_cast<long int>(entry_begin_),
            static_cast<int>(end_time),
            static_cast<long int>(entry_end_ - 1),
            static_cast<long int>(t_pol_event_tree_->GetEntries()));
    cout << filename_str_ << endl;
    cout << " - MET TIME SPAN: { " << time_span_buffer << " }" << endl;
}

string SAPFileR::get_filename() {
    return filename_str_;
}

double SAPFileR::get_time_first() {
    return time_first_;
}

double SAPFileR::get_time_last() {
    return time_last_;
}

string SAPFileR::get_energy_unit() {
    return energy_unit_;
}

string SAPFileR::get_level_num() {
    return level_num_;
}

void SAPFileR::pol_event_set_start() {
    pol_event_cur_entry_ = entry_begin_ - 1;
    pol_event_reach_end_ = false;
}

bool SAPFileR::pol_event_next() {
    if (t_file_in_ == NULL || t_pol_event_tree_ == NULL)
        return false;
    if (pol_event_reach_end_)
        return false;
    pol_event_cur_entry_++;
    if (pol_event_cur_entry_ < entry_end_) {
        t_pol_event_tree_->GetEntry(pol_event_cur_entry_);
        return true;
    } else {
        pol_event_reach_end_ = true;
        return false;
    }
    return true;
}

Long64_t SAPFileR::pol_event_get_cur_entry() {
    return pol_event_cur_entry_ - entry_begin_;
}

Long64_t SAPFileR::pol_event_get_tot_entries() {
    return entry_end_ - entry_begin_;
}

Long64_t SAPFileR::pol_event_get_entry_begin() {
    return entry_begin_;
}

Long64_t SAPFileR::pol_event_get_entry_end() {
    return entry_end_;
}
