#ifndef HKFRAME_H
#define HKFRAME_H

#include <iostream>
#include <cstddef>
#include <cassert>
#include <iomanip>
#include <stdint.h>
#include <boost/crc.hpp>
#include "Decoder.hpp"

using namespace std;
using boost::crc_optimal;

class HkFrame: private Decoder {
private:
    const char* frame_data_;
    crc_optimal<32, 0x04C11DB7> crc_32_;
    uint16_t pre_frame_index_;
    bool processed_flag_;

public:
    // decoded info is here
    
public:
    int32_t is_bad;

private:
    // decoding function is here

public:
    void update_ibox_info(int32_t cur_is_bad);
    
public:
    HkFrame();
    HkFrame(const char*);
    ~HkFrame();
    void setdata(const char*);
    void updated();
    bool check_valid();
    bool check_crc();
    uint16_t get_bits() const;
    uint16_t get_index() const;
    uint16_t get_pkt_tag() const;
    void processed();    
    bool can_connect();
    
};

#endif
