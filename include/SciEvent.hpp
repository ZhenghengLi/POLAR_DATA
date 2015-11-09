#ifndef SCIEVENT_H
#define SCIEVENT_H

#include <iostream>
#include <cstddef>
#include <cassert>
#include <iomanip>
#include <stdint.h>
#include "Decoder.hpp"
#include "Counter.hpp"
#include "Constants.hpp"

class SciTrigger;

class SciEvent: private Decoder {
private:
	int period_;
public:
	uint16_t mode;	
	uint16_t ct_num;
	uint32_t timestamp;
	uint32_t time_align;
//	uint16_t status;
	uint16_t trigger_bit[64];
	uint16_t rate;
	uint16_t deadtime;
	uint16_t energy_ch[64];
	uint16_t common_noise;
private:
	void set_mode_(const char* packet_buffer, size_t packet_len);
	void set_ct_num_(const char* packet_buffer, size_t packet_len);
	void set_timestamp_(const char* packet_buffer, size_t packet_len);
	void set_time_align_();
	void set_deadtime_(const char* packet_buffer, size_t packet_len);
	void set_trigger_bit_(const char* packet_buffer, size_t packet_len);
	void set_energy_ch_(const char* packet_buffer, size_t packet_len);
	void set_common_noise_(const char* packet_buffer, size_t packet_len);
public:
	void update(const char* packet_buffer, size_t packet_len);
	void print(const Counter& cnt, ostream& os = cout);
public:
	int get_period() const;
	void set_period(int pt);
	bool operator<(const SciEvent& right) const;
	bool operator<=(const SciEvent& right) const;
	bool operator>(const SciEvent& right) const;
	bool operator>=(const SciEvent& right) const;
	bool operator==(const SciEvent& right) const;
	int operator-(const SciEvent& right) const;
	int operator-(const SciTrigger& right) const;
};

#endif
