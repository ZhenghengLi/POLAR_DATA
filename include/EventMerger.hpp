#ifndef EVENTMERGER_H
#define EVENTMERGER_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <stdint.h>
#include <vector>
#include "SciTrigger.hpp"
#include "SciEvent.hpp"
#include "trigger_queue.hpp"

using namespace std;

class EventMerger {
private:
	SciTrigger result_ped_trigger_;
	vector<SciEvent> result_ped_events_vec_;
	SciTrigger result_noped_trigger_;
	vector<SciEvent> result_noped_events_vec_;

public:
	const SciTrigger& get_result_ped_trigger();
	const vector<SciEvent>& get_result_ped_events_vec();
	const SciTrigger& get_result_noped_trigger();
	const vector<SciEvent>& get_result_noped_events_vec();
	void add_ped_trigger(SciTrigger& trigger);
	void add_ped_event(SciEvent& event);
	void add_noped_trigger(SciTrigger& trigger);
	void add_noped_event(SciEvent& event);
};

#endif
