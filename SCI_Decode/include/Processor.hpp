#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <iterator>
#include "SciFrame.hpp"
#include "SciEvent.hpp"
#include "SciTrigger.hpp"
#include "Counter.hpp"
#include "Constants.hpp"
#include "RootInc.hpp"
#include "EventMerger.hpp"

using namespace std;

class Processor {
private:
	// trigger time
	int pre_ped_trigg_time_;
	bool start_flag_;
	// log file
	bool log_flag_;
	ofstream os_logfile_;
	// output settings
	TFile* t_file_out_;
	TTree* t_event_tree_;
	TTree* t_ped_event_tree_;
	TTree* t_trigg_tree_;
	TTree* t_ped_trigg_tree_;
	// two type of packet
	SciEvent sci_event;
	SciTrigger sci_trigger;
    // for Branch of TTree
	// for event
	Long64_t b_trigg_index_;
	Long64_t b_ped_trigg_index_;
	Int_t b_event_mode_;
	Int_t b_event_ct_num_;
	UInt_t b_event_timestamp_;
	UInt_t b_event_time_align_;
	Bool_t b_event_trigger_bit_[64];
	Int_t b_event_energy_ch_[64];
	Int_t b_event_rate_;
	UInt_t b_event_deadtime_;
	Int_t b_event_common_noise_;
	UShort_t b_event_status_;
	// for trigger
	Int_t b_trigg_mode_;
	UInt_t b_trigg_timestamp_;
	UInt_t b_trigg_time_align_;
	Int_t b_trigg_packet_num_;
	Bool_t b_trigg_trig_accepted_[25];
	Bool_t b_trigg_trig_rejected_[25];
	UInt_t b_trigg_deadtime_;
	Long64_t b_trigg_start_entry_;
	Long64_t b_ped_trigg_start_entry_;
	Int_t b_trigg_pkt_count_;
	Int_t b_trigg_lost_count_;
	ULong64_t b_trigg_frm_ship_time_;
	ULong64_t b_trigg_frm_gps_time_;
	UShort_t b_trigg_status_;
	UChar_t b_trigg_trig_sig_con_[25];
	// EventMerger
	EventMerger evtMgr_;
private:
	void br_trigg_update_(const SciTrigger& trigger);
	void br_event_update_(const SciEvent& event);
	void trigg_write_tree_(const SciTrigger& trigger);
	void ped_trigg_write_tree_(const SciTrigger& trigger);
	void event_write_tree_(const SciEvent& event);
	void ped_event_write_tree_(const SciEvent& event);
	void result_ped_write_tree_(const SciTrigger& trigger,
								const vector<SciEvent>& events_vec);
	void result_write_tree_(const SciTrigger& trigger,
							const vector<SciEvent>& events_vec);
public:
	Counter cnt;
public:
	Processor();
	~Processor();
	void initialize();
	bool process_frame(SciFrame& frame);
	bool process_start(SciFrame& frame);
	bool process_restart(SciFrame& frame);
	bool interruption_occurred(SciFrame& frame);
	void process_packet(SciFrame& frame);
	bool rootfile_open(const char* filename);
	void rootfile_close();
	// for test
	void set_log(bool flag);
	bool can_log();
	bool logfile_open(const char* filename);
	void logfile_close();
	void do_the_last_work();
};

#endif