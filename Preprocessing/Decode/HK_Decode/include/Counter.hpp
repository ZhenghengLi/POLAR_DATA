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
    int64_t frame;
    int64_t frm_valid;
    int64_t frm_invalid;
    int64_t frm_crc_passed;
    int64_t frm_crc_error;
    int64_t frm_con_error;
    int64_t obox_packet;
    int64_t obox_valid;
    int64_t obox_invalid;
    int64_t obox_crc_passed;
    int64_t obox_crc_error;
    
public:
    void clear();
    void print(ostream& os = cout);
    
};

#endif
