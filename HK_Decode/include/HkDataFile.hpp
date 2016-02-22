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
    void clear_obox_branch_data_();
    
public:
    HkDataFile();
    ~HkDataFile();

    bool open(const char* filename);
    void close();

    void write_after_decoding();
    void write_ibox_info(const HkFrame& frame);
    void write_two_packet(const HkOdd& odd_pkt, const HkEven even_pkt);
    void write_odd_packet_alone(const HkOdd& odd_pkt);
    void write_even_packet_alone(const HkEven& even_pkt);
    void write_meta(const char* key, const char* value);
    
};

#endif
