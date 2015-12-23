#include "EventIterator.hpp"

EventIterator::EventIterator() {
    t_file_in_ = NULL;
    t_trigg_tree_ = NULL;
    t_ped_trigg_tree_ = NULL;
    t_event_tree_ = NULL;
    t_ped_event_tree_ = NULL;
}

EventIterator::~EventIterator() {
    if (t_file_in_ != NULL)
        close();
}

bool EventIterator::open(const char* filename) {
    if (t_file_in_ != NULL)
        return false;
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;

    t_trigg_tree_ = static_cast<TTree*>(t_file_in_->Get("t_trigg"));
    t_trigg_tree_->SetBranchAddress("trigg_index", &trigg.trigg_index);
    t_trigg_tree_->SetBranchAddress("mode", &trigg.mode);
    t_trigg_tree_->SetBranchAddress("timestamp", &trigg.timestamp);
    t_trigg_tree_->SetBranchAddress("time_align", &trigg.time_align);
    t_trigg_tree_->SetBranchAddress("packet_num", &trigg.packet_num);
    t_trigg_tree_->SetBranchAddress("trig_accepted", trigg.trig_accepted);
    t_trigg_tree_->SetBranchAddress("trig_rejected", trigg.trig_rejected);
    t_trigg_tree_->SetBranchAddress("deadtime", &trigg.deadtime);
    t_trigg_tree_->SetBranchAddress("start_entry", &trigg.start_entry);
    t_trigg_tree_->SetBranchAddress("pkt_count", &trigg.pkt_count);
    t_trigg_tree_->SetBranchAddress("lost_count", &trigg.lost_count);
    t_trigg_tree_->SetBranchAddress("frm_ship_time", &trigg.frm_ship_time);
    t_trigg_tree_->SetBranchAddress("frm_gps_time", &trigg.frm_gps_time);
    t_trigg_tree_->SetBranchAddress("status", &trigg.status);
    t_trigg_tree_->SetBranchAddress("trig_sig_con", trigg.trig_sig_con);
    
    t_ped_trigg_tree_ = static_cast<TTree*>(t_file_in_->Get("t_ped_trigg"));
    t_ped_trigg_tree_->SetBranchAddress("trigg_index", &ped_trigg.trigg_index);
    t_ped_trigg_tree_->SetBranchAddress("mode", &ped_trigg.mode);
    t_ped_trigg_tree_->SetBranchAddress("timestamp", &ped_trigg.timestamp);
    t_ped_trigg_tree_->SetBranchAddress("time_align", &ped_trigg.time_align);
    t_ped_trigg_tree_->SetBranchAddress("packet_num", &ped_trigg.packet_num);
    t_ped_trigg_tree_->SetBranchAddress("trig_accepted", ped_trigg.trig_accepted);
    t_ped_trigg_tree_->SetBranchAddress("trig_rejected", ped_trigg.trig_rejected);
    t_ped_trigg_tree_->SetBranchAddress("deadtime", &ped_trigg.deadtime);
    t_ped_trigg_tree_->SetBranchAddress("start_entry", &ped_trigg.start_entry);
    t_ped_trigg_tree_->SetBranchAddress("pkt_count", &ped_trigg.pkt_count);
    t_ped_trigg_tree_->SetBranchAddress("lost_count", &ped_trigg.lost_count);
    t_ped_trigg_tree_->SetBranchAddress("frm_ship_time", &ped_trigg.frm_ship_time);
    t_ped_trigg_tree_->SetBranchAddress("frm_gps_time", &ped_trigg.frm_gps_time);
    t_ped_trigg_tree_->SetBranchAddress("status", &ped_trigg.status);
    t_ped_trigg_tree_->SetBranchAddress("trig_sig_con", ped_trigg.trig_sig_con);
    
    t_event_tree_ = static_cast<TTree*>(t_file_in_->Get("t_event"));
    t_event_tree_->SetBranchAddress("trigg_index", &event.trigg_index);
    t_event_tree_->SetBranchAddress("mode", &event.mode);
    t_event_tree_->SetBranchAddress("ct_num", &event.ct_num);
    t_event_tree_->SetBranchAddress("timestamp", &event.timestamp);
    t_event_tree_->SetBranchAddress("time_align", &event.time_align);
    t_event_tree_->SetBranchAddress("trigger_bit", event.trigger_bit);
    t_event_tree_->SetBranchAddress("energy_ch", event.energy_ch);
    t_event_tree_->SetBranchAddress("rate", &event.rate);
    t_event_tree_->SetBranchAddress("deadtime", &event.deadtime);
    t_event_tree_->SetBranchAddress("common_noise", &event.common_noise);
    t_event_tree_->SetBranchAddress("status", &event.status);
    
    t_ped_event_tree_ = static_cast<TTree*>(t_file_in_->Get("t_ped_event"));
    t_ped_event_tree_->SetBranchAddress("trigg_index", &ped_event.trigg_index);
    t_ped_event_tree_->SetBranchAddress("mode", &ped_event.mode);
    t_ped_event_tree_->SetBranchAddress("ct_num", &ped_event.ct_num);
    t_ped_event_tree_->SetBranchAddress("timestamp", &ped_event.timestamp);
    t_ped_event_tree_->SetBranchAddress("time_align", &ped_event.time_align);
    t_ped_event_tree_->SetBranchAddress("trigger_bit", ped_event.trigger_bit);
    t_ped_event_tree_->SetBranchAddress("energy_ch", ped_event.energy_ch);
    t_ped_event_tree_->SetBranchAddress("rate", &ped_event.rate);
    t_ped_event_tree_->SetBranchAddress("deadtime", &ped_event.deadtime);
    t_ped_event_tree_->SetBranchAddress("common_noise", &ped_event.common_noise);
    t_ped_event_tree_->SetBranchAddress("status", &ped_event.status);

    trigg_tot_entries_ = t_trigg_tree_->GetEntries();
    trigg_cur_index_ = 0;
    event_cur_index_ = 0;
    event_cur_start_index_ = 0;
    event_cur_end_index_ = 0;
    trigg_is_start_ = true;
    event_is_start_ = true;
    trigg_reach_end_ = false;
    event_reach_end_ = false;
    
    ped_trigg_tot_entries_ = t_ped_trigg_tree_->GetEntries();
    ped_trigg_cur_index_ = 0;
    ped_event_cur_index_ = 0;
    ped_event_cur_start_index_ = 0;
    ped_event_cur_end_index_ = 0;
    ped_trigg_is_start_ = true;
    ped_event_is_start_ = true;
    ped_trigg_reach_end_ = false;
    ped_event_reach_end_ = false;

    return true;
}

void EventIterator::close() {
    if (t_file_in_ != NULL) {
        t_file_in_->Close();
        delete t_file_in_;
        t_file_in_ = NULL;
        t_trigg_tree_ = NULL;
        t_ped_trigg_tree_ = NULL;
        t_event_tree_ = NULL;
        t_ped_event_tree_ = NULL;
    }
}


bool EventIterator::trigg_next() {
    if (t_file_in_ == NULL)
        return false;
    if (trigg_tot_entries_ == 0)
        return false;
    if (trigg_reach_end_)
        return false;
    if (trigg_is_start_) {
        trigg_is_start_ = false;
        t_trigg_tree_->GetEntry(trigg_cur_index_);
        event_cur_start_index_ = trigg.start_entry;
        event_cur_end_index_ = trigg.start_entry + trigg.pkt_count;
        event_cur_index_ = event_cur_start_index_;
        event_is_start_ = true;
        event_reach_end_ = false;       
        return true;
    } else {
        trigg_cur_index_++;
        if (trigg_cur_index_ < trigg_tot_entries_) {
            t_trigg_tree_->GetEntry(trigg_cur_index_);
            event_cur_start_index_ = trigg.start_entry;
            event_cur_end_index_ = trigg.start_entry + trigg.pkt_count;
            event_cur_index_ = event_cur_start_index_;
            event_is_start_ =true;
            event_reach_end_ = false;
            return true;
        } else {
            trigg_reach_end_ = true;
            return false;
        }
    }
}

void EventIterator::trigg_restart() {
    trigg_cur_index_ = 0;
    event_cur_index_ = 0;
    event_cur_start_index_ = 0;
    event_cur_end_index_ = 0;
    trigg_is_start_ = true;
    event_is_start_ = true;
    trigg_reach_end_ = false;
    event_reach_end_ = false;
}

bool EventIterator::trigg_select(Long64_t entry_index) {
    if (t_file_in_ == NULL)
        return false;
    if (entry_index < 0 || entry_index >= trigg_tot_entries_)
        return false;
    t_trigg_tree_->GetEntry(entry_index);
    event_cur_start_index_ = trigg.start_entry;
    event_cur_end_index_ = trigg.start_entry + trigg.pkt_count;
    event_cur_index_ = event_cur_start_index_;
    event_is_start_ = true;
    event_reach_end_ = false;
    return true;
}

bool EventIterator::ped_trigg_next() {
    if (t_file_in_ == NULL)
        return false;
    if (ped_trigg_tot_entries_ == 0)
        return false;
    if (ped_trigg_reach_end_)
        return false;
    if (ped_trigg_is_start_) {
        ped_trigg_is_start_ = false;
        t_ped_trigg_tree_->GetEntry(ped_trigg_cur_index_);
        ped_event_cur_start_index_ = ped_trigg.start_entry;
        ped_event_cur_end_index_ = ped_trigg.start_entry + ped_trigg.pkt_count;
        ped_event_cur_index_ = ped_event_cur_start_index_;
        ped_event_is_start_ = true;
        ped_event_reach_end_ = false;
        return true;
    } else {
        ped_trigg_cur_index_++;
        if (ped_trigg_cur_index_ < ped_trigg_tot_entries_) {
            t_ped_trigg_tree_->GetEntry(ped_trigg_cur_index_);
            ped_event_cur_start_index_ = ped_trigg.start_entry;
            ped_event_cur_end_index_ = ped_trigg.start_entry + ped_trigg.pkt_count;
            ped_event_cur_index_ = ped_event_cur_start_index_;
            ped_event_is_start_ = true;
            ped_event_reach_end_ = false;
            return true;
        } else {
            ped_trigg_reach_end_ = true;
            return false;
        }
    }
}

void EventIterator::ped_trigg_restart() {
    ped_trigg_cur_index_ = 0;
    ped_event_cur_index_ = 0;
    ped_event_cur_start_index_ = 0;
    ped_event_cur_end_index_ = 0;
    ped_trigg_is_start_ = true;
    ped_event_is_start_ = true;
    ped_trigg_reach_end_ = false;
    ped_event_reach_end_ = false;
}

bool EventIterator::ped_trigg_select(Long64_t entry_index) {
    if (t_file_in_ == NULL)
        return false;
    if (entry_index < 0 || entry_index >= ped_trigg_tot_entries_)
        return false;
    t_ped_trigg_tree_->GetEntry(entry_index);
    ped_event_cur_start_index_ = ped_trigg.start_entry;
    ped_event_cur_end_index_ = ped_trigg.start_entry + ped_trigg.pkt_count;
    ped_event_cur_index_ = ped_event_cur_start_index_;
    ped_event_is_start_ = true;
    ped_event_reach_end_ = false;
    return true;
}

bool EventIterator::event_next() {
    if (trigg_is_start_)
        return false;
    if (trigg_reach_end_)
        return false;
    if (event_cur_start_index_ == event_cur_end_index_)
        return false;
    if (event_reach_end_)
        return false;
    if (event_is_start_) {
        event_is_start_ = false;
        t_event_tree_->GetEntry(event_cur_index_);
        return true;
    } else {
        event_cur_index_++;
        if (event_cur_index_ < event_cur_end_index_) {
            t_event_tree_->GetEntry(event_cur_index_);
            return true;
        } else {
            event_reach_end_ = true;
            return false;
        }
    }
}

void EventIterator::event_restart() {
    event_reach_end_ = false;
    event_is_start_ = true;
    event_cur_index_ = event_cur_start_index_;
}

bool EventIterator::ped_event_next() {
    if (ped_trigg_is_start_)
        return false;
    if (ped_trigg_reach_end_)
        return false;
    if (ped_event_cur_start_index_ == ped_event_cur_end_index_)
        return false;
    if (ped_event_reach_end_)
        return false;
    if (ped_event_is_start_) {
        ped_event_is_start_ = false;
        t_ped_event_tree_->GetEntry(ped_event_cur_index_);
        return true;
    } else {
        ped_event_cur_index_++;
        if (ped_event_cur_index_ < ped_event_cur_end_index_) {
            t_ped_event_tree_->GetEntry(ped_event_cur_index_);
            return true;
        } else {
            ped_event_reach_end_ = true;
            return false;
        }
    }
}

void EventIterator::ped_event_restart() {
    ped_event_reach_end_ = false;
    ped_event_is_start_ = true;
    ped_event_cur_index_ = ped_event_cur_start_index_;
}



