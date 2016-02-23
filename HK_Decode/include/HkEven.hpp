#ifndef HKEVEN_H
#define HKEVEN_H

#include <iostream>
#include <cstddef>
#include <stdint.h>
#include "Decoder.hpp"

class HkEven: private Decoder {
private:
    const char* frame_data_;

public:
    // each field of Even Packet is here
    int16_t   frm_index;
    int16_t   pkt_tag;
    uint16_t  fe_temp[7];
    uint16_t  fe_hv[7];
    uint16_t  fe_thr[7];
    uint16_t  fe_rate[7];
    uint16_t  fe_cosmic[7];
    uint16_t  flex_i_p3v3[5];
    uint16_t  flex_i_p1v7[5];
    uint16_t  flex_i_n2v5[5];
    uint16_t  flex_v_p3v3[5];
    uint16_t  flex_v_p1v7[5];
    uint16_t  flex_v_n2v5[5];
    uint16_t  hv_v_hot;
    uint16_t  hv_i_hot;
    uint16_t  ct_v_hot[2];
    uint16_t  ct_i_hot[2];
    uint16_t  hv_v_cold;
    uint16_t  hv_i_cold;
    uint16_t  ct_v_cold[2];
    uint16_t  ct_i_cold[2];
    uint32_t  timestamp_sync;
    uint16_t  command_rec;
    uint16_t  command_exec;
    uint16_t  command_last_num;
    uint16_t  command_last_stamp;
    uint16_t  command_last_exec;
    uint16_t  command_last_arg[2];
    uint16_t  obox_hk_crc;
    uint16_t  saa;
    uint16_t  sci_head;
    uint64_t  gps_pps_count;
    uint64_t  gps_sync_gen_count;
    uint64_t  gps_sync_send_count;
    uint16_t  hk_head;
    uint16_t  hk_tail;

public:
    int32_t is_bad;
    
private:
    void set_frm_index_();
    void set_pkt_tag_();
    void set_fe_temp_();
    void set_fe_hv_();
    void set_fe_thr_();
    void set_fe_rate_();
    void set_fe_cosmic_();
    void set_flex_i_p3v3_();
    void set_flex_i_p1v7_();
    void set_flex_i_n2v5_();
    void set_flex_v_p3v3_();
    void set_flex_v_p1v7_();
    void set_flex_v_n2v5_();
    void set_hv_v_hot_();
    void set_hv_i_hot_();
    void set_ct_v_hot_();
    void set_ct_i_hot_();
    void set_hv_v_cold_();
    void set_hv_i_cold_();
    void set_ct_v_cold_();
    void set_ct_i_cold_();
    void set_timestamp_sync_();
    void set_command_rec_();
    void set_command_exec_();
    void set_command_last_num_();
    void set_command_last_stamp_();
    void set_command_last_exec_();
    void set_command_last_arg_();
    void set_obox_hk_crc_();
    void set_saa_();
    void set_sci_head_();
    void set_gps_pps_count_();
    void set_gps_sync_gen_count_();
    void set_gps_sync_send_count_();
    void set_hk_head_();
    void set_hk_tail_();
    
public:
    void update(int32_t cur_is_bad);
    
public:
    HkEven();
    HkEven(const char*);
    ~HkEven();
    void setdata(const char*);
    
};

#endif
