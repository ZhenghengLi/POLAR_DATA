#ifndef HKODD_H
#define HKODD_H

#include <iostream>
#include <cstddef>
#include <stdint.h>
#include "Decoder.hpp"

class HkOdd: private Decoder {
private:
    const char* frame_data_;

public:
    // each field of Odd Packet is here

public:
    void update();
    void clear_all_info();

public:
    HkOdd();
    HkOdd(const char*);
    ~HkOdd();
    void setdata(const char*);

};

#endif
