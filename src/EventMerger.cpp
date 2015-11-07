#include "EventMerger.hpp"

using namespace std;

const SciTrigger& EventMerger::get_result_ped_trigger() {
	return result_ped_trigger_;
}

const vector<SciEvent>& EventMerger::get_result_ped_events_vec() {
	return result_ped_events_vec_;
}

const SciTrigger& EventMerger::get_result_noped_trigger() {
	return result_noped_trigger_;
}

const vector<SciEvent>& EventMerger::get_result_noped_events_vec() {
	return result_noped_events_vec_;
}

void EventMerger::add_ped_trigger(SciTrigger& trigger) {

}

void EventMerger::add_ped_event(SciEvent& event) {

}

void EventMerger::add_noped_trigger(SciTrigger& trigger) {

}

void EventMerger::add_noped_event(SciEvent& event) {

}
