#ifndef COUNTER_H
#define COUNTER_H

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <stdint.h>
#include "Constants.hpp"

using namespace std;

class Counter {
public:
    // packet
    int64_t frame;
    int64_t packet;
    int64_t trigger;
    int64_t ped_trigger;
    int64_t sec_ped_trigger;
    int64_t tin_ped_trigger;
    int64_t noped_trigger;
    int64_t event;
    int64_t frm_con_error;
    int64_t frm_valid;
    int64_t frm_invalid;
    int64_t frm_crc_passed;
    int64_t frm_crc_error;
    int64_t frm_start_error;
    int64_t pkt_valid;
    int64_t pkt_invalid;
    int64_t pkt_crc_passed;
    int64_t pkt_crc_error;
    int64_t pkt_too_short;
    // module
    int64_t ped_trig[25];
    int64_t ped_event[25];
    int64_t noped_trig[25];
    int64_t noped_event[25];
    int64_t aligned_event_sum;
public:
    void clear();
    void print();
};

#endif
