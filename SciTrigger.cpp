#include "SciTrigger.hpp"

using namespace std;

void SciTrigger::set_mode_(const char* packet_buffer, size_t packet_len) {
	mode = decode_byte<uint16_t>(packet_buffer, 2, 3);
}

void SciTrigger::set_timestamp_(const char* packet_buffer, size_t packet_len) {
	timestamp = decode_byte<uint32_t>(packet_buffer, 4, 7);
}

void SciTrigger::set_packet_num_(const char* packet_buffer, size_t packet_len) {
	packet_num = decode_byte<uint16_t>(packet_buffer, 10, 11);
} 

void SciTrigger::set_trig_accepted_(const char* packet_buffer, size_t packet_len) {
	
}

void SciTrigger::update(const char* packet_buffer, size_t packet_len) {
	set_mode_(packet_buffer, packet_len);
	set_timestamp_(packet_buffer, packet_len);
	set_packet_num_(packet_buffer, packet_len);
}

