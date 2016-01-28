#ifndef SCIDATAFILE_H
#define SCIDATAFILE_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <iterator>
#include "RootInc.hpp"
#include "Constants.hpp"
#include "SciEvent.hpp"
#include "SciTrigger.hpp"

using namespace std;

class SciDataFile {
public:
    struct Modules_T {
        Long64_t     trigg_num;                // Sequential number of the trigger packet of an event.
        //                                        Start from 0, -1 if it has no corresponding trigger packet.
        //                                        Pedestal and no pedestal packets use different number counter.
        Long64_t     event_num;                // Sequential number of the event packet of a module. Start from 0.
        //                                        Pedestal and no pedestal packets use different number counter.
        Long64_t     event_num_g;              // Order number at the sequence of appearing in the raw data file. Start from 0.
        //                                        Pedestal and no pedestal packets use the same number counter.
        Int_t        is_bad;                   // if the packet is invalid or has CRC error: 1 when invalid, 2 when crc error, 0 when good
        Int_t        pre_is_bad;               // if the previous packet is invalid or has CRC error
        Int_t        compress;                 // compress mode: 0 for default, 1 for simple, 2 for pedestal, 3 for full reduction 
        Int_t        ct_num;                   // CT number, from 1 to 25
        UInt_t       time_stamp;               // raw data of TIMESTAMP field of the packet
        UInt_t       time_period;              // overflow counter of time_stamp
        UInt_t       time_wait;                // time_stamp difference since previous event
        UInt_t       time_align;               // 23 LSB of time_stamp
        Int_t        raw_rate;                 // raw data of RATE field of the packet
        Int_t        raw_dead;                 // raw data of DEADTIME field of the packet
        Float_t      dead_ratio;               // increased number of raw_dead divided by time_wait
        UShort_t     status;                   // raw data of the 16 bits STATUS field of the packet
        Bool_t       trigger_bit[64];          // raw data of the TRIGGERBIT, the same as pattern[64]
        Float_t      energy_adc[64];           // ADC of energy of the 64 channels, the same as pm[64]
        Float_t      common_noise;             // raw data of COMMON NOISE field for compress mode 3, 0 for other compress mode
    };

    struct Trigger_T {
        Long64_t     trigg_num;                // Sequential number of the trigger packet. Start from 0.
        //                                        Pedestal and no pedestal packets use different number counter.
        Long64_t     trigg_num_g;              // Order number at the sequence of appearing in the raw data file. Start from 0.
        //                                        Pedestal and no pedestal packets use the same number counter.
        Int_t        is_bad;                   // if the packet is invalid or has CRC error: 1 when invalid, 2 when crc error, 0 when good
        Int_t        pre_is_bad;               // if the previous packet is invalid or has CRC error
        Int_t        type;                     // the 4 types of trigger packet: 0x00F0 for pedestal, 0x00FF nor normal, 0xF000 for single, 0xFF00 for cosmic
        Int_t        packet_num;               // raw data of packet number of the trigger packet
        UInt_t       time_stamp;               // raw data of Timestamp register of the trigger packet
        UInt_t       time_period;              // overflow counter of time_stamp
        UInt_t       time_wait;                // time_stamp difference since previous event
        UInt_t       time_align;               // 23 MSB of time_stamp
        ULong64_t    frm_ship_time;            // raw data of the ship time from frame in which this packet is.
        ULong64_t    frm_gps_time;             // raw data of the GPS time from frame in which this packet is.
        //                                        Other forms of frm_ship_time and frm_gps_time will be converted when doing UTC calculating.
        Long64_t     pkt_start;                // first entry index of all the adjacent event packets of this event in the modules tree 
        Int_t        pkt_count;                // number of entries of event packets for this event in the modules tree
        Int_t        lost_count;               // number of lost event packets for this event
        Int_t        trigger_n;                // sum of the trigger_bit[64] of all the event packets for this event
        UShort_t     status;                   // raw data of Status register of the trigger packet
        UChar_t      trig_sig_con[25];         // raw data of Trigger signals conditions for each frontend
        Bool_t       trig_accepted[25];        // raw data of FEE TRIGGER ACCEPTED for each frontend
        Bool_t       trig_rejected[25];        // raw data of FEE TRIGGER REJECTED for each frontend
        UInt_t       raw_dead;                 // raw data of the dead time counter field
        Float_t      dead_ratio;               // increased number of raw_dead divided by time_wait
    };

public:
    Modules_T t_modules;
    Trigger_T t_trigger;
    Modules_T t_ped_modules;
    Trigger_T t_ped_trigger;
    
private:
    TFile*    t_out_file_;
    TTree*    t_modules_tree_;
    TTree*    t_trigger_tree_;
    TTree*    t_ped_modules_tree_;
    TTree*    t_ped_trigger_tree_;
    
    Long64_t  t_modules_cur_entry_;
    Long64_t  t_trigger_cur_entry_;
    Long64_t  t_ped_modules_cur_entry_;
    Long64_t  t_ped_trigger_cur_entry_;
    Long64_t  ped_module_last_entry_[25];
    Long64_t  t_modules_cur_start_;
    Int_t     t_modules_cur_count_;
    UInt_t    t_module_pre_time_stamp_[25];
    UInt_t    t_trigger_pre_time_stamp_;
    UInt_t    t_ped_module_pre_time_stamp_[25];
    UInt_t    t_ped_trigger_pre_time_stamp_;
    Long64_t  cur_trigg_num_;
    Long64_t  cur_event_num_[25];
    Long64_t  cur_ped_trigg_num_;
    Long64_t  cur_ped_event_num_[25];
private:
    void copy_event_pkt_(Modules_T& t_modules_par, const SciEvent& event);
    void copy_trigger_pkt_(Trigger_T& t_trigger_par, const SciTrigger& trigger);

public:
    SciDataFile();
    ~SciDataFile();
    
    bool open(const char* filename);
    void close();
    void write_module_alone(const SciEvent& event, Int_t bad_status);
    void write_trigger_alone(const SciTrigger& trigger, Int_t bad_status);
    void write_event_align(const SciTrigger& trigger, const vector<SciEvent>& events_vec);
    void write_ped_module_alone(const SciEvent& ped_event, Int_t ped_bad_status);
    void write_ped_trigger_alone(const SciTrigger& ped_trigger, Int_t ped_bad_statsu);
    void write_ped_event_align(const SciTrigger& ped_trigger, const vector<SciEvent>& ped_events_vec);

};

#endif
