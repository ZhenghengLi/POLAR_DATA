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
    int16_t frm_index;
    int16_t pkt_tag;

public:
    int32_t is_bad;
    
private:
    void set_frm_index_();
    void set_pkt_tag_();
    
public:
    void update(int32_t cur_is_bad);
    
public:
    HkEven();
    HkEven(const char*);
    ~HkEven();
    void setdata(const char*);
    
};

#endif
