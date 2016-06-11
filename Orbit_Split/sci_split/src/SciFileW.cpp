#include "SciFileW.hpp"

SciFileW::SciFileW() {
    t_file_out_ = NULL;
    t_modules_tree_ = NULL;
    t_trigger_tree_ = NULL;
    t_ped_modules_tree_ = NULL;
    t_ped_trigger_tree_ = NULL;

    cur_scifile_r = NULL;
}

SciFileW::~SciFileW() {
    close();
}


bool SciFileW::open(const char* filename) {
    if (t_file_out_ != NULL)
        return false;
    
    t_file_out_ = new TFile(filename, "RECREATE");
    if (t_file_out_->IsZombie())
        return false;

    // t_modules
    t_modules_tree_ = new TTree("t_modules", "physical modules packets");
    build_modules_tree(t_modules_tree_, t_modules);

    // t_trigger
    t_trigger_tree_ = new TTree("t_trigger", "physical trigger packets");
    build_trigger_tree(t_trigger_tree_, t_trigger);

    // t_ped_modules
    t_ped_modules_tree_ = new TTree("t_ped_modules", "pedestal modules packets");
    build_modules_tree(t_ped_modules_tree_, t_ped_modules);

    // t_ped_trigger
    t_ped_trigger_tree_ = new TTree("t_ped_trigger", "pedestal trigger packets");
    build_trigger_tree(t_ped_trigger_tree_, t_ped_trigger);

    cur_scifile_r = NULL;
    
    return true;
}

void SciFileW::close() {
    if (t_file_out_ == NULL)
        return;

    delete t_modules_tree_;
    t_modules_tree_ = NULL;

    delete t_trigger_tree_;
    t_trigger_tree_ = NULL;

    delete t_ped_modules_tree_;
    t_ped_modules_tree_ = NULL;

    delete t_ped_trigger_tree_;
    t_ped_trigger_tree_ = NULL;

    t_file_out_->Close();
    delete t_file_out_;
    t_file_out_ = NULL;
}

void SciFileW::write_before_close() {
    t_modules_tree_->Write();
    t_trigger_tree_->Write();
    t_ped_modules_tree_->Write();
    t_ped_trigger_tree_->Write();
}

void SciFileW::set_scifile_r(SciFileR* scifile_r) {
    if (cur_scifile_r == NULL) {
        cur_phy_trigg_num_offset_               = 0;
        cur_ped_trigg_num_offset_               = 0;
        cur_trigg_num_g_offset_                 = 0;
        cur_trigger_time_period_offset_         = 0;
        for (int i = 0; i < 25; i++) {
            cur_phy_event_num_offset_[i]        = 0;
            cur_ped_event_num_offset_[i]        = 0;
            cur_event_num_g_offset_[i]          = 0;
            cur_modules_time_period_offset_[i]  = 0;
        }
        cur_phy_pkt_start_offset_               = 0;
        cur_ped_pkt_start_offset_               = 0;
    } else {
        cur_phy_trigg_num_offset_              += cur_scifile_r->phy_trigger_last_trigg_num   - cur_scifile_r->phy_trigger_first_trigg_num   + 1;
        cur_ped_trigg_num_offset_              += cur_scifile_r->ped_trigger_last_trigg_num   - cur_scifile_r->ped_trigger_first_trigg_num   + 1;
        cur_trigg_num_g_offset_                += cur_scifile_r->all_trigger_last_trigg_num_g - cur_scifile_r->all_trigger_first_trigg_num_g + 1;
        cur_trigger_time_period_offset_        += cur_scifile_r->all_trigger_last_time_period - cur_scifile_r->all_trigger_first_time_period;
        cur_time_stamp_diff_ = static_cast<int64_t>(scifile_r->all_trigger_first_time_stamp) - static_cast<int64_t>(cur_scifile_r->all_trigger_last_time_stamp);
        if (cur_time_stamp_diff_  < TRIGG_MIN_TIMESTAMP_DIFF) {
            cur_trigger_time_period_offset_    += 1;
        }
        for (int i = 0; i < 25; i++) {
            if (cur_scifile_r->phy_modules_first_found[i]) {
                cur_phy_event_num_offset_[i]       += cur_scifile_r->phy_modules_last_event_num[i] - cur_scifile_r->phy_modules_first_event_num[i] + 1;
            }
            if (cur_scifile_r->ped_modules_first_found[i]) {
                cur_ped_event_num_offset_[i]       += cur_scifile_r->ped_modules_last_event_num[i] - cur_scifile_r->ped_modules_first_event_num[i] + 1;
            }
            if (cur_scifile_r->phy_modules_first_found[i] || cur_scifile_r->ped_modules_first_found[i]) {
                cur_event_num_g_offset_[i]         += cur_scifile_r->all_modules_last_event_num_g[i] - cur_scifile_r->all_modules_first_event_num_g[i] + 1;
                cur_modules_time_period_offset_[i] += cur_scifile_r->all_modules_last_time_period[i] - cur_scifile_r->all_modules_first_time_period[i];
                cur_time_stamp_diff_ = static_cast<int64_t>(scifile_r->all_modules_first_time_stamp[i]) - static_cast<int64_t>(cur_scifile_r->all_modules_last_time_stamp[i]);
                if (cur_time_stamp_diff_ < EVENT_MIN_TIMESTAMP_DIFF) {
                    cur_modules_time_period_offset_[i] += 1;
                }
            }
        }
        cur_phy_pkt_start_offset_              += cur_scifile_r->phy_trigger_last_pkt_start - cur_scifile_r->phy_trigger_first_pkt_start;
        if (cur_phy_pkt_start_offset_ != t_modules_tree_->GetEntries()) {
            cerr << "pkt_start of physical events has error." << endl;
        }
        cur_ped_pkt_start_offset_              += cur_scifile_r->ped_trigger_last_pkt_start - cur_scifile_r->ped_trigger_first_pkt_start;
        if (cur_ped_pkt_start_offset_ != t_ped_modules_tree_->GetEntries()) {
            cerr << "pkt_start of pedestal events has error." << endl;
        }
    }
    cur_scifile_r = scifile_r;
}

void SciFileW::write_phy_trigger() {
    if (cur_scifile_r == NULL)
        return;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Copying physical trigger data from " << cur_scifile_r->get_filename() << " ..." << endl;
    cout << "[ " << flush;
    cur_scifile_r->phy_trigger_set_start();
    while (cur_scifile_r->phy_trigger_next()) {
        cur_percent = static_cast<int>(100 * cur_scifile_r->phy_trigger_get_cur_entry() / cur_scifile_r->phy_trigger_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_trigger = cur_scifile_r->t_trigger;
        // correct numbers
        if (t_trigger.is_bad <= 0) {
            if (t_trigger.pkt_start >= 0) {
                t_trigger.pkt_start = t_trigger.pkt_start   - cur_scifile_r->phy_trigger_first_pkt_start   + cur_phy_pkt_start_offset_;
            }
            t_trigger.trigg_num     = t_trigger.trigg_num   - cur_scifile_r->phy_trigger_first_trigg_num   + cur_phy_trigg_num_offset_;
            t_trigger.trigg_num_g   = t_trigger.trigg_num_g - cur_scifile_r->all_trigger_first_trigg_num_g + cur_trigg_num_g_offset_;
            t_trigger.time_period   = t_trigger.time_period - cur_scifile_r->all_trigger_first_time_period + cur_trigger_time_period_offset_;
            t_trigger.time_second   = (static_cast<Double_t>(t_trigger.time_stamp) + static_cast<Double_t>(t_trigger.time_period) * 4294967296) * 8.0E-8;
        }
        // fill data
        t_trigger_tree_->Fill();
    }
    cout << " DONE ] " << endl;
}

void SciFileW::write_ped_trigger() {
    if (cur_scifile_r == NULL)
        return;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Copying pedestal trigger data from " << cur_scifile_r->get_filename() << " ..." << endl;
    cout << "[ " << flush;
    cur_scifile_r->ped_trigger_set_start();
    while (cur_scifile_r->ped_trigger_next()) {
        cur_percent = static_cast<int>(100 * cur_scifile_r->ped_trigger_get_cur_entry() / cur_scifile_r->ped_trigger_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_ped_trigger = cur_scifile_r->t_ped_trigger;
        // correct numbers
        if (t_ped_trigger.is_bad <= 0) {
            if (t_ped_trigger.pkt_start >= 0) {
                t_ped_trigger.pkt_start = t_ped_trigger.pkt_start   - cur_scifile_r->ped_trigger_first_pkt_start   + cur_ped_pkt_start_offset_;
            }
            t_ped_trigger.trigg_num     = t_ped_trigger.trigg_num   - cur_scifile_r->ped_trigger_first_trigg_num   + cur_ped_trigg_num_offset_;
            t_ped_trigger.trigg_num_g   = t_ped_trigger.trigg_num_g - cur_scifile_r->all_trigger_first_trigg_num_g + cur_trigg_num_g_offset_;
            t_ped_trigger.time_period   = t_ped_trigger.time_period - cur_scifile_r->all_trigger_first_time_period + cur_trigger_time_period_offset_;
            t_ped_trigger.time_second   = (static_cast<Double_t>(t_ped_trigger.time_stamp) + static_cast<Double_t>(t_ped_trigger.time_period) * 4294967296) * 8.0E-8;
        }
        // fill data
        t_ped_trigger_tree_->Fill();
    }
    cout << " DONE ] " << endl;
}

void SciFileW::write_phy_modules() {
    if (cur_scifile_r == NULL)
        return;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Copying physical modules data from " << cur_scifile_r->get_filename() << " ..." << endl;
    cout << "[ " << flush;
    cur_scifile_r->phy_modules_set_start();
    while (cur_scifile_r->phy_modules_next()) {
        cur_percent = static_cast<int>(100 * cur_scifile_r->phy_modules_get_cur_entry() / cur_scifile_r->phy_modules_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_modules = cur_scifile_r->t_modules;
        // correct numbers
        int idx = t_modules.ct_num - 1;
        if (t_modules.is_bad <= 0) {
            if (t_modules.trigg_num >= 0) {
                t_modules.trigg_num = t_modules.trigg_num   - cur_scifile_r->phy_trigger_first_trigg_num        + cur_phy_trigg_num_offset_;
            }
            t_modules.event_num     = t_modules.event_num   - cur_scifile_r->phy_modules_first_event_num[idx]   + cur_phy_event_num_offset_[idx];
            t_modules.event_num_g   = t_modules.event_num_g - cur_scifile_r->all_modules_first_event_num_g[idx] + cur_event_num_g_offset_[idx];
            t_modules.time_period   = t_modules.time_period - cur_scifile_r->all_modules_first_time_period[idx] + cur_modules_time_period_offset_[idx];
            t_modules.time_second   = (static_cast<Double_t>(t_modules.time_stamp) + static_cast<Double_t>(t_modules.time_period) * 16777216) * 8.0E-8 * 512;
        }
        // fill data
        t_modules_tree_->Fill();
    }
    cout << " DONE ]" << endl;
}

void SciFileW::write_ped_modules() {
    if (cur_scifile_r == NULL)
        return;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Copying pedestal modules data from " << cur_scifile_r->get_filename() << " ..." << endl;
    cout << "[ " << flush;
    cur_scifile_r->ped_modules_set_start();
    while (cur_scifile_r->ped_modules_next()) {
        cur_percent = static_cast<int>(100 * cur_scifile_r->ped_modules_get_cur_entry() / cur_scifile_r->ped_modules_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_ped_modules = cur_scifile_r->t_ped_modules;
        // correct numbers
        int idx = t_ped_modules.ct_num - 1;
        if (t_ped_modules.is_bad <= 0) {
            if (t_ped_modules.trigg_num >= 0) {
                t_ped_modules.trigg_num = t_ped_modules.trigg_num   - cur_scifile_r->ped_trigger_first_trigg_num        + cur_ped_trigg_num_offset_;
            }
            t_ped_modules.event_num     = t_ped_modules.event_num   - cur_scifile_r->ped_modules_first_event_num[idx]   + cur_ped_event_num_offset_[idx];
            t_ped_modules.event_num_g   = t_ped_modules.event_num_g - cur_scifile_r->all_modules_first_event_num_g[idx] + cur_event_num_g_offset_[idx];
            t_ped_modules.time_period   = t_ped_modules.time_period - cur_scifile_r->all_modules_first_time_period[idx] + cur_modules_time_period_offset_[idx];
            t_ped_modules.time_second   = (static_cast<Double_t>(t_ped_modules.time_stamp) + static_cast<Double_t>(t_ped_modules.time_period) * 16777216) * 8.0E-8 * 512;
        }
        // fill data
        t_ped_modules_tree_->Fill();
    }
    cout << " DONE ]" << endl;
}

void SciFileW::write_meta(const char* key, const char* value) {
    if (t_file_out_ == NULL)
        return;
    TNamed* cur_meta = new TNamed(key, value);
    cur_meta->Write();
    delete cur_meta;
    cur_meta = NULL;
}
