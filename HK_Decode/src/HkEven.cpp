#include "HkEven.hpp"

using namespace std;

HkEven::HkEven() {

}

HkEven::HkEven(const char* data) {
    frame_data_ = data;
}

HkEven::~HkEven() {

}

void HkEven::setdata(const char* data) {
    frame_data_ = data;
}

void HkEven::clear_all_info() {
    is_bad = 0;
    frm_index = 0;
    pkt_tag = 0;
}

void HkEven::set_frm_index_() {
    frm_index = decode_bit<int16_t>(frame_data_ + 2, 2, 15);
}

void HkEven::set_pkt_tag_() {
    pkt_tag = decode_byte<int16_t>(frame_data_, 10, 11);
}

void HkEven::update(int32_t cur_is_bad) {
    is_bad = cur_is_bad;
    set_frm_index_();
    set_pkt_tag_();
}
