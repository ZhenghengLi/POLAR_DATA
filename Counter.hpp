#ifndef COUNTER_H
#define COUNTER_H

struct Counter {
    int frame;
    int packet;
    int trigger;
    int event;
	int frm_con_error;
    int pkt_valid;
    int pkt_invalid;
    int pkt_crc_passed;
    int pkt_crc_error;
    int pkt_too_short;
};

#endif
