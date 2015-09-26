#ifndef PROCESS_PACKET_H
#define PROCESS_PACKET_H

#include <iostream>
#include <cstdlib>
#include "SciFrame.hpp"
#include "SciEvent.hpp"
#include "SciTrigger.hpp"

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
	
	
	
}

#endif
