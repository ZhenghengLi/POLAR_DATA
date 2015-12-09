#include "PhyEventFile.hpp"

using namespace std;

PhyEventFile::PhyEventFile() {
	t_file_ = NULL;
	t_trigg_tree_ = NULL;
	t_event_tree_ = NULL;
}

PhyEventFile::~PhyEventFile() {
	if (t_file_ != NULL)
		close();
}

bool PhyEventFile::open_write_(const char* filename) {
	if (t_file_ != NULL)
		return false;
	t_file_ = new TFile(filename, "RECREATE");
	if (t_file_->IsZombie())
		return false;
	t_trigg_tree_ = new TTree("t_trigg", "trigger packet with index");
	t_trigg_tree_->SetDirectory(t_file_);
	t_event_tree_ = new TTree("t_event", "event packet of 25 modules");
	t_event_tree_->SetDirectory(t_file_);

	t_trigg_tree_->Branch("trigg_index", &trigg.trigg_index, "trigg_index/L");
	t_trigg_tree_->Branch("mode", &trigg.mode, "mode/I");
	t_trigg_tree_->Branch("trig_accepted", trigg.trig_accepted, "trig_accepted[25]/O");
	t_trigg_tree_->Branch("start_entry", &trigg.start_entry, "start_entry/L");
	t_trigg_tree_->Branch("pkt_count", &trigg.pkt_count, "pkt_count/I");
	t_trigg_tree_->Branch("lost_count", &trigg.lost_count, "lost_count/I");

	t_event_tree_->Branch("trigg_index", &event.trigg_index, "trigg_index/L");
	t_event_tree_->Branch("ct_num", &event.ct_num, "ct_num/I");
	t_event_tree_->Branch("trigger_bit", event.trigger_bit, "trigger_bit[64]/O");
	t_event_tree_->Branch("energy_ch", event.energy_ch, "energy_ch[64]/I");

	return true;
}

bool PhyEventFile::open_read_(const char* filename) {
	if (t_file_ != NULL)
		return false;
	t_file_ = new TFile(filename, "READ");
	if (t_file_->IsZombie())
		return false;

	t_trigg_tree_ = static_cast<TTree*>(t_file_->Get("t_trigg"));
	t_trigg_tree_->SetBranchAddress("trigg_index", &trigg.trigg_index);
	t_trigg_tree_->SetBranchAddress("mode", &trigg.mode);
	t_trigg_tree_->SetBranchAddress("trig_accepted", trigg.trig_accepted);
	t_trigg_tree_->SetBranchAddress("start_entry", &trigg.start_entry);
	t_trigg_tree_->SetBranchAddress("pkt_count", &trigg.pkt_count);
	t_trigg_tree_->SetBranchAddress("lost_count", &trigg.lost_count);
	
	t_event_tree_ = static_cast<TTree*>(t_file_->Get("t_event"));
	t_event_tree_->SetBranchAddress("trigg_index", &event.trigg_index);
	t_event_tree_->SetBranchAddress("ct_num", &event.ct_num);
	t_event_tree_->SetBranchAddress("trigger_bit", event.trigger_bit);
	t_event_tree_->SetBranchAddress("energy_ch", event.energy_ch);

	trigg_tot_entries_ = t_trigg_tree_->GetEntries();
	trigg_cur_index_ = 0;
	event_cur_index_ = 0;
	event_cur_start_index_ = 0;
	event_cur_end_index_ = 0;
	trigg_is_start_ = true;
	event_is_start_ = true;
	trigg_reach_end_ = false;
	event_reach_end_ = false;

	return true;
}

bool PhyEventFile::open(const char* filename, char mode_par) {
	if (mode_par == 'r' || mode_par == 'R') {
		mode_ = 'r';
		return open_read_(filename);
	} else if (mode_par == 'w' || mode_par == 'W') {
		mode_ = 'w';
		return open_write_(filename);
	} else {
		return false;
	}
}

void PhyEventFile::close() {
	if (t_file_ == NULL)
		return;
	if (mode_ == 'w') {
		t_trigg_tree_->Write();
		delete t_trigg_tree_;
		t_trigg_tree_ = NULL;
		t_event_tree_->Write();
		delete t_event_tree_;
		t_event_tree_ = NULL;
		t_file_->Close();
		delete t_file_;
		t_file_ = NULL;
	} else if (mode_ == 'r') {
		t_file_->Close();
		delete t_file_;
		t_file_ = NULL;
		t_trigg_tree_ = NULL;
		t_event_tree_ = NULL;
	}
}

void PhyEventFile::write_trigg() {
	if (t_file_ == NULL)
		return;
	if (mode_ != 'w')
		return;
	t_trigg_tree_->Fill();
}

void PhyEventFile::write_event() {
	if (t_file_ == NULL)
		return;
	if (mode_ != 'w')
		return;
	t_event_tree_->Fill();
}

bool PhyEventFile::trigg_next() {
	if (t_file_ == NULL)
		return false;
	if (mode_ != 'r')
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

void PhyEventFile::trigg_restart() {
	if (mode_ != 'r')
		return;
    trigg_cur_index_ = 0;
    event_cur_index_ = 0;
    event_cur_start_index_ = 0;
    event_cur_end_index_ = 0;
    trigg_is_start_ = true;
    event_is_start_ = true;
    trigg_reach_end_ = false;
    event_reach_end_ = false;
}

bool PhyEventFile::trigg_select(Long64_t entry_index) {
    if (t_file_ == NULL)
        return false;
	if (mode_ != 'r')
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

bool PhyEventFile::event_next() {
	if (mode_ != 'r')
		return false;
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

void PhyEventFile::event_restart() {
	if (mode_ != 'r')
		return;
    event_reach_end_ = false;
    event_is_start_ = true;
    event_cur_index_ = event_cur_start_index_;
}
