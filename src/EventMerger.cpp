#include "EventMerger.hpp"

using namespace std;

EventMerger::EventMerger() {
	ped_trigger_not_ready_ = true;
	trigger_is_first_ = true;
	for (int i = 0; i < 25; i++) {
		event_is_first_[i] = true;
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

void EventMerger::add_ped_trigger(SciTrigger& trigger) {
	if (trigger_is_first_) {
		trigger_is_first_ = false;
		trigger_period_ = 0;
		trigger_curr_time_align_ = static_cast<int>(trigger.time_align);		
		trigger.set_period(trigger_period_);
		next_ped_trigger_ = trigger;
	} else {
		if (static_cast<int>(trigger.time_align) - trigger_curr_time_align_ < -1 * PedCircle)
			trigger_period_++;
		trigger_curr_time_align_ = static_cast<int>(trigger.time_align);
		trigger.set_period(trigger_period_);
		next_ped_trigger_ = trigger;
	}
}

void EventMerger::add_ped_event(SciEvent& event) {
	int ct_number = static_cast<int>(event.ct_num);
	if (find(curr_ped_event_ct_num_vec_.begin(), curr_ped_event_ct_num_vec_.end(),
			ct_number) != curr_ped_event_ct_num_vec_.end()) {
		add_noped_event(event);
		return;
	}
	int idx = ct_number - 1;
	if (event_is_first_[idx]) {
		event_is_first_[idx] = false;
		event_period_[idx] = 0;
		event_curr_time_align_[idx] = static_cast<int>(event.time_align);
		event.set_period(event_period_[idx]);
		curr_ped_event_ct_num_vec_.push_back(ct_number);
		curr_ped_events_vec_.push_back(event);
	} else {
		if (static_cast<int>(event.time_align) - event_curr_time_align_[idx] < -1 * PedCircle)
			event_period_[idx]++;
		event_curr_time_align_[idx] = static_cast<int>(event.time_align);
		event.set_period(event_period_[idx]);
		curr_ped_event_ct_num_vec_.push_back(ct_number);
		curr_ped_events_vec_.push_back(event);
	}
}

void EventMerger::add_noped_trigger(SciTrigger& trigger) {
	if (trigger_is_first_) {
		trigger_is_first_ = false;
		trigger_period_ = 0;
		trigger_curr_time_align_ = static_cast<int>(trigger.time_align);
		trigger.set_period(trigger_period_);
		noped_trigger_queue_.push(trigger);
	} else {
		if (static_cast<int>(trigger.time_align) - trigger_curr_time_align_ < -1 * PedCircle)
			trigger_period_++;
		trigger_curr_time_align_ = static_cast<int>(trigger.time_align);
		trigger.set_period(trigger_period_);
		noped_trigger_queue_.push(trigger);
	}
}

void EventMerger::add_noped_event(SciEvent& event) {
	int ct_number = static_cast<int>(event.ct_num);
	int idx = ct_number - 1;
	if (event_is_first_[idx]) {
		event_is_first_[idx] = false;
		event_period_[idx] = 0;
		event_curr_time_align_[idx] = static_cast<int>(event.time_align);
		event.set_period(event_period_[idx]);
		noped_event_queue_[idx].push(event);
	} else {
		if (static_cast<int>(event.time_align) - event_curr_time_align_[idx] < -1 * PedCircle)
			event_period_[idx]++;
		event_curr_time_align_[idx] = static_cast<int>(event.time_align);
		event.set_period(event_period_[idx]);
		noped_event_queue_[idx].push(event);
	}
}

bool EventMerger::noped_do_merge() {
	bool result = true;
	
	return result;
}

void EventMerger::noped_clear_result() {

}

bool EventMerger::ped_check_valid() {
	if (ped_trigger_not_ready_)
		return false;
	if (curr_ped_events_vec_.size() > 25 || curr_ped_events_vec_.size() < 10)
		return false;
	bool is_alone;
	for (size_t i = 0; i < curr_ped_events_vec_.size(); i++) {
		is_alone = true;
		for (size_t j = 0; j < curr_ped_events_vec_.size(); j++) {
			if (i == j)
				continue;
			if (abs(static_cast<int>(curr_ped_events_vec_[i].time_align) -
					static_cast<int>(curr_ped_events_vec_[j].time_align)) < 2) {
				is_alone = false;
				break;
			}
		}
		if (is_alone)
			curr_ped_event_alone_idx_vec_.push_back(i);
	}
	for (size_t i = 1; i < curr_ped_events_vec_.size(); i++) {
		if (find(curr_ped_event_alone_idx_vec_.begin(),
				 curr_ped_event_alone_idx_vec_.end(),
				 i) != curr_ped_event_alone_idx_vec_.end())
			continue;
		if (abs(static_cast<int>(curr_ped_events_vec_[i].time_align) -
				static_cast<int>(curr_ped_events_vec_[0].time_align)) >=2 )
			return false;
	}
	if (curr_ped_events_vec_.size() - curr_ped_event_alone_idx_vec_.size() < 10)
		return false;
	else
		return true;
}

void EventMerger::ped_update_time_diff() {
	map<int, int> time_diff_count;
	map<int, int>::iterator mapItr;
	int time_diff;
	for (size_t i = 0; i < result_ped_events_vec_.size(); i++) {
		time_diff = static_cast<int>(result_ped_trigger_.time_align) -
			static_cast<int>(result_ped_events_vec_[i].time_align);
		if (time_diff_count.find(time_diff) == time_diff_count.end()) {
			time_diff_count.insert(make_pair(time_diff, 1));
		} else {
			time_diff_count[time_diff] = time_diff_count[time_diff] + 1;
		}
	}
	int max_count = 0;
	for (mapItr = time_diff_count.begin(); mapItr != time_diff_count.end(); mapItr++) {
		if (mapItr->second > max_count) {
			max_count = mapItr->second;
			global_time_diff_ = mapItr->first;
		}
	}
	if (!global_start_flag_)
		global_start_flag_ = true;
}

void EventMerger::ped_move_result(bool valid) {
	if (valid) {
		result_ped_trigger_ = curr_ped_trigger_;
		for (size_t i = 0; i < curr_ped_events_vec_.size(); i++) {
			if (find(curr_ped_event_alone_idx_vec_.begin(),
					 curr_ped_event_alone_idx_vec_.end(),
					 i) != curr_ped_event_alone_idx_vec_.end())
				continue;
			result_ped_events_vec_.push_back(curr_ped_events_vec_[i]);
		}
		curr_ped_trigger_ = next_ped_trigger_;
		curr_ped_events_vec_.clear();
		curr_ped_event_ct_num_vec_.clear();
		curr_ped_event_alone_idx_vec_.clear();
	} else {
		if (ped_trigger_not_ready_)
			ped_trigger_not_ready_ = false;
		curr_ped_trigger_ = next_ped_trigger_;
		curr_ped_events_vec_.clear();
		curr_ped_event_ct_num_vec_.clear();
		curr_ped_event_alone_idx_vec_.clear();
	}
}

void EventMerger::ped_clear_result() {
	result_ped_events_vec_.clear();
}

bool EventMerger::can_noped_do_merge() const {
	bool flag_event = true;
	SciTrigger top_trigger = noped_trigger_queue_.top();
	for (int i = 0; i < 25; i++) {
		if (top_trigger.trig_accepted[i] > 0) {
			if (noped_event_queue_[i].size() < 20) {
				flag_event = false;
				break;
			}
		}
	}
	bool flag_trigger = (noped_trigger_queue_.distance() > PedCircle);
	return (flag_event || flag_trigger);
}        
