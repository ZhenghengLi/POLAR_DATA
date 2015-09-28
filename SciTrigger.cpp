#include "SciTrigger.hpp"

using namespace std;

void SciTrigger::set_mode_(const char* packet_buffer, size_t packet_len) {
	mode = 0;
	for (int i = 0; i < 2; i++) {
		mode <<= 8;
		mode += static_cast<uint8_t>(packet_buffer[2 + i]);
	}
}

void SciTrigger::set_timestamp_(const char* packet_buffer, size_t packet_len) {
	timestamp = 0;
	for (int i = 0; i < 4; i++) {
		timestamp <<= 8;
		timestamp += static_cast<uint8_t>(packet_buffer[4 + i]);
	}
}

void SciTrigger::set_packet_num_(const char* packet_buffer, size_t packet_len) {
	packet_num = 0;
	for (int i = 0; i < 2; i++) {
		packet_num <<= 8;
		packet_num += static_cast<uint8_t>(packet_buffer[10 + i]);
	}
} 

void SciTrigger::set_trig_accepted_(const char* packet_buffer, size_t packet_len) {
	
}

void SciTrigger::update(const char* packet_buffer, size_t packet_len) {
	set_mode_(packet_buffer, packet_len);
	set_timestamp_(packet_buffer, packet_len);
	set_packet_num_(packet_buffer, packet_len);
}

