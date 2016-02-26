#ifndef HKDATAFILE_H
#define HKDATAFILE_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <stdint.h>
#include "RootInc.hpp"
#include "HkOdd.hpp"
#include "HkEven.hpp"
#include "HkFrame.hpp"

using namespace std;

class HkDataFile {
public:
    struct Hk_Obox_T {
        Int_t          odd_index;                     // -1 when lost
        Int_t          even_index;                    // -1 when lost
        Int_t          odd_is_bad;                    // 3 when lost, 2 when invalid, 1 when crc error, 0 when good
        Int_t          even_is_bad;                   // 3 when lost, 2 when invalid, 1 when crc error, 0 when good
        Int_t          obox_is_bad;                   // 3 when half, 2 when invalid, 1 when crc error, 0 when good
        UShort_t       packet_num;                    // raw data of obox packet number
        UInt_t         timestamp;                     // raw data of obox timestamp
        UShort_t       obox_mode;                     // raw data of obox operational mode
        UShort_t       cpu_status;                    // raw data of ct_cpu status
        UShort_t       trig_status;                   // raw data of ct_trig status
        UShort_t       comm_status;                   // raw data of ct_com status
        UShort_t       ct_temp;                       // raw data of ct temperature
        UShort_t       chain_temp;                    // raw data of sensor chain temperature
        UShort_t       reserved;                      // raw data of reserved information in obox hk packet
        UShort_t       lv_status;                     // raw data of lv power supply status
        UShort_t       fe_pattern;                    // raw data of fes and flexes powered
        UShort_t       hv_pwm;                        // raw data of hv pwm setting
        UShort_t       hv_status;                     // raw data of hvps status
        UShort_t       hv_current[2];                 // raw data of hv current readout1 and readout2
        UShort_t       fe_status[25];                 // raw data of ?
        UShort_t       fe_temp[25];                   // raw data of ?
        UShort_t       fe_hv[25];                     // raw data of ?
        UShort_t       fe_thr[25];                    // raw data of ?
        UShort_t       fe_rate[25];                   // raw data of ?
        UShort_t       fe_cosmic[25];                 // raw data of ?
        UShort_t       flex_i_p3v3[5];                // raw data of flex1-5 +3.3v current
        UShort_t       flex_i_p1v7[5];                // raw data of flex1-5 +1.7v current
        UShort_t       flex_i_n2v5[5];                // raw data of flex1-5 -2.5v current
        UShort_t       flex_v_p3v3[5];                // raw data of flex1-5 +3.3v voltage
        UShort_t       flex_v_p1v7[5];                // raw data of flex1-5 +1.7v voltage
        UShort_t       flex_v_n2v5[5];                // raw data of flex1-5 -2.5v voltage
        UShort_t       hv_v_hot;                      // raw data of ?
        UShort_t       hv_i_hot;                      // raw data of ?
        UShort_t       ct_v_hot[2];                   // raw data of ?
        UShort_t       ct_i_hot[2];                   // raw data of ?
        UShort_t       hv_v_cold;                     // raw data of ?
        UShort_t       hv_i_cold;                     // raw data of ?
        UShort_t       ct_v_cold[2];                  // raw data of ?
        UShort_t       ct_i_cold[2];                  // raw data of ?
        UInt_t         timestamp_sync;                // raw data of last timestamp at sync
        UShort_t       command_rec;                   // raw data of obox received commands cnt
        UShort_t       command_exec;                  // raw data of obox executed commands cnt
        UShort_t       command_last_num;              // raw data of cmd feedback: executed number
        UShort_t       command_last_stamp;            // raw data of cmd feedback: executed timestamp
        UShort_t       command_last_exec;             // raw data of cmd feedback: executed code
        UShort_t       command_last_arg[2];           // raw data of cmd feedback: executed arg1 and arg2
        UShort_t       obox_hk_crc;                   // raw data of obox hk packet crc followd
        UShort_t       saa;                           // raw data of saa flag
        UShort_t       sci_head;                      // raw data of obox science packet header counter
        ULong64_t      gps_pps_count;                 // raw data of Time_PPS
        ULong64_t      gps_sync_gen_count;            // raw data of Time_synchComGen
        ULong64_t      gps_sync_send_count;           // raw data of Time_synchComTx
        UShort_t       hk_head;                       // raw data of obox hk header counter
        UShort_t       hk_tail;                       // raw data of obox hk tail counter
        
    };

    struct Hk_Ibox_T {
        Int_t          frm_index;                     // 
        Int_t          pkt_tag;                       //
        Int_t          is_bad;                        // 2 when invalid, 1 when crc error, 0 when good
        ULong64_t      ship_time;                     // raw data of ship time
        UShort_t       error[2];                      // raw data of command feedback error number1 and number2
        UShort_t       frame_head;                    // raw data of frame header
        UShort_t       command_head;                  // raw data of command frame header
        UShort_t       command_num;                   // raw data of command number
        UShort_t       command_code;                  // raw data of comand code
        UShort_t       command_arg[2];                // raw data of command argument1 and argument2
        UShort_t       head;                          // raw data of obox hk header counter
        UShort_t       tail;                          // raw data of obox hk tail counter
        ULong64_t      ibox_gps;                      // raw data of ibox gps time
    };
    
public:
    Hk_Obox_T t_hk_obox;
    Hk_Ibox_T t_hk_ibox;

private:
    TFile* t_out_file_;
    TTree* t_hk_obox_tree_;
    TTree* t_hk_ibox_tree_;

private:
    void copy_odd_packet_(const HkOdd& odd_pkt);
    void copy_even_packet_(const HkEven& even_pkt);
    void copy_ibox_info_(const HkFrame& frame);
    
public:
    HkDataFile();
    ~HkDataFile();

    bool open(const char* filename);
    void close();

    void write_after_decoding();
    void write_ibox_info(const HkFrame& frame);
    void write_two_packet(const HkOdd& odd_pkt, const HkEven even_pkt, int cur_obox_is_bad);
    void write_odd_packet_alone(const HkOdd& odd_pkt);
    void write_even_packet_alone(const HkEven& even_pkt);
    void write_meta(const char* key, const char* value);
    
};

#endif
