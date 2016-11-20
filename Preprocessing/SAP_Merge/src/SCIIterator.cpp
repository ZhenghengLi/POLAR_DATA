#include "SCIIterator.hpp"

using namespace std;

SCIIterator::SCIIterator() {
    t_file_in_ = NULL;
    t_trigger_tree_ = NULL;
    t_modules_tree_ = NULL;
    t_ped_trigger_tree_ = NULL;
    t_ped_modules_tree_ = NULL;

    re_ship_span_ = "^ *(\\d+)\\[\\d+\\] => (\\d+)\\[\\d+\\]; \\d+/\\d+ *$";

    total_entries_ = 0;
    bad_entries_   = 0;
}

SCIIterator::~SCIIterator() {
    if (t_file_in_ != NULL)
        close();
}

bool SCIIterator::open(const char* filename) {
    if (t_file_in_ != NULL)
        return false;
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

    if (!check_1P(t_trigger_tree_))
        return false;

    TNamed* m_pedship = static_cast<TNamed*>(t_file_in_->Get("m_pedship"));
    if (m_pedship == NULL)
        return false;
    cmatch cm;
    if (regex_match(m_pedship->GetTitle(), cm, re_ship_span_)) {
        first_ship_second_ = TString(cm[1]).Atof();
        last_ship_second_  = TString(cm[2]).Atof();
    } else {
        cerr << "ship time span match failed: " << m_pedship->GetTitle() << endl;
        return false;
    }
    TNamed* m_version = static_cast<TNamed*>(t_file_in_->Get("m_version"));
    if (m_version == NULL)
        return false;
    else
        m_version_value_ = m_version->GetTitle();
    TNamed* m_gentime = static_cast<TNamed*>(t_file_in_->Get("m_gentime"));
    if (m_gentime == NULL)
        return false;
    else
        m_gentime_value_ = m_gentime->GetTitle();
    TNamed* m_rawfile = static_cast<TNamed*>(t_file_in_->Get("m_rawfile"));
    if (m_rawfile == NULL)
        return false;
    else
        m_rawfile_value_ = m_rawfile->GetTitle();
    TNamed* m_dcdinfo = static_cast<TNamed*>(t_file_in_->Get("m_dcdinfo"));
    if (m_dcdinfo == NULL)
        return false;
    else
        m_dcdinfo_value_ = m_dcdinfo->GetTitle();

    total_entries_ = t_trigger_tree_->GetEntries() + t_ped_trigger_tree_->GetEntries();
    bad_entries_ = 0;
    bad_time_entries_ = 0;

    bind_trigger_tree(t_trigger_tree_, phy_trigger_);
    bind_trigger_tree(t_ped_trigger_tree_, ped_trigger_);
    bind_modules_tree(t_modules_tree_, phy_modules_);
    bind_modules_tree(t_ped_modules_tree_, ped_modules_);

    set_start();

    return true;
}

void SCIIterator::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    t_file_in_ = NULL;
    t_trigger_tree_ = NULL;
    t_modules_tree_ = NULL;
    t_ped_trigger_tree_ = NULL;
    t_ped_modules_tree_ = NULL;
}

void SCIIterator::set_start() {
    ped_trigger_cur_entry_ = -1;
    ped_trigger_reach_end_ = false;
    ped_trigger_next_();
    phy_trigger_cur_entry_ = -1;
    phy_trigger_reach_end_ = false;
    phy_trigger_next_();
}

bool SCIIterator::ped_trigger_next_() {
    if (t_file_in_ == NULL)
        return false;
    if (ped_trigger_reach_end_)
        return false;
    do {
        ped_trigger_cur_entry_++;
        if (ped_trigger_cur_entry_ < t_ped_trigger_tree_->GetEntries()) {
            t_ped_trigger_tree_->GetEntry(ped_trigger_cur_entry_);
            if (ped_trigger_.is_bad > 0)
                bad_entries_++;
            if (!ped_trigger_.abs_gps_valid || ped_trigger_.abs_gps_week < 0)
                bad_time_entries_++;
        } else {
            ped_trigger_reach_end_ = true;
            return false;
        }
    } while (ped_trigger_.is_bad > 0 || !ped_trigger_.abs_gps_valid || ped_trigger_.abs_gps_week < 0);
    cur_ped_pkt_start_ = ped_trigger_.pkt_start;
    cur_ped_pkt_count_ = ped_trigger_.pkt_count;
    ped_modules_cur_entry_ = cur_ped_pkt_start_ - 1;
    return true;
}

bool SCIIterator::phy_trigger_next_() {
    if (t_file_in_ == NULL)
        return false;
    if (phy_trigger_reach_end_)
        return false;
    do {
        phy_trigger_cur_entry_++;
        if (phy_trigger_cur_entry_ < t_trigger_tree_->GetEntries()) {
            t_trigger_tree_->GetEntry(phy_trigger_cur_entry_);
            if (phy_trigger_.is_bad > 0)
                bad_entries_++;
            if (!phy_trigger_.abs_gps_valid || phy_trigger_.abs_gps_week < 0)
                bad_time_entries_++;
        } else {
            phy_trigger_reach_end_ = true;
            return false;
        }
    } while (phy_trigger_.is_bad > 0 || !phy_trigger_.abs_gps_valid || phy_trigger_.abs_gps_week < 0);
    cur_phy_pkt_start_ = phy_trigger_.pkt_start;
    cur_phy_pkt_count_ = phy_trigger_.pkt_count;
    phy_modules_cur_entry_ = cur_phy_pkt_start_ - 1;
    return true;
}

bool SCIIterator::next_event() {
    if (ped_trigger_reach_end_ && phy_trigger_reach_end_)
        return false;
    if (ped_trigger_reach_end_) {
        cur_trigger = phy_trigger_;
        cur_is_ped_ = false;
    } else if (phy_trigger_reach_end_) {
        cur_trigger = ped_trigger_;
        cur_is_ped_ = true;
    } else {
        if (ped_trigger_.trigg_num_g < phy_trigger_.trigg_num_g) {
            cur_trigger = ped_trigger_;
            cur_is_ped_ = true;
        } else {
            cur_trigger = phy_trigger_;
            cur_is_ped_ = false;
        }
    }
    if (cur_is_ped_) {
        ped_trigger_next_();
    } else {
        phy_trigger_next_();
    }
    return true;
}

bool SCIIterator::next_packet() {
    if (cur_is_ped_) {
        ped_modules_cur_entry_++;
        if (ped_modules_cur_entry_ < cur_ped_pkt_start_ + cur_ped_pkt_count_) {
            t_ped_modules_tree_->GetEntry(ped_modules_cur_entry_);
            cur_modules = ped_modules_;
        } else {
            return false;
        }
    } else {
        phy_modules_cur_entry_++;
        if (phy_modules_cur_entry_ < cur_phy_pkt_start_ + cur_phy_pkt_count_) {
            t_modules_tree_->GetEntry(phy_modules_cur_entry_);
            cur_modules = phy_modules_;
        } else {
            return false;
        }
    }
    return true;
}

double SCIIterator::get_first_ship_second() {
    return first_ship_second_;
}

double SCIIterator::get_last_ship_second() {
    return last_ship_second_;
}

string SCIIterator::get_bad_ratio_str() {
    if (ped_trigger_reach_end_ && phy_trigger_reach_end_) {
        return string(Form("bad_packet: %ld/%ld, bad_time: %ld/%ld",
                    static_cast<long int>(bad_entries_),
                    static_cast<long int>(total_entries_),
                    static_cast<long int>(bad_time_entries_),
                    static_cast<long int>(total_entries_)));
    } else {
        return string("not_reach_end");
    }
}

