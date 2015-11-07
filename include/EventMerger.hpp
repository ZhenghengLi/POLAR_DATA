#ifndef EVENTMERGER_H
#define EVENTMERGER_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <stdint.h>
#include <vector>
#include <queue>
#include <algorithm>
#include "SciTrigger.hpp"
#include "SciEvent.hpp"

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
	vector<int> curr_ped_event_IDs_vec_;
	
	bool ped_trigger_is_first_;
	bool ped_event_is_first_[25];
	bool noped_trigger_is_first_;
	bool noped_event_is_first_[25];

	int trigger_period_;
	int trigger_pre_time_align_;
	int event_period_[25];
	int event_pre_time_align_[25];

	int global_time_diff_;
	bool global_start_flag_;

	priority_queue<SciTrigger, vector<SciTrigger>, greater<SciTrigger> > noped_trigger_queue_;
	priority_queue<SciEvent, vector<SciEvent>, greater<SciEvent> > noped_event_queue_[25];
private:
	bool can_noped_do_merge() const;
public:
	EventMerger();
	~EventMerger();

	const SciTrigger& get_result_ped_trigger();
	const vector<SciEvent>& get_result_ped_events_vec();
	const SciTrigger& get_result_noped_trigger();
	const vector<SciEvent>& get_result_noped_events_vec();

	void add_ped_trigger(const SciTrigger& trigger);
	void add_ped_event(const SciEvent& event);
	void add_noped_trigger(const SciTrigger& trigger);
	void add_noped_event(const SciEvent& event);

	bool noped_do_merge();
	void noped_clear_result();
	bool ped_check_valid() const ;
	void ped_update_time_diff();
	void ped_move_result(bool valid);
	void ped_clear_result();
};

#endif
