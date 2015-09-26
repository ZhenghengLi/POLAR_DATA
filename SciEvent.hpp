#ifndef SCIEVENT_H
#define SCIEVENT_H

#include <iostream>
#include <cstddef>
#include <cassert>
#include <iomanip>
#include <stdint.h>

class SciEvent {
public:
	uint16_t mode;	
	uint16_t ct_num;
	uint32_t timestamp;
	uint16_t status;
	uint16_t trigger_bit[64];
	uint16_t rate;
	uint16_t deadtime;
	uint16_t energy_ch[64];

	uint16_t common_noise;
	
private:
	void set_mode_(const char* packet_buffer, uint16_t packet_len);
	void set_ct_num_(const char* packet_buffer, uint16_t packet_len);
	void set_timestamp_(const char* packet_buffer, uint16_t packet_len);
	void set_deadtime_(const char* packet_buffer, uint16_t packet_len);
	
public:
	void update(const char* packet_buffer, uint16_t packet_len);

};

#endif
