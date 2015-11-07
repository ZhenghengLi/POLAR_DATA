#include "EventMerger.hpp"

using namespace std;

EventMerger::EventMerger() {
	ped_trigger_is_first_ = true;
	noped_trigger_is_first_ = true;
	for (int i = 0; i < 25; i++) {
		ped_event_is_first_[i] = true;
		noped_event_is_first_[i] = true;
	}
	global_start_flag_ = false;
}

EventMerger::~EventMerger() {

}

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

void EventMerger::add_ped_trigger(const SciTrigger& trigger) {

}

void EventMerger::add_ped_event(const SciEvent& event) {

}

void EventMerger::add_noped_trigger(const SciTrigger& trigger) {

}

void EventMerger::add_noped_event(const SciEvent& event) {

}

bool EventMerger::noped_do_merge() {
	return true;
}

void EventMerger::noped_clear_result() {

}

bool EventMerger::ped_check_valid() const {
	return true;
}

void EventMerger::ped_update_time_diff() {

}

void EventMerger::ped_move_result(bool valid) {

}

void EventMerger::ped_clear_result() {

}

bool EventMerger::can_noped_do_merge() const {
	return true;
}
