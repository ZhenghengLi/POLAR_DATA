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
    frm_index = decode_bit<uint16_t>(frame_data_ + 2, 2, 15);
}

void HkEven::set_pkt_tag_() {
    pkt_tag = decode_byte<uint16_t>(frame_data_, 10, 11);
}

void HkEven::set_fe_status_() {
    for (int i = 0; i < 7; i++) {
        fe_status[i] = decode_byte<uint8_t>(frame_data_ + 34 + 10 * i, 0, 0);
    }
}

void HkEven::set_fe_temp_() {
    for (int i = 0; i < 7; i++) {
        fe_temp[i] = decode_byte<uint8_t>(frame_data_ + 34 + 10 * i, 1, 1);
    }
}

void HkEven::set_fe_hv_() {
    for (int i = 0; i < 7; i++) {
        fe_hv[i] = decode_bit<uint16_t>(frame_data_ + 34 + 10 * i + 2, 0, 11);
    }
}

void HkEven::set_fe_thr_() {
    for (int i = 0; i < 7; i++) {
        fe_thr[i] = decode_bit<uint16_t>(frame_data_ + 34 + 10 * i + 4, 4, 15);
    }
}

void HkEven::set_fe_rate_() {
    for (int i = 0; i < 7; i++) {
        fe_rate[i] = decode_byte<uint16_t>(frame_data_ + 34 + 10 * i, 6, 7);
    }
}

void HkEven::set_fe_cosmic_() {
    for (int i = 0; i < 7; i++) {
        fe_cosmic[i] = decode_byte<uint16_t>(frame_data_ + 34 + 10 * i, 8, 9);
    }
}

void HkEven::set_flex_i_p3v3_() {
    for (int i = 0; i < 5; i++) {
        flex_i_p3v3[i] = decode_byte<uint16_t>(frame_data_ + 104 + 12 * i, 0, 1);
    }
}

void HkEven::set_flex_i_p1v7_() {
    for (int i = 0; i < 5; i++) {
        flex_i_p1v7[i] = decode_byte<uint16_t>(frame_data_ + 104 + 12 * i, 2, 3);
    }
}

void HkEven::set_flex_i_n2v5_() {
    for (int i = 0; i < 5; i++) {
        flex_i_n2v5[i] = decode_byte<uint16_t>(frame_data_ + 104 + 12 * i, 4, 5);
    }
}

void HkEven::set_flex_v_p3v3_() {
    for (int i = 0; i < 5; i++) {
        flex_v_p3v3[i] = decode_byte<uint16_t>(frame_data_ + 104 + 12 * i, 6, 7);
    }
}

void HkEven::set_flex_v_p1v7_() {
    for (int i = 0; i < 5; i++) {
        flex_v_p1v7[i] = decode_byte<uint16_t>(frame_data_ + 104 + 12 * i, 8, 9);
    }
}

void HkEven::set_flex_v_n2v5_() {
    for (int i = 0; i < 5; i++) {
        flex_v_n2v5[i] = decode_byte<uint16_t>(frame_data_ + 104 + 12 * i, 10, 11);
    }
}

void HkEven::set_hv_v_hot_() {
    hv_v_hot = decode_byte<uint16_t>(frame_data_ + 164, 0, 1);
}

void HkEven::set_hv_i_hot_() {
    hv_i_hot = decode_byte<uint16_t>(frame_data_ + 164, 2, 3);
}

void HkEven::set_ct_v_hot_() {
    for (int i = 0; i < 2; i++) {
        ct_v_hot[i] = decode_byte<uint16_t>(frame_data_ + 164, 4 + 4 * i, 5 + 4 * i);
    }
}

void HkEven::set_ct_i_hot_() {
    for (int i = 0; i < 2; i++) {
        ct_i_hot[i] = decode_byte<uint16_t>(frame_data_ + 164, 6 + 4 * i, 7 + 4 * i);
    }
}

void HkEven::set_hv_v_cold_() {
    hv_v_cold = decode_byte<uint16_t>(frame_data_ + 164, 12, 13);
}

void HkEven::set_hv_i_cold_() {
    hv_i_cold = decode_byte<uint16_t>(frame_data_ + 164, 14, 15);
}

void HkEven::set_ct_v_cold_() {
    for (int i = 0; i < 2; i++) {
        ct_v_cold[i] = decode_byte<uint16_t>(frame_data_ + 164, 16 + 4 * i, 17 + 4 * i);
    }
}

void HkEven::set_ct_i_cold_() {
    for (int i = 0; i < 2; i++) {
        ct_i_cold[i] = decode_byte<uint16_t>(frame_data_ + 164, 18 + 4 * i, 19 + 4 * i);
    }
}

void HkEven::set_timestamp_sync_() {
    timestamp_sync = decode_byte<uint32_t>(frame_data_, 188, 191);
}

void HkEven::set_command_rec_() {
    command_rec = decode_byte<uint8_t>(frame_data_, 192, 192);
}

void HkEven::set_command_exec_() {
    command_exec = decode_byte<uint8_t>(frame_data_, 193, 193);
}

void HkEven::set_command_last_num_() {
    command_last_num = decode_byte<uint16_t>(frame_data_, 194, 195);
}

void HkEven::set_command_last_stamp_() {
    command_last_stamp = decode_byte<uint16_t>(frame_data_, 196, 197);
}

void HkEven::set_command_last_exec_() {
    command_last_exec = decode_byte<uint16_t>(frame_data_, 198, 199);
}

void HkEven::set_command_last_arg_() {
    for (int i = 0; i < 2; i++) {
        command_last_arg[i] = decode_byte<uint16_t>(frame_data_, 200 + 2 * i, 201 + 2 * i);
    }
}

void HkEven::set_obox_hk_crc_() {
    obox_hk_crc = decode_byte<uint16_t>(frame_data_, 204, 205);
}

void HkEven::set_saa_() {
    saa = decode_bit<uint16_t>(frame_data_ + 220, 0, 1);
}

void HkEven::set_sci_head_() {
    sci_head = decode_byte<uint16_t>(frame_data_, 222, 223);
}

void HkEven::set_gps_pps_count_() {
    gps_pps_count = decode_byte<uint64_t>(frame_data_, 224, 231);
}

void HkEven::set_gps_sync_gen_count_() {
    gps_sync_gen_count = decode_byte<uint64_t>(frame_data_, 232, 239);
}

void HkEven::set_gps_sync_send_count_() {
    gps_sync_send_count = decode_byte<uint64_t>(frame_data_, 240, 247);
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
}

