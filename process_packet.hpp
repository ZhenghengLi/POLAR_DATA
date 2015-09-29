#ifndef PROCESS_PACKET_H
#define PROCESS_PACKET_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "SciFrame.hpp"
#include "SciEvent.hpp"
#include "SciTrigger.hpp"
#include "Counter.hpp"

using namespace std;

SciEvent gSciEvent;
SciTrigger gSciTrigger;

void process_packet(SciFrame& frame, Counter& cnt) {
	cnt.packet++;
	// check packet
	bool is_trigger = frame.cur_is_trigger();
	if (is_trigger) {
		cnt.trigger++;
	} else {
		cnt.event++;
	}
	bool tmp_valid = frame.cur_check_valid();
	bool tmp_crc = frame.cur_check_crc();
	if (tmp_valid) {
		cnt.pkt_valid++;
	} else {
		cout << endl;
		cout << "packet invalid: " << cnt.packet << endl;
		frame.cur_print_packet();
		cnt.pkt_invalid++;
	}
	if (tmp_crc) {
		cnt.pkt_crc_passed++;
	} else {
		cout << endl;
		cout << "packet crc error: " << cnt.packet << endl;
		frame.cur_print_packet();
		cnt.pkt_crc_error++;
	}
	if (frame.get_cur_pkt_len() < 28) {
		cout << "packet too short: " << cnt.packet << endl;
		frame.cur_print_packet();
		cnt.pkt_too_short++;
	}
	if (!(tmp_valid & tmp_crc))
		return;
	
	// start process packet
	if (is_trigger) {
		gSciTrigger.update(frame.get_cur_pkt_buf(), frame.get_cur_pkt_len());
		gSciTrigger.print(cnt);
	} else {
		gSciEvent.update(frame.get_cur_pkt_buf(), frame.get_cur_pkt_len());
		gSciEvent.print(cnt);
	}
	
	
}

#endif
