#include "HkOdd.hpp"

using namespace std;

HkOdd::HkOdd() {

}

HkOdd::HkOdd(const char* data) {
    frame_data_ = data;
}

HkOdd::~HkOdd() {

}

void HkOdd::setdata(const char* data) {
    frame_data_ = data;
}

void HkOdd::set_frm_index_() {
    frm_index = decode_bit<uint16_t>(frame_data_ + 2, 2, 15);
}

void HkOdd::set_pkt_tag_() {
    pkt_tag = decode_byte<uint16_t>(frame_data_, 10, 11);
}

void HkOdd::set_packet_num_() {
    packet_num = decode_byte<uint16_t>(frame_data_, 36, 37);
}

void HkOdd::set_timestamp_() {
    timestamp = decode_byte<uint32_t>(frame_data_, 38, 41);
}

void HkOdd::set_obox_mode_() {
    obox_mode = decode_bit<uint8_t>(frame_data_ + 42, 0, 3);
}

void HkOdd::set_cpu_status_() {
    cpu_status = decode_bit<uint16_t>(frame_data_ + 42, 4, 15);
}

void HkOdd::set_trig_status_() {
    trig_status = decode_byte<uint8_t>(frame_data_, 44, 44);
}

void HkOdd::set_comm_status_() {
    comm_status = decode_byte<uint8_t>(frame_data_, 45, 45);
}

void HkOdd::set_ct_temp_() {
    ct_temp = decode_byte<uint16_t>(frame_data_, 46, 47);
}

void HkOdd::set_chain_temp_() {
    chain_temp = decode_byte<uint16_t>(frame_data_, 48, 49);
}

void HkOdd::set_reserved_() {
    reserved = decode_byte<uint16_t>(frame_data_, 50, 51);
}

void HkOdd::set_lv_status_() {
    lv_status = decode_byte<uint16_t>(frame_data_, 52, 53);
}

void HkOdd::set_fe_pattern_() {
    fe_pattern = decode_byte<uint32_t>(frame_data_, 54, 57);
}

void HkOdd::set_lv_temp_() {
    lv_temp = decode_byte<uint16_t>(frame_data_, 58, 59);
}

void HkOdd::set_hv_pwm_() {
    hv_pwm = decode_byte<uint16_t>(frame_data_, 60, 61);
}

void HkOdd::set_hv_status_() {
    hv_status = decode_byte<uint16_t>(frame_data_, 62, 63);
}

void HkOdd::set_hv_current_() {
    for (int i = 0; i < 2; i++) {
        hv_current[i] = decode_byte<uint16_t>(frame_data_, 64 + 2 * i, 65 + 2 * i);
    }
}

void HkOdd::set_fe_status_() {
    for (int i = 0; i < 18; i++) {
        fe_status[i] = decode_byte<uint8_t>(frame_data_ + 68 + 10 * i, 0, 0);
    }
}

void HkOdd::set_fe_temp_() {
    for (int i = 0; i < 18; i++) {
        fe_temp[i] = decode_byte<uint8_t>(frame_data_ + 68 + 10 * i, 1, 1);
    }
}

void HkOdd::set_fe_hv_() {
    for (int i = 0; i < 18; i++) {
        fe_hv[i] = decode_byte<uint16_t>(frame_data_ + 68 + 10 * i, 2, 3);
    }
}

void HkOdd::set_fe_thr_() {
    for (int i = 0; i < 18; i++) {
        fe_thr[i] = decode_byte<uint16_t>(frame_data_ + 68 + 10 * i, 4, 5);
    }
}

void HkOdd::set_fe_rate_() {
    for (int i = 0; i < 18; i++) {
        fe_rate[i] = decode_byte<uint16_t>(frame_data_ + 68 + 10 * i, 6, 7);
    }
}

void HkOdd::set_fe_cosmic_() {
    for (int i = 0; i < 18; i++) {
        fe_cosmic[i] = decode_byte<uint16_t>(frame_data_ + 68 + 10 * i, 8, 9);
    }
}

void HkOdd::update(int32_t cur_is_bad) {
    is_bad = cur_is_bad;
    set_frm_index_();
    set_pkt_tag_();
    set_packet_num_();
    set_timestamp_();
    set_obox_mode_();
    set_cpu_status_();
    set_trig_status_();
    set_comm_status_();
    set_ct_temp_();
    set_chain_temp_();
    set_reserved_();
    set_lv_status_();
    set_fe_pattern_();
    set_lv_temp_();
    set_hv_pwm_();
    set_hv_status_();
    set_hv_current_();
    set_fe_status_();
    set_fe_temp_();
    set_fe_hv_();
    set_fe_thr_();
    set_fe_rate_();
    set_fe_cosmic_();
}
