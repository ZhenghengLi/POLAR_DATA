#ifndef COUNTER_H
#define COUNTER_H

struct Counter {
    int64_t frame;
    int64_t packet;
    int64_t trigger;
    int64_t event;
	int64_t frm_con_error;
    int64_t pkt_valid;
    int64_t pkt_invalid;
    int64_t pkt_crc_passed;
    int64_t pkt_crc_error;
    int64_t pkt_too_short;
};

#endif
