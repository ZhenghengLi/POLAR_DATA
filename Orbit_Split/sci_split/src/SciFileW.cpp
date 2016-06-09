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
