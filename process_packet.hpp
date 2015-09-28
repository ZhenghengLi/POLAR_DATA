#ifndef PROCESS_PACKET_H
#define PROCESS_PACKET_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "SciFrame.hpp"
#include "SciEvent.hpp"
#include "SciTrigger.hpp"

using namespace std;

SciEvent gSciEvent;
SciTrigger gSciTrigger;


struct Counter {
	int frame;
	int packet;
	int trigger;
	int event;
	int pkt_valid;
	int pkt_invalid;
	int pkt_crc_passed;
	int pkt_crc_error;
	int pkt_too_short;
};

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
	cout << uppercase;
	if (is_trigger) {
		gSciTrigger.update(frame.get_cur_pkt_buf(), frame.get_cur_pkt_len());
		cout << cnt.packet << " trigger ";
		cout << hex << setfill('0') << setw(4) << gSciTrigger.mode << dec << " ";
		cout << hex << setfill('0') << setw(8) << gSciTrigger.timestamp << dec << " ";
		cout << gSciTrigger.packet_num << endl;
	} else {
		gSciEvent.update(frame.get_cur_pkt_buf(), frame.get_cur_pkt_len());
		cout << cnt.packet << " event ";
		cout << gSciEvent.mode << " ";
		cout << hex << setfill('0') << setw(6) << gSciEvent.timestamp << dec << " ";
		cout << gSciEvent.ct_num << " ";
		cout << gSciEvent.deadtime << endl;
	}
	
	
}

#endif
