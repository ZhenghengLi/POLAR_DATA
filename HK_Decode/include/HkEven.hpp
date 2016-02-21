#ifndef HKEVEN_H
#define HKEVEN_H

#include <iostream>
#include <cstddef>
#include <stdint.h>
#include "Decoder.hpp"

class HkEven: private Decoder {
private:
    const char* frame_data_;

public:
    // each field of Even Packet is here

public:
    void update();
    void clear_all_info();
    
public:
    HkEven();
    HkEven(const char*);
    ~HkEven();
    void setdata(const char*);
    
};

#endif
