#ifndef SCIFRAME_H
#define SCIFRAME_H

#include <iostream>
#include <cstddef>
#include <cassert>
#include <iomanip>
#include <stdint.h>
#include <boost/crc.hpp>

using namespace std;
using boost::crc_optimal;

class SciFrame {
private:
    const char* frame_data_;    
    crc_optimal<32, 0x04C11DB7> crc_32_;
    crc_optimal<16, 0x1021, 0xFFFF>  crc_ccitt_;
    char* pre_half_packet_; 
    uint16_t pre_half_packet_len_;
    uint16_t pre_frame_index_;
    uint16_t start_packet_pos_;
    uint16_t cur_packet_pos_;
    uint16_t cur_packet_len_;
    const char* cur_packet_buffer_;
    bool cur_is_cross_;
    bool reach_end_;
    uint64_t frm_ship_time_;
    uint64_t frm_gps_time_;
    
private:
    uint16_t get_cur_packet_len_() {
        return 2 * (static_cast<uint16_t>(
            static_cast<uint8_t>(
                frame_data_[cur_packet_pos_ + 1])) + 1);
    }
    void copy_frame_(const SciFrame& other_frame);
public:
    SciFrame();
    SciFrame(const char*);
    SciFrame(const SciFrame& other_frame);
    ~SciFrame();
    const SciFrame& operator=(const SciFrame& other_frame);
    void setdata(const char*);
    void reset();
    bool check_valid() const;
    uint16_t get_bits() const;
    uint16_t get_index() const;
    bool check_crc();
    bool next_packet();
    void updated();
    bool cur_is_trigger() const;
    uint16_t cur_get_mode() const;
    uint16_t cur_get_ctNum() const;
    bool cur_check_crc();
    bool cur_check_valid() const;
    bool find_start_pos();
    const char* get_cur_pkt_buf();
    size_t get_cur_pkt_len();
    bool can_connect();
    void cur_print_packet(ostream& os = cout);
    void update_time();
    uint64_t get_ship_time();
    uint64_t get_gps_time();
};

#endif
