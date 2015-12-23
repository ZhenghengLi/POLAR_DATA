#include "EventMerger.hpp"

using namespace std;

EventMerger::EventMerger() {
    ped_trigger_not_ready_ = true;
    trigger_is_first_ = true;
    for (int i = 0; i < 25; i++) {
        event_is_first_[i] = true;
    }
    global_start_flag_ = false;
    force_start_count_ = 0;
}

EventMerger::~EventMerger() {

}

void EventMerger::all_clear() {
    ped_trigger_not_ready_ = true;
    trigger_is_first_ = true;
    for (int i = 0; i < 25; i++) {
        event_is_first_[i] = true;
    }
    global_start_flag_ = false;
    force_start_count_ = 0;
    result_ped_events_vec_.clear();
    result_noped_events_vec_.clear();
    curr_ped_events_vec_.clear();
    curr_ped_event_ct_num_vec_.clear();
    curr_ped_event_alone_idx_vec_.clear();
    while (!noped_trigger_queue_.empty()) {
        noped_trigger_queue_.pop();
    }
    for (int i = 0; i < 25; i++) {
        while (!noped_event_queue_[i].empty()) {
            noped_event_queue_[i].pop();
        }
    }
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
        sync_event_period_(idx);
        event.set_period(event_period_[idx]);
        curr_ped_event_ct_num_vec_.push_back(ct_number);
        curr_ped_events_vec_.push_back(event);
    } else {
        if (static_cast<int>(event.time_align) - event_curr_time_align_[idx] < -1 * PedCircle) 
            event_period_[idx]++;
        event_curr_time_align_[idx] = static_cast<int>(event.time_align);
        sync_event_period_(idx);
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
        sync_event_period_(idx);
        event.set_period(event_period_[idx]);
        noped_event_queue_[idx].push(event);
    } else {
        if (static_cast<int>(event.time_align) - event_curr_time_align_[idx] < -1 * PedCircle)
            event_period_[idx]++;
        event_curr_time_align_[idx] = static_cast<int>(event.time_align);
        sync_event_period_(idx);
        event.set_period(event_period_[idx]);
        noped_event_queue_[idx].push(event);
    }
}

bool EventMerger::noped_do_merge(bool force) {
    if (!global_start_flag_)
        return false;
    if (noped_trigger_queue_.empty())
        return false;
    if ((!force) && (!can_noped_do_merge()))
        return false;
    result_noped_trigger_ = noped_trigger_queue_.top();
    noped_trigger_queue_.pop();
    SciEvent result_noped_event;
    int pkt_count = 0;
    int tot_count = 0;
    for (int i = 0; i < 25; i++) {
        if (result_noped_trigger_.trig_accepted[i] < 1)
            continue;
        tot_count++;
        if (noped_event_queue_[i].empty())
            continue;
        result_noped_event = noped_event_queue_[i].top();
        while ((result_noped_trigger_ - result_noped_event) -
               global_time_diff_  > -2) {
            if ((result_noped_trigger_ - result_noped_event) -
                global_time_diff_ > 1) {
                noped_event_queue_[i].pop();
                if (noped_event_queue_[i].empty())
                    break;
                result_noped_event = noped_event_queue_[i].top();
            } else {
                noped_event_queue_[i].pop();                
                result_noped_events_vec_.push_back(result_noped_event);
                pkt_count++;
                break;
            }
        }
    }
    result_noped_trigger_.set_pkt_count(pkt_count);
    result_noped_trigger_.set_lost_count(tot_count - pkt_count);
    if (result_noped_events_vec_.empty())
        return false;
    else
        return true;
}

bool EventMerger::noped_trigger_empty() {
    return noped_trigger_queue_.empty();
}

void EventMerger::noped_clear_result() {
    result_noped_events_vec_.clear();
}

bool EventMerger::ped_check_valid() {
    if (ped_trigger_not_ready_)
        return false;
    if (curr_ped_events_vec_.size() > 25 || curr_ped_events_vec_.size() < 10)
        return false;
    if (!global_start_flag_) {
        force_start_count_++;
        if (force_start_count_ > 10) {
            cerr << "WARNING: the offsets of 25 modules are too different" << endl;
            global_start_flag_ = true;
        }
    }
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
    if (curr_ped_events_vec_.size() - curr_ped_event_alone_idx_vec_.size() < 10)
        return false;
    size_t first_i = 0;
    for (size_t i = 0; i < curr_ped_events_vec_.size(); i++) {
        if (find(curr_ped_event_alone_idx_vec_.begin(),
                 curr_ped_event_alone_idx_vec_.end(),
                 i) == curr_ped_event_alone_idx_vec_.end()) {
            first_i = i;
            break;
        }
    }
    for (size_t i = first_i + 1; i < curr_ped_events_vec_.size(); i++) {
        if (find(curr_ped_event_alone_idx_vec_.begin(),
                 curr_ped_event_alone_idx_vec_.end(),
                 i) != curr_ped_event_alone_idx_vec_.end())
            continue;
        if (abs(static_cast<int>(curr_ped_events_vec_[i].time_align) -
                static_cast<int>(curr_ped_events_vec_[first_i].time_align)) >=2 )
            return false;
    }
    return true;
}

void EventMerger::ped_update_time_diff() {
    int time_diff[25];
    for (size_t i = 0; i < result_ped_events_vec_.size(); i++) {
        time_diff[i] = result_ped_trigger_ - result_ped_events_vec_[i];
    }
    global_time_diff_ = find_common_(time_diff, result_ped_events_vec_.size());
    if (!global_start_flag_)
        global_start_flag_ = true;
}

void EventMerger::ped_move_result(bool valid) {
    if (valid) {
        int pkt_count = 0;
        int tot_count = 0;
        for (int i = 0; i < 25; i++) {
            if (curr_ped_trigger_.trig_accepted[i] > 0)
                tot_count++;
        }
        result_ped_trigger_ = curr_ped_trigger_;
        for (size_t i = 0; i < curr_ped_events_vec_.size(); i++) {
            if (find(curr_ped_event_alone_idx_vec_.begin(),
                     curr_ped_event_alone_idx_vec_.end(),
                     i) != curr_ped_event_alone_idx_vec_.end())
                continue;
            result_ped_events_vec_.push_back(curr_ped_events_vec_[i]);
            pkt_count++;
        }
        result_ped_trigger_.set_pkt_count(pkt_count);
        result_ped_trigger_.set_lost_count(tot_count - pkt_count);
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
            if (noped_event_queue_[i].size() < 25) {
                flag_event = false;
                break;
            }
        }
    }
    bool flag_trigger = (noped_trigger_queue_.distance() > PedCircle);
    return (flag_event || flag_trigger);
}        

int EventMerger::find_common_(int* arr, size_t len) {
    map<int, int> val_count;
    map<int, int>::iterator mapItr;
    for (size_t i = 0; i < len; i++) {
        if (val_count.find(arr[i]) == val_count.end()) {
            val_count.insert(make_pair(arr[i], 1));
        } else {
            val_count[arr[i]] = val_count[arr[i]] + 1;
        }
    }
    int max_count = 0;
    int result_val = 0;
    for (mapItr = val_count.begin(); mapItr != val_count.end(); mapItr++) {
        if (mapItr->second > max_count) {
            max_count = mapItr->second;
            result_val = mapItr->first;
        }
    }
    return result_val;
}

void EventMerger::sync_event_period_(int idx) {
    if (!global_start_flag_)
        return;
    if (abs(event_period_[idx] - trigger_period_) > 1) {
        if (global_time_diff_ >= 0) {
            if (trigger_curr_time_align_ - event_curr_time_align_[idx] < -1 * PedCircle)
                event_period_[idx] = trigger_period_ - 1;
            else
                event_period_[idx] = trigger_period_;
        } else {
            if (event_curr_time_align_[idx] - trigger_curr_time_align_ < -1 * PedCircle)
                event_period_[idx] = trigger_period_ + 1;
            else
                event_period_[idx] = trigger_period_;
        }
        return;
    } else {
        return;
    }
}
