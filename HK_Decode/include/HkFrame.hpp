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
    crc_optimal<16, 0x1021, 0xFFFF>  crc_ccitt_;
    uint16_t pre_frame_index_;
    bool processed_flag_;
    char* obox_raw_data_;

public:
    // decoded info is here
    uint64_t ship_time;
    uint16_t error[2];
    uint16_t frame_head;
    uint16_t command_head;
    uint16_t command_num;
    uint16_t command_code;
    uint16_t command_arg[2];
    uint16_t head;
    uint16_t tail;
    uint64_t ibox_gps;
    
public:
    int32_t is_bad;

private:
    // decoding function is here
    void set_ship_time_();
    void set_error_();
    void set_frame_head_();
    void set_command_head_();
    void set_command_num_();
    void set_command_code_();
    void set_command_arg_();
    void set_head_();
    void set_tail_();
    void set_ibox_gps_();

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
    void obox_copy_odd();
    void obox_copy_even();
    bool obox_check_valid();
    bool obox_check_crc();
    
};

#endif
