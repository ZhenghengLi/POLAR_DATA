#ifndef SCITRIGGER_H
#define SCITRIGGER_H

#include <iostream>
#include <cstddef>
#include <cassert>
#include <iomanip>
#include <stdint.h>

class SciTrigger {
public:
	uint16_t mode;
	uint32_t timestamp;
	uint16_t status_reg;
	uint16_t packet_num;
	uint8_t trig_sig_con[25];
	uint16_t trig_accepted[25];
	uint16_t trig_rejected[25];
	
private:
	void set_mode_(const char* packet_buffer, size_t packet_len);
	void set_timestamp_(const char* packet_buffer, size_t packet_len);
	void set_packet_num_(const char* packet_buffer, size_t packet_len);
	void set_trig_accepted_(const char* packet_buffer, size_t packet_len);
	
public:
	void update(const char* packet_buffer, size_t packet_len);

};

#endif
