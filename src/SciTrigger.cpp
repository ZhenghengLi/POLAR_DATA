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
	for (int i = 0; i < 25; i++) {
		trig_accepted[i] = decode_bit<uint16_t>(packet_buffer + 37, i, i);
	}
}

void SciTrigger::set_trig_rejected_(const char* packet_buffer, size_t packet_len) {
	for (int i = 0; i < 25; i++) {
		trig_rejected[i] = decode_bit<uint16_t>(packet_buffer + 40, 1 + i, 1 + i);
	}
}

void SciTrigger::update(const char* packet_buffer, size_t packet_len) {
	set_mode_(packet_buffer, packet_len);
	set_timestamp_(packet_buffer, packet_len);
	set_packet_num_(packet_buffer, packet_len);
	set_trig_accepted_(packet_buffer, packet_len);
	set_trig_rejected_(packet_buffer, packet_len);
}

void SciTrigger::print(const Counter& cnt) {
	if (mode == 0x00F0) {
		cout << uppercase;
		cout << cnt.packet << " trigger ";
		cout << hex << setfill('0') << setw(4) << mode << dec << " ";
		cout << (timestamp >> 11) << " ";
		cout << packet_num << " > ";
		cout << "|";
		for (int i = 0; i < 25; i++)
			if (trig_accepted[i] == 1)
				cout << i + 1 << "|";
		cout << " < ";
		cout << " | " << timestamp << " ";
		cout << endl;
		cout << nouppercase;
	}
}
