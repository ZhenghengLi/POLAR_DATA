#ifndef EVENTITERATOR_H
#define EVENTITERATOR_H

#include <iostream>
#include <cstdlib>
#include "RootInc.hpp"

using namespace std;

class EventIterator {
private:
	TFile * t_file_in_;
	TTree * t_trigg_tree_;
	TTree * t_ped_trigg_tree_;
	TTree * t_event_tree_;
	TTree * t_ped_event_tree_;
	
	Long64_t trigg_tot_entries_;
	Long64_t trigg_cur_index_;
	Long64_t event_cur_index_;
	Long64_t event_cur_start_index_;
	Long64_t event_cur_end_index_;
	bool trigg_is_start_;
	bool event_is_start_;	
	bool trigg_reach_end_;
	bool event_reach_end_;
	
	Long64_t ped_trigg_tot_entries_;
	Long64_t ped_trigg_cur_index_;
	Long64_t ped_event_cur_index_;
	Long64_t ped_event_cur_start_index_;
	Long64_t ped_event_cur_end_index_;
	bool ped_trigg_is_start_;
	bool ped_event_is_start_;	
	bool ped_trigg_reach_end_;
	bool ped_event_reach_end_;
	
public:
	struct Trigg_T {
		Long64_t trigg_index;
		Int_t mode;
		UInt_t timestamp;
		UInt_t time_align;
		Int_t packet_num;
		Bool_t trig_accepted[25];
		Bool_t trig_rejected[25];
		UInt_t deadtime;
		Long64_t start_entry;
		Int_t pkt_count;
		Int_t lost_count;
		ULong64_t frm_ship_time;
		ULong64_t frm_gps_time;
		UShort_t status;
		UChar_t trig_sig_con[25];
	};
	
	struct Event_T {
		Long64_t trigg_index;
		Int_t mode;
		Int_t ct_num;
		UInt_t timestamp;
		UInt_t time_align;
		Bool_t trigger_bit[64];
		Int_t energy_ch[64];
		Int_t rate;
		UInt_t deadtime;
		Int_t common_noise;
		UShort_t status;
	};
	
public:
	Trigg_T trigg;
	Trigg_T ped_trigg;
	Event_T event;
	Event_T ped_event;
	
public:
	EventIterator();
	~EventIterator();
	
	bool open(const char* filename);
	void close();
	
	bool trigg_next();
	bool event_next();
	void trigg_restart();
	void event_restart();
	bool trigg_select(Long64_t entry_index);
	Long64_t total_entries() {
		return trigg_tot_entries_;
	}
	Long64_t current_index() {
		return trigg_cur_index_;
	}

	bool ped_trigg_next();
	bool ped_event_next();
	void ped_trigg_restart();
	void ped_event_restart();
	bool ped_trigg_select(Long64_t event_index);
	Long64_t ped_total_entries() {
		return ped_trigg_tot_entries_;
	}
	Long64_t ped_current_index() {
		return ped_trigg_cur_index_;
	}
};

#endif 
