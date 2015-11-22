#ifndef SCITRIGGER_H
#define SCITRIGGER_H

#include <iostream>
#include <cstddef>
#include <cassert>
#include <iomanip>
#include <stdint.h>
#include "Decoder.hpp"
#include "Counter.hpp"
#include "Constants.hpp"

class SciEvent;

class SciTrigger: private Decoder {
private:
	int period_;
	int pkt_count_;
	int lost_count_;
public:
	uint16_t mode;
	uint32_t timestamp;
	uint32_t time_align;
	uint16_t status_;
	uint16_t packet_num;
	uint8_t trig_sig_con[25];
	uint16_t trig_accepted[25];
	uint16_t trig_rejected[25];
	uint16_t deadtime;
	uint64_t frm_ship_time;
	uint64_t frm_gps_time;
private:
	void set_mode_(const char* packet_buffer, size_t packet_len);
	void set_timestamp_(const char* packet_buffer, size_t packet_len);
	void set_time_align_();
	void set_packet_num_(const char* packet_buffer, size_t packet_len);
	void set_trig_accepted_(const char* packet_buffer, size_t packet_len);
	void set_trig_rejected_(const char* packet_buffer, size_t packet_len);
	void set_deadtime_(const char* packet_buffer, size_t packet_len);
	void set_status_(const char* packet_buffer, size_t packet_len);
	void set_trig_sig_con_(const char* packet_buffer, size_t packet_len);
public:
	void update(const char* packet_buffer, size_t packet_len);
	void set_frm_time(uint64_t ship_time, uint64_t gps_time);
	void print(const Counter& cnt, ostream& os = cout);
public:
	int get_period() const;
	void set_period(int pt);
	int get_pkt_count() const;
	void set_pkt_count(int pkt_cnt);
	int get_lost_count() const;
	void set_lost_count(int lost_cnt);
	bool operator<(const SciTrigger& right) const;
	bool operator<=(const SciTrigger& right) const;
	bool operator>(const SciTrigger& right) const;
	bool operator>=(const SciTrigger& right) const;
	bool operator==(const SciTrigger& right) const;
	int operator-(const SciTrigger& right) const;
	int operator-(const SciEvent& right) const;
};

#endif
