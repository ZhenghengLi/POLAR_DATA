#include "SciTrigger.hpp"
#include "SciEvent.hpp"

using namespace std;

void SciTrigger::set_mode_(const char* packet_buffer, size_t packet_len) {
    mode = decode_byte<uint16_t>(packet_buffer, 2, 3);
}

void SciTrigger::set_timestamp_(const char* packet_buffer, size_t packet_len) {
    timestamp = decode_byte<uint32_t>(packet_buffer, 4, 7);
}

void SciTrigger::set_time_align_() {
    time_align = timestamp >> 9;
}

void SciTrigger::set_packet_num_(const char* packet_buffer, size_t packet_len) {
    packet_num = decode_byte<uint16_t>(packet_buffer, 10, 11);
} 

void SciTrigger::set_trig_accepted_(const char* packet_buffer, size_t packet_len) {
    for (int i = 0; i < 25; i++) {
        trig_accepted[i] = decode_bit<uint16_t>(packet_buffer + 37, 24 - i, 24 - i);
    }
}

void SciTrigger::set_trig_rejected_(const char* packet_buffer, size_t packet_len) {
    for (int i = 0; i < 25; i++) {
        trig_rejected[i] = decode_bit<uint16_t>(packet_buffer + 40, 25 - i, 25 - i);
    }
}

void SciTrigger::set_deadtime_(const char* packet_buffer, size_t packet_len) {
    deadtime = decode_byte<uint16_t>(packet_buffer, 46, 47);
}

void SciTrigger::set_status_(const char* packet_buffer, size_t packet_len) {
    status = decode_byte<uint16_t>(packet_buffer, 8, 9);
}

void SciTrigger::set_trig_sig_con_(const char* packet_buffer, size_t packet_len) {
    for (int i = 0; i < 25; i++) {
        trig_sig_con[i] = decode_byte<uint8_t>(packet_buffer, 12 + i, 12 + i);
    }
}

void SciTrigger::clear_all_info() {
    period_ = 0;
    pkt_count_ = 0;
    lost_count_ = 0;
    mode = 0;
    timestamp = 0;
    time_align = 0;
    status = 0;
    packet_num = 0;
    for (int i = 0; i < 25; i++) {
        trig_sig_con[i] = 0;
        trig_accepted[i] = 0;
        trig_rejected[i] = 0;
    }
    deadtime = 0;
    trigg_num_g = 0;
    is_bad = 0;
    pre_is_bad = 0;
    time_period = 0;
    time_wait = 0;
    dead_ratio = 0;
}

void SciTrigger::update(const char* packet_buffer, size_t packet_len) {
    set_mode_(packet_buffer, packet_len);
    set_timestamp_(packet_buffer, packet_len);
    set_time_align_();
    set_packet_num_(packet_buffer, packet_len);
    set_trig_accepted_(packet_buffer, packet_len);
    set_trig_rejected_(packet_buffer, packet_len);
    set_deadtime_(packet_buffer, packet_len);
    set_status_(packet_buffer, packet_len);
    set_trig_sig_con_(packet_buffer, packet_len);
}

void SciTrigger::set_frm_time(uint64_t ship_time, uint64_t gps_time) {
    frm_ship_time = ship_time;
    frm_gps_time = gps_time;
}

int SciTrigger::get_period() const {
    return period_;
}

void SciTrigger::set_period(int pt) {
    period_ = pt;
}

int SciTrigger::get_pkt_count() const {
    return pkt_count_;
}

void SciTrigger::set_pkt_count(int pkt_cnt) {
    pkt_count_ = pkt_cnt;
}

int SciTrigger::get_lost_count() const {
    return lost_count_;
}

void SciTrigger::set_lost_count(int lost_cnt) {
    lost_count_ = lost_cnt;
}

bool SciTrigger::operator<(const SciTrigger& right) const {
    if (period_ < right.period_) {
        return true;
    } else if (period_ > right.period_) {
        return false;
    } else {
        return (timestamp < right.timestamp);
    }
}

bool SciTrigger::operator<=(const SciTrigger& right) const {
    if (period_ < right.period_) {
        return true;
    } else if (period_ > right.period_) {
        return false;
    } else {
        return (timestamp <= right.timestamp);
    }
}

bool SciTrigger::operator>(const SciTrigger& right) const {
    if (period_ > right.period_) {
        return true;
    } else if (period_ < right.period_) {
        return false;
    } else {
        return (timestamp > right.timestamp);
    }
}

bool SciTrigger::operator>=(const SciTrigger& right) const {
    if (period_ > right.period_) {
        return true;
    } else if (period_ < right.period_) {
        return false;
    } else {
        return (timestamp >= right.timestamp);
    }
}

bool SciTrigger::operator==(const SciTrigger& right) const {
    if (period_ > right.period_) {
        return false;
    } else if (period_ < right.period_) {
        return false;
    } else {
        return (timestamp == right.timestamp);
    }
}

int SciTrigger::operator-(const SciTrigger& right) const {
    return (period_ - right.period_) * CircleTime + (static_cast<int>(time_align) - static_cast<int>(right.time_align));
}

int SciTrigger::operator-(const SciEvent& right) const {
    return (period_ - right.get_period()) * CircleTime + (static_cast<int>(time_align) - static_cast<int>(right.time_align));
}

void SciTrigger::print(const Counter& cnt, ostream& os) {
    os << uppercase;
    os << cnt.packet << " trigger ";
    os << hex << setfill('0') << setw(4) << mode << dec << " ";
    os << time_align << " ";
    os << packet_num << " > ";
    os << "|";
    for (int i = 0; i < 25; i++)
        if (trig_accepted[i] == 1)
            os << i + 1 << "|";
    os << " < ";
    os << " | " << timestamp << " ";
    os << endl;
    os << nouppercase;
}
