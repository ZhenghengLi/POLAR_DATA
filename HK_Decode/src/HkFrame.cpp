#include "HkFrame.hpp"
#include <cstring>

using namespace std;
using boost::crc_optimal;

HkFrame::HkFrame() {
    frame_data_ = NULL;
    processed_flag_ = false;
    obox_raw_data_ = new char[386];
}

HkFrame::HkFrame(const char* data) {
    frame_data_ = data;
    processed_flag_ = false;
    obox_raw_data_ = new char[386];
}

HkFrame::~HkFrame() {
    delete [] obox_raw_data_;
}

void HkFrame::setdata(const char* data) {
    frame_data_ = data;
}

void HkFrame::updated() {
    processed_flag_ = false;
}

bool HkFrame::check_valid() {
    assert(frame_data_ != NULL);
    uint16_t frame_header = 0;
    for (int i = 0; i < 2; i++) {
        frame_header <<= 8;
        frame_header += static_cast<uint8_t>(frame_data_[0 + i]);
    }
    if (frame_header != 0x0749)
        return false;
    uint16_t frame_length = 0;
    for (int i = 0; i < 2; i++) {
        frame_length <<= 8;
        frame_length += static_cast<uint8_t>(frame_data_[4 + i]);
    }
    if (frame_length != 0x00F9)
        return false;
    uint32_t hk_header = 0;
    for (int i = 0; i < 4; i++) {
        hk_header <<= 8;
        hk_header += static_cast<uint8_t>(frame_data_[6 + i]);
    }
    if (hk_header != 0xD9A4C2EA)
        return false;
    uint16_t packet_tag = 0;
    for (int i = 0; i < 2; i++) {
        packet_tag <<= 8;
        packet_tag += static_cast<uint8_t>(frame_data_[10 + i]);
    }
    if (packet_tag > 1)
        return false;
    
    return true;
}

bool HkFrame::check_crc() {
    assert(frame_data_ != NULL);
    uint32_t expected, result;
    expected = 0;
    for (int i = 0; i < 4; i++) {
        expected <<= 8;
        expected += static_cast<uint8_t>(frame_data_[256 + i]);
    }
    crc_32_.reset();
    crc_32_.process_bytes(frame_data_, 256);
    result = crc_32_.checksum();
    if (result == expected)
        return true;
    else
        return false;
}

uint16_t HkFrame::get_bits() const {
    assert(frame_data_ != NULL);
    uint8_t tmp = 0;
    tmp += static_cast<uint8_t>(frame_data_[2]);
    return static_cast<uint16_t>(tmp >> 6);
}

uint16_t HkFrame::get_index() const {
    assert(frame_data_ != NULL);
    uint16_t sum = 0;
    sum += static_cast<uint8_t>(frame_data_[2] & 0x3F);
    sum <<= 8;
    sum += static_cast<uint8_t>(frame_data_[3]);
    return sum;
}

uint16_t HkFrame::get_pkt_tag() const {
    assert(frame_data_ != NULL);
    uint16_t pkt_tag = 0;
    for (int i = 0; i < 2; i++) {
        pkt_tag <<= 8;
        pkt_tag += static_cast<uint8_t>(frame_data_[10 + i]);
    }
    return pkt_tag;
}

void HkFrame::processed() {
    assert(frame_data_ != NULL);
    pre_frame_index_ = get_index();
    processed_flag_ = true;
}

bool HkFrame::can_connect() {
    assert(frame_data_ != NULL);
    assert(!processed_flag_);
    uint16_t cur_frm_index = get_index();
    if (cur_frm_index == pre_frame_index_ + 1 ||
        (pre_frame_index_ == 16383 && cur_frm_index == 0))
        return true;
    else
        return false;
}

void HkFrame::set_ship_time_() {
    ship_time = decode_byte<int64_t>(frame_data_, 12, 17);
}

void HkFrame::set_error_() {
    error[0] = decode_byte<int16_t>(frame_data_, 18, 19);
    error[1] = decode_byte<int16_t>(frame_data_, 20, 21);
}

void HkFrame::set_frame_head_() {
    frame_head = decode_byte<int16_t>(frame_data_, 22, 23);
}

void HkFrame::set_command_head_() {
    command_head = decode_byte<int16_t>(frame_data_, 24, 25);
}

void HkFrame::set_command_num_() {
    command_num = decode_byte<int16_t>(frame_data_, 26, 27);
}

void HkFrame::set_command_code_() {
    command_code = decode_byte<int16_t>(frame_data_, 28, 29);
}

void HkFrame::set_command_arg_() {
    command_arg[0] = decode_byte<int16_t>(frame_data_, 30, 31);
    command_arg[1] = decode_byte<int16_t>(frame_data_, 32, 33);
}

void HkFrame::set_head_() {
    head = decode_byte<int16_t>(frame_data_, 248, 248);
}

void HkFrame::set_tail_() {
    tail = decode_byte<int16_t>(frame_data_, 249, 249);
}

void HkFrame::set_ibox_gps_() {
    ibox_gps = decode_byte<int64_t>(frame_data_, 250, 255);
}

void HkFrame::update_ibox_info(int32_t cur_is_bad) {
    is_bad = cur_is_bad;
    set_ship_time_();
    set_error_();
    set_frame_head_();
    set_command_head_();
    set_command_num_();
    set_command_arg_();
    set_head_();
    set_tail_();
    set_ibox_gps_();
}

void HkFrame::obox_copy_odd() {
    memcpy(obox_raw_data_ + 0, frame_data_ + 34, 214);
}

void HkFrame::obox_copy_even() {
    memcpy(obox_raw_data_ + 214, frame_data_ + 34, 172);
}

bool HkFrame::obox_check_valid() {
    int16_t obox_header = 0;
    for (int i = 0; i < 2; i++) {
        obox_header <<= 8;
        obox_header += static_cast<uint8_t>(obox_raw_data_[0 + i]);
    }
    if (!(obox_header == 0x20C0 || obox_header == 0x28C0))
        return false;

    return true;
}

bool HkFrame::obox_check_crc() {
    uint16_t expected, result;
    expected = 0;
    for (int i = 0; i < 2; i++) {
        expected <<= 8;
        expected += static_cast<uint8_t>(obox_raw_data_[384 + i]);
    }
    crc_ccitt_.reset();
    crc_ccitt_.process_bytes(obox_raw_data_ + 2, 382);
    result = crc_ccitt_.checksum();
    if (result == expected)
        return true;
    else
        return false;
}
