#ifndef PHYEVENTFILE
#define PHYEVENTFILE

#include <iostream>
#include <cstdlib>
#include "RootInc.hpp"

using namespace std;

class PhyEventFile {
private:
	TFile * t_file_;
	TTree * t_trigg_tree_;
	TTree * t_event_tree_;
	
	char mode_;    // 'r', 'w'
	
	Long64_t trigg_tot_entries_;
    Long64_t trigg_cur_index_;
    Long64_t event_cur_index_;
    Long64_t event_cur_start_index_;
    Long64_t event_cur_end_index_;
    bool trigg_is_start_;
    bool event_is_start_;   
    bool trigg_reach_end_;
    bool event_reach_end_;

public:
	struct Event_T {
		Long64_t trigg_index;
		Int_t ct_num;
		Bool_t trigger_bit[64];
		Float_t energy_ch[64];
	};
	struct Trigg_T {
		Long64_t trigg_index;
		Int_t mode;
		Bool_t trig_accepted[25];
		Long64_t start_entry;
		Int_t pkt_count;
		Int_t lost_count;
		Int_t level_flag;
	};

public:
	Event_T event;
	Trigg_T trigg;

private:
	bool open_write_(const char* filename);
	bool open_read_(const char* filename);

public:
	PhyEventFile();
	~PhyEventFile();

	bool open(const char* filename, char mode_par);
	void close();
	// mode 'w'
	void write_trigg();
	void write_event();
	// mode 'r'
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
};

#endif
