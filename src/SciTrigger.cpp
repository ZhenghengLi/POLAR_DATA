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

void SciTrigger::update(const char* packet_buffer, size_t packet_len) {
	set_mode_(packet_buffer, packet_len);
	set_timestamp_(packet_buffer, packet_len);
	set_time_align_();
	set_packet_num_(packet_buffer, packet_len);
	set_trig_accepted_(packet_buffer, packet_len);
	set_trig_rejected_(packet_buffer, packet_len);
	set_deadtime_(packet_buffer, packet_len);
}

int SciTrigger::get_period() const {
	return period;
}

void SciTrigger::set_period(int pt) {
	period = pt;
}

bool SciTrigger::operator<(const SciTrigger& right) const {
	if (period < right.period) {
		return true;
	} else if (period > right.period) {
		return false;
	} else {
		return (time_align < right.time_align);
	}
}

bool SciTrigger::operator<=(const SciTrigger& right) const {
	if (period < right.period) {
		return true;
	} else if (period > right.period) {
		return false;
	} else {
		return (time_align <= right.time_align);
	}
}

bool SciTrigger::operator>(const SciTrigger& right) const {
	if (period > right.period) {
		return true;
	} else if (period < right.period) {
		return false;
	} else {
		return (time_align > right.time_align);
	}
}

bool SciTrigger::operator>=(const SciTrigger& right) const {
	if (period > right.period) {
		return true;
	} else if (period < right.period) {
		return false;
	} else {
		return (time_align >= right.time_align);
	}
}

bool SciTrigger::operator==(const SciTrigger& right) const {
	if (period > right.period) {
		return false;
	} else if (period < right.period) {
		return false;
	} else {
		return (time_align == right.time_align);
	}
}

int SciTrigger::operator-(const SciTrigger& right) const {
	return (period - right.period) * CircleTime + (time_align - right.time_align);
}

int SciTrigger::operator-(const SciEvent& right) const {
	return (period - right.get_period()) * CircleTime + (time_align - right.time_align);
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
