#ifndef HKDATAFILE_H
#define HKDATAFILE_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <stdint.h>
#include "RootInc.hpp"
#include "HkOdd.hpp"
#include "HkEven.hpp"

using namespace std;

class HkDataFile {
public:
    struct Hk_Packet_T {

    };
    
public:
    Hk_Packet_T t_hk_packet;

private:
    TFile* t_out_file_;
    TTree* t_hk_packet_tree_;

private:
    void copy_odd_packet_(const HkOdd& odd_pkt);
    void copy_even_packet_(const HkEven& even_pkt);
    void clear_branch_data_();
    
public:
    HkDataFile();
    ~HkDataFile();

    bool open(const char* filename);
    void close();

    void write_after_decoding();
    void write_two_packet(const HkOdd& odd_pkt, const HkEven even_pkt);
    void write_odd_packet_alone(const HkOdd& odd_pkt);
    void write_even_packet_alone(const HkEven& even_pkt);
    void write_meta(const char* key, const char* value);
    
};

#endif
