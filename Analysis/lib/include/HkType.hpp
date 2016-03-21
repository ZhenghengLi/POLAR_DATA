#ifndef HKTYPE_H
#define HKTYPE_H

#include "RootInc.hpp"

class HkType {
public:
    struct Hk_Obox_T {
        Int_t          odd_index;                     // Frame Index of odd packet, -1 when lost
        Int_t          even_index;                    // Frame Index of even packet, -1 when lost
        Int_t          odd_is_bad;                    // 3 when lost, 2 when invalid, 1 when crc error, 0 when good
        Int_t          even_is_bad;                   // 3 when lost, 2 when invalid, 1 when crc error, 0 when good
        Int_t          obox_is_bad;                   // 3 when half, 2 when invalid, 1 when crc error, 0 when good
        UShort_t       packet_num;                    // raw data of OBOX packet number
        UInt_t         timestamp;                     // raw data of OBOX Timestamp
        UChar_t        obox_mode;                     // raw data of OBOX operational mode
        UShort_t       cpu_status;                    // raw data of OBOX CT CPU status
        UChar_t        trig_status;                   // raw data of OBOX CT Trigger status
        UChar_t        comm_status;                   // raw data of OBOX CT Communication status
        Float_t        ct_temp;                       // physical value of Central Trigger temperature
        Float_t        chain_temp;                    // physical value of Sensor chain temperature
        UShort_t       reserved;                      // raw data of Reserved
        UShort_t       lv_status;                     // raw data of LV power supply status
        UInt_t         fe_pattern;                    // raw data of FEs powered
        Float_t        lv_temp;                       // physical value of LV power supply temperature
        UShort_t       hv_pwm;                        // raw data of HV PWM setting
        UShort_t       hv_status;                     // raw data of HV power supply status
        UShort_t       hv_current[2];                 // raw data of HV current readout1 and readout2
        UChar_t        fe_status[25];                 // raw data of module status
        Float_t        fe_temp[25];                   // physical value of module temperature
        Float_t        fe_hv[25];                     // physical value of HV voltage setting
        Float_t        fe_thr[25];                    // physical value of threshold setting
        UShort_t       fe_rate[25];                   // raw data of count rate
        UShort_t       fe_cosmic[25];                 // raw data of too many / too high rate 
        Float_t        flex_i_p3v3[5];                // physical value of Current at P3V3 rail
        Float_t        flex_i_p1v7[5];                // physical value of Current at P1V7 rail
        Float_t        flex_i_n2v5[5];                // physical value of Current at N2V5 rail
        Float_t        flex_v_p3v3[5];                // physical value of Voltage at P3V3 rail
        Float_t        flex_v_p1v7[5];                // physical value of Voltage at P1V7 rail
        Float_t        flex_v_n2v5[5];                // physical value of Voltage at N2V5 rail
        Float_t        hv_v_hot;                      // physical value of Voltage at HV Hot P3V3 rail
        Float_t        hv_i_hot;                      // physical value of Current at HV Hot P3V3 rail
        Float_t        ct_v_hot[2];                   // physical value of Voltage at CT Hot P3V3 and 1V5 rail
        Float_t        ct_i_hot[2];                   // physical value of Current at CT Hot P3V3 and 1V5 rail
        Float_t        hv_v_cold;                     // physical value of Voltage at HV Cold P3V3 rail
        Float_t        hv_i_cold;                     // physical value of Current at HV Cold P3V3 rail
        Float_t        ct_v_cold[2];                  // physical value of Voltage at CT Cold P3V3 and 1V5 rail
        Float_t        ct_i_cold[2];                  // physical value of Current at CT Cold P3V3 and 1V5 rail
        UInt_t         timestamp_sync;                // raw data of Timestamp at last sync
        UShort_t       command_rec;                   // raw data of Command received counter
        UShort_t       command_exec;                  // raw data of Command executed counter
        UShort_t       command_last_num;              // raw data of Command last executed number
        UShort_t       command_last_stamp;            // raw data of Command last executed Timestamp
        UShort_t       command_last_exec;             // raw data of Command last executed code
        UShort_t       command_last_arg[2];           // raw data of Command last executed argument 1 and argument 2
        UShort_t       obox_hk_crc;                   // raw data of OBOX HK packet CRC
        UShort_t       saa;                           // raw data of SAA flag
        UShort_t       sci_head;                      // raw data of OBOX science packet header counter
        ULong64_t      gps_pps_count;                 // raw data of Time_PPS
        ULong64_t      gps_sync_gen_count;            // raw data of Time_synchComGen
        ULong64_t      gps_sync_send_count;           // raw data of Time_synchComTx
    };

    struct Hk_Ibox_T {
        Int_t          frm_index;                     // raw data of Frame Index
        Int_t          pkt_tag;                       // raw data of Packet tag
        Int_t          is_bad;                        // 2 when invalid, 1 when crc error, 0 when good
        ULong64_t      ship_time;                     // raw data of Ship time
        UShort_t       error[2];                      // raw data of Command feedback error number1 and number2
        UShort_t       frame_head;                    // raw data of Frame header
        UShort_t       command_head;                  // raw data of Command frame header
        UShort_t       command_num;                   // raw data of Command number
        UShort_t       command_code;                  // raw data of Command code
        UShort_t       command_arg[2];                // raw data of Command argument1 and argument2
        UShort_t       head;                          // raw data of OBOX HK header counter
        UShort_t       tail;                          // raw data of OBOX HK tail counter
        ULong64_t      ibox_gps;                      // raw data of IBOX GPS time
    };

public:
    void bind_hk_obox_tree(TTree* t_hk_obox_tree, Hk_Obox_T& t_hk_obox);
    void bind_hk_ibox_tree(TTree* t_hk_ibox_tree, Hk_Ibox_T& t_hk_ibox);
};

#endif
