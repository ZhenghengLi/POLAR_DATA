#include "HkFrame.hpp"

using namespace std;
using boost::crc_optimal;

HkFrame::HkFrame() {
    frame_data_ = NULL;
    processed_flag_ = false;
}

HkFrame::HkFrame(const char* data) {
    frame_data_ = data;
    processed_flag_ = false;
}

HkFrame::~HkFrame() {

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
