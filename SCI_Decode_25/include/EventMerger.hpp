#ifndef EVENTMERGER_H
#define EVENTMERGER_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <stdint.h>
#include <utility>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
#include <iterator>
#include "SciTrigger.hpp"
#include "SciEvent.hpp"
#include "trigger_queue.hpp"
#include "Constants.hpp"

using namespace std;

class EventMerger {
private:
	SciTrigger result_ped_trigger_;
	vector<SciEvent> result_ped_events_vec_;
	SciTrigger result_noped_trigger_;
	vector<SciEvent> result_noped_events_vec_;

	SciTrigger curr_ped_trigger_;
	SciTrigger next_ped_trigger_;
	vector<SciEvent> curr_ped_events_vec_;
	vector<int> curr_ped_event_ct_num_vec_;
	
	bool ped_trigger_not_ready_;
	bool trigger_is_first_;
	bool event_is_first_[25];

	int trigger_period_;
	int trigger_curr_time_align_;
	int event_period_[25];
	int event_common_period_;
	int event_curr_time_align_[25];

	int event_time_diff_[25];
	bool event_start_flag_[25];
	bool global_start_flag_;

	trigger_queue<SciTrigger, vector<SciTrigger>, greater<SciTrigger> > noped_trigger_queue_;
	priority_queue<SciEvent, vector<SciEvent>, greater<SciEvent> > noped_event_queue_[25];
private:
	bool can_noped_do_merge() const;
	void sync_event_period_(int idx);
public:
	EventMerger();
	~EventMerger();

	const SciTrigger& get_result_ped_trigger();
	const vector<SciEvent>& get_result_ped_events_vec();
	const SciTrigger& get_result_noped_trigger();
	const vector<SciEvent>& get_result_noped_events_vec();

	void add_ped_trigger(SciTrigger& trigger);
	void add_ped_event(SciEvent& event);
	void add_noped_trigger(SciTrigger& trigger);
	void add_noped_event(SciEvent& event);

	bool noped_do_merge(bool force = false);
	bool noped_trigger_empty(); 
	void noped_clear_result();
	bool ped_check_valid();
	void ped_update_time_diff();
	void ped_move_result(bool valid);
	void ped_clear_result();

	void all_clear();
	bool global_start() {
		return global_start_flag_;
	}
};

#endif
