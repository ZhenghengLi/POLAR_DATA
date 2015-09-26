#include "SciEvent.hpp"

using namespace std;

void SciEvent::set_mode_(const char* packet_buffer, uint16_t packet_len) {
	mode = 0;
	for (int i = 0; i < 2; i++) {
		mode <<= 8;
		mode += static_cast<uint8_t>(packet_buffer[6 + i]);
	}
	mode &= 0x180;
	mode >>= 7;
}

void SciEvent::set_ct_num_(const char* packet_buffer, uint16_t packet_len) {
	ct_num = 0;
	for (int i = 0; i < 2; i++) {
		ct_num <<= 8;
		ct_num += static_cast<uint8_t>(packet_buffer[2 + i]);
	}
}

void SciEvent::set_timestamp_(const char* packet_buffer, uint16_t packet_len) {
	timestamp = static_cast<uint8_t>(packet_buffer[17]);
	for (int i = 0; i < 2; i++) {
		timestamp <<= 8;
		timestamp += static_cast<uint8_t>(packet_buffer[4 + i]);
	}
}

void SciEvent::set_deadtime_(const char* packet_buffer, uint16_t packet_len) {
	deadtime = 0;
	for (int i = 0; i < 2; i++) {
		deadtime <<= 8;
		deadtime += static_cast<uint8_t>(packet_buffer[20 + i]);
	}
}

void SciEvent::update(const char* packet_buffer, uint16_t packet_len) {
	set_mode_(packet_buffer, packet_len);
	set_ct_num_(packet_buffer, packet_len);
	set_timestamp_(packet_buffer, packet_len);
	set_deadtime_(packet_buffer, packet_len);
}
