#include "SciEvent.hpp"

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

void SciEvent::set_deadtime_(const char* packet_buffer, size_t packet_len) {
	deadtime = decode_byte<uint16_t>(packet_buffer, 20, 21);
}

void SciEvent::update(const char* packet_buffer, size_t packet_len) {
	set_mode_(packet_buffer, packet_len);
	set_ct_num_(packet_buffer, packet_len);
	set_timestamp_(packet_buffer, packet_len);
	set_deadtime_(packet_buffer, packet_len);
}

void SciEvent::print(const Counter& cnt) {
	if (mode == 2) {
		cout << cnt.packet << " event ";
		cout << mode << " ";
		cout << (timestamp & 0x1FFFFF) << " ";
		cout << ct_num << " ";
		cout << endl;
	}
}
