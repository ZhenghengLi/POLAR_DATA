#ifndef HKODD_H
#define HKODD_H

#include <iostream>
#include <cstddef>
#include <stdint.h>
#include "Decoder.hpp"

class HkOdd: private Decoder {
private:
    const char* frame_data_;

public:
    // each field of Odd Packet is here
    int16_t   frm_index;
    int16_t   pkt_tag;
    uint16_t  packet_num;
    uint32_t  timestamp;
    uint16_t  obox_mode;
    uint16_t  cpu_status;
    uint16_t  trig_status;
    uint16_t  comm_status;
    uint16_t  ct_temp;
    uint16_t  chain_temp;
    uint16_t  reserved;
    uint16_t  lv_status;
    uint32_t  fe_pattern;
    uint16_t  hv_pwm;
    uint16_t  hv_status;
    uint16_t  hv_current[2];
    uint16_t  fe_temp[18];
    uint16_t  fe_hv[18];
    uint16_t  fe_thr[18];
    uint16_t  fe_rate[18];
    uint16_t  fe_cosmic[18];

public:
    int32_t is_bad;
    
private:
    void set_frm_index_();
    void set_pkt_tag_();
    void set_packet_num_();
    void set_timestamp_();
    void set_obox_mode_();
    void set_cpu_status_();
    void set_trig_status_();
    void set_comm_status_();
    void set_ct_temp_();
    void set_chain_temp_();
    void set_reserved_();
    void set_lv_status_();
    void set_fe_pattern_();
    void set_hv_pwm_();
    void set_hv_status_();
    void set_hv_current_();
    void set_fe_temp_();
    void set_fe_hv_();
    void set_fe_thr_();
    void set_fe_rate_();
    void set_fe_cosmic_();
    
public:
    void update(int32_t cur_is_bad);

public:
    HkOdd();
    HkOdd(const char*);
    ~HkOdd();
    void setdata(const char*);

};

#endif
