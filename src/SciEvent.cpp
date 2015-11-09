#include "SciEvent.hpp"
#include "SciTrigger.hpp"

using namespace std;

void SciEvent::set_mode_(const char* packet_buffer, size_t packet_len) {
	mode = decode_bit<uint16_t>(packet_buffer + 6, 7, 8);
}

void SciEvent::set_ct_num_(const char* packet_buffer, size_t packet_len) {
	ct_num = decode_byte<uint16_t>(packet_buffer, 2, 3);
}

void SciEvent::set_timestamp_(const char* packet_buffer, size_t packet_len) {
	timestamp = static_cast<uint8_t>(packet_buffer[17]);
	for (int i = 0; i < 2; i++) {
		timestamp <<= 8;
		timestamp += static_cast<uint8_t>(packet_buffer[4 + i]);
	}
}

void SciEvent::set_time_align_() {
	time_align = timestamp & 0x7FFFFF;
}

void SciEvent::set_deadtime_(const char* packet_buffer, size_t packet_len) {
	deadtime = decode_byte<uint16_t>(packet_buffer, 20, 21);
}

void SciEvent::set_trigger_bit_(const char* packet_buffer, size_t packet_len) {
	for(int i = 0; i < 64; i++) {
		trigger_bit[i] = decode_bit<uint16_t>(packet_buffer + 8, i, i);
	}
}

void SciEvent::set_energy_ch_(const char* packet_buffer, size_t packet_len) {
	if (mode == 0 || mode == 2) {
		for (int i = 0; i < 64; i++) {
			energy_ch[TriggerIndex[i]] = decode_bit<uint16_t>(packet_buffer + 22, 12 * i, 12 * i + 11);
		}
	} else if (mode == 1) {
		for (int i = 0; i < 64; i++) {
			energy_ch[i] = 0;
		}
		uint16_t tmp_ch;
		int cur_pos;
		cur_pos = -1;
		for (int i = 0; i < static_cast<int>((packet_len - 13 * 2) / 2); i++) {
			for (int j = cur_pos + 1; j < 64; j++) {
				if (trigger_bit[j] == 1) {
					cur_pos = j;
					break;
				}
			}
			tmp_ch = decode_byte<uint16_t>(packet_buffer + 22, 2 * i, 2 * i + 1);
			energy_ch[cur_pos] = tmp_ch & 0xFFF;
		}
	} else if (mode == 3) {
		for (int i = 0; i < 64; i++) {
			energy_ch[i] = 0;
		}
		uint32_t position_channel;
		uint32_t position;
		uint32_t channel;
		for (int i = 0; i < static_cast<int>((packet_len * 8 - 14 * 16) / 18); i++) {
			position_channel = decode_bit<uint32_t>(packet_buffer + 24, 18 * i , 18 * i + 17);
			position = position_channel & 0x3F000;
			position >>= 12;
			channel = position_channel & 0xFFF;
			energy_ch[TriggerIndex[position]] = static_cast<uint16_t>(channel);
		}
	}
}

void SciEvent::set_common_noise_(const char* packet_buffer, size_t packet_len) {
	if (mode == 3) {
		common_noise = decode_bit<uint16_t>(packet_buffer + 22, 4, 15);
	} else {
		common_noise = 0;
	}
}

void SciEvent::update(const char* packet_buffer, size_t packet_len) {
	set_mode_(packet_buffer, packet_len);
	set_ct_num_(packet_buffer, packet_len);
	set_timestamp_(packet_buffer, packet_len);
	set_time_align_();
	set_deadtime_(packet_buffer, packet_len);
	set_trigger_bit_(packet_buffer, packet_len);
	set_energy_ch_(packet_buffer, packet_len);
	set_common_noise_(packet_buffer, packet_len);
}

int SciEvent::get_period() const {
	return period_;
}

void SciEvent::set_period(int pt) {
	period_ = pt;
}

bool SciEvent::operator<(const SciEvent& right) const {
	if (period_ < right.period_) {
		return true;
	} else if (period_ > right.period_) {
		return false;
	} else {
		return (time_align < right.time_align);
	}
}

bool SciEvent::operator<=(const SciEvent& right) const {
	if (period_ < right.period_) {
		return true;
	} else if (period_ > right.period_) {
		return false;
	} else {
		return (time_align <= right.time_align);
	}
}

bool SciEvent::operator>(const SciEvent& right) const {
	if (period_ > right.period_) {
		return true;
	} else if (period_ < right.period_) {
		return false;
	} else {
		return (time_align > right.time_align);
	}
}

bool SciEvent::operator>=(const SciEvent& right) const {
	if (period_ > right.period_) {
		return true;
	} else if (period_ < right.period_) {
		return false;
	} else {
		return (time_align >= right.time_align);
	}
}

bool SciEvent::operator==(const SciEvent& right) const {
	if (period_ > right.period_) {
		return false;
	} else if (period_ < right.period_) {
		return false;
	} else {
		return (time_align == right.time_align);
	}
}

int SciEvent::operator-(const SciEvent& right) const {
	return (period_ - right.period_) * CircleTime + (time_align - right.time_align);
}

int SciEvent::operator-(const SciTrigger& right) const {
	return (period_ - right.get_period()) * CircleTime + (time_align - right.time_align);
}

void SciEvent::print(const Counter& cnt, ostream& os) {
	os << cnt.packet << " event ";
	os << mode << " ";
	os << time_align << " ";
	os << ct_num << " ";
	os << "| " << timestamp;
	os << endl;
}

