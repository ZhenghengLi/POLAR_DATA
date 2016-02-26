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

void HkEven::set_frm_index_() {
    frm_index = decode_bit<int16_t>(frame_data_ + 2, 2, 15);
}

void HkEven::set_pkt_tag_() {
    pkt_tag = decode_byte<int16_t>(frame_data_, 10, 11);
}

void HkEven::set_fe_status_() {

}

void HkEven::set_fe_temp_() {

}

void HkEven::set_fe_hv_() {

}

void HkEven::set_fe_thr_() {

}

void HkEven::set_fe_rate_() {

}

void HkEven::set_fe_cosmic_() {

}

void HkEven::set_flex_i_p3v3_() {

}

void HkEven::set_flex_i_p1v7_() {

}

void HkEven::set_flex_i_n2v5_() {

}

void HkEven::set_flex_v_p3v3_() {

}

void HkEven::set_flex_v_p1v7_() {

}

void HkEven::set_flex_v_n2v5_() {

}

void HkEven::set_hv_v_hot_() {

}

void HkEven::set_hv_i_hot_() {

}

void HkEven::set_ct_v_hot_() {

}

void HkEven::set_ct_i_hot_() {

}

void HkEven::set_hv_v_cold_() {

}

void HkEven::set_hv_i_cold_() {

}

void HkEven::set_ct_v_cold_() {

}

void HkEven::set_ct_i_cold_() {

}

void HkEven::set_timestamp_sync_() {

}

void HkEven::set_command_rec_() {

}

void HkEven::set_command_exec_() {

}

void HkEven::set_command_last_num_() {

}

void HkEven::set_command_last_stamp_() {

}

void HkEven::set_command_last_exec_() {

}

void HkEven::set_command_last_arg_() {

}

void HkEven::set_obox_hk_crc_() {

}

void HkEven::set_saa_() {

}

void HkEven::set_sci_head_() {

}

void HkEven::set_gps_pps_count_() {

}

void HkEven::set_gps_sync_gen_count_() {

}

void HkEven::set_gps_sync_send_count_() {

}

void HkEven::set_hk_head_() {

}

void HkEven::set_hk_tail_() {

}

void HkEven::update(int32_t cur_is_bad) {
    is_bad = cur_is_bad;
    set_frm_index_();
    set_pkt_tag_();
    set_fe_status_();
    set_fe_temp_();
    set_fe_hv_();
    set_fe_thr_();
    set_fe_rate_();
    set_fe_cosmic_();
    set_flex_i_p3v3_();
    set_flex_i_p1v7_();
    set_flex_i_n2v5_();
    set_flex_v_p3v3_();
    set_flex_v_p1v7_();
    set_flex_v_n2v5_();
    set_hv_v_hot_();
    set_hv_i_hot_();
    set_ct_v_hot_();
    set_ct_i_hot_();
    set_hv_v_cold_();
    set_hv_i_cold_();
    set_ct_v_cold_();
    set_ct_i_cold_();
    set_timestamp_sync_();
    set_command_rec_();
    set_command_exec_();
    set_command_last_num_();
    set_command_last_stamp_();
    set_command_last_exec_();
    set_command_last_arg_();
    set_obox_hk_crc_();
    set_saa_();
    set_sci_head_();
    set_gps_pps_count_();
    set_gps_sync_gen_count_();
    set_gps_sync_send_count_();
    set_hk_head_();
    set_hk_tail_();
}

