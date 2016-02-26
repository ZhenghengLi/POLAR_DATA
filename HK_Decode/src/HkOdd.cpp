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

}

void HkOdd::set_timestamp_() {

}

void HkOdd::set_obox_mode_() {

}

void HkOdd::set_cpu_status_() {

}

void HkOdd::set_trig_status_() {

}

void HkOdd::set_comm_status_() {

}

void HkOdd::set_ct_temp_() {

}

void HkOdd::set_chain_temp_() {

}

void HkOdd::set_reserved_() {

}

void HkOdd::set_lv_status_() {

}

void HkOdd::set_fe_pattern_() {

}

void HkOdd::set_hv_pwm_() {

}

void HkOdd::set_hv_status_() {

}

void HkOdd::set_hv_current_() {

}

void HkOdd::set_fe_status_() {

}

void HkOdd::set_fe_temp_() {

}

void HkOdd::set_fe_hv_() {

}

void HkOdd::set_fe_thr_() {

}

void HkOdd::set_fe_rate_() {

}

void HkOdd::set_fe_cosmic_() {

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
