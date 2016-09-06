#ifndef SCIDATAFILE_H
#define SCIDATAFILE_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <stdint.h>
#include <vector>
#include <iterator>
#include "RootInc.hpp"
#include "Constants.hpp"
#include "SciEvent.hpp"
#include "SciTrigger.hpp"

using namespace std;

class SciDataFile {
public:
    struct Event_Status_T {
        Bool_t trigger_fe_busy;           // 15
        Bool_t fifo_full;                 // 14
        Bool_t fifo_empty;                // 13
        Bool_t trigger_enable;            // 12
        Bool_t trigger_waiting;           // 11
        Bool_t trigger_hold_b;            // 10
        Bool_t timestamp_enable;          //  9
        Bool_t reduction_mode_b1;         //  8
        Bool_t reduction_mode_b0;         //  7
        Bool_t subsystem_busy;            //  6
        Bool_t dynode_2;                  //  5
        Bool_t dynode_1;                  //  4
        Bool_t dy12_too_high;             //  3
        Bool_t t_out_too_many;            //  2
        Bool_t t_out_2;                   //  1
        Bool_t t_out_1;                   //  0
    };

    struct Trigg_Status_T {
        Bool_t science_disable;           // 15
        Bool_t master_clock_enable;       // 14
        Bool_t saving_data;               // 13
        Bool_t taking_event_or_ped;       // 12
        Bool_t fifo_full;                 // 11
        Bool_t fifo_almost_full;          // 10
        Bool_t fifo_empty;                //  9
        Bool_t fifo_almost_empty;         //  8
        Bool_t any_waiting;               //  7
        Bool_t any_waiting_two_hits;      //  6
        Bool_t any_tmany_thigh;           //  5
        Bool_t packet_type_b2;            //  4
        Bool_t packet_type_b1;            //  3
        Bool_t packet_type_b0;            //  2
    };

    struct Trig_Sig_Con_T {
        Bool_t fe_busy[25];               //  5
        Bool_t fe_waiting[25];            //  4
        Bool_t fe_hold_b[25];             //  3
        Bool_t fe_tmany_thigh[25];        //  2
        Bool_t fe_tout_2[25];             //  1
        Bool_t fe_tout_1[25];             //  0
    };
    
    struct Modules_T {
        Long64_t        trigg_num;                // Sequential number of the trigger packet of an event.
        //                                           Start from 0, -1 if it has no corresponding trigger packet. -2 when bad.
        //                                           Pedestal and no pedestal packets use different number counter.
        Long64_t        event_num;                // Sequential number of the event packet of a module. Start from 0. -1 when bad.
        //                                           Pedestal and no pedestal packets use different number counter.
        Long64_t        event_num_g;              // Order number at the sequence of appearing in the raw data file. Start from 0. -1 when bad.
        //                                           Pedestal and no pedestal packets use the same number counter.
        Int_t           is_bad;                   // if the packet is invalid or has CRC error: 3 when short, 2 when invalid, 1 when crc error, 0 when good,
        //                                           -1 when timestamp is 0.
        Int_t           pre_is_bad;               // if the previous packet is invalid or has CRC error
        Int_t           compress;                 // compress mode: 0 for default, 1 for simple, 2 for pedestal, 3 for full reduction 
        Int_t           ct_num;                   // CT number, from 1 to 25
        UInt_t          time_stamp;               // raw data of TIMESTAMP field of the packet
        UInt_t          time_period;              // overflow counter of time_stamp
        UInt_t          time_align;               // 23 LSB of time_stamp
        Double_t        time_second;              // time in seconds from start
        Double_t        time_wait;                // time_second difference since previous event
        Int_t           raw_rate;                 // raw data of RATE field of the packet
        UInt_t          raw_dead;                 // raw data of DEADTIME field of the packet
        Float_t         dead_ratio;               // increased number of raw_dead divided by time_wait
        UShort_t        status;                   // raw data of the 16 bits STATUS field of the packet
        Event_Status_T  status_bit;               // each bit in status
        Bool_t          trigger_bit[64];          // raw data of the TRIGGERBIT, the same as pattern[64]
        Float_t         energy_adc[64];           // ADC of energy of the 64 channels, the same as pm[64]
        Float_t         common_noise;             // raw data of COMMON NOISE field for compress mode 3, 0 for other compress mode
        Int_t           multiplicity;             // sum of trigger_bit[64] of this packet
    };

    struct Trigger_T {
        Long64_t        trigg_num;                // Sequential number of the trigger packet. Start from 0. -1 when bad.
        //                                           Pedestal and no pedestal packets use different number counter.
        Long64_t        trigg_num_g;              // Order number at the sequence of appearing in the raw data file. Start from 0.
        //                                           Pedestal and no pedestal packets use the same number counter.
        Int_t           is_bad;                   // if the packet is invalid or has CRC error: 3 when short, 2 when invalid, 1 when crc error, 0 when good,
        //                                           -1 when timestamp is 0.
        Int_t           pre_is_bad;               // if the previous packet is invalid or has CRC error
        Int_t           type;                     // the 4 types of trigger packet: 0x00F0 for pedestal, 0x00FF nor normal, 0xF000 for single, 0xFF00 for cosmic
        Int_t           packet_num;               // raw data of packet number of the trigger packet
        UInt_t          time_stamp;               // raw data of Timestamp register of the trigger packet
        UInt_t          time_period;              // overflow counter of time_stamp
        UInt_t          time_align;               // 23 MSB of time_stamp
        Double_t        time_second;              // time in seconds from start
        Double_t        time_wait;                // time_second difference since previous event
        ULong64_t       frm_ship_time;            // raw data of the ship time from frame in which this packet is.
        ULong64_t       frm_gps_time;             // raw data of the GPS time from frame in which this packet is.
        //                                           Other forms of frm_ship_time and frm_gps_time will be converted when doing UTC calculating.
        Long64_t        pkt_start;                // first entry index of all the adjacent event packets of this event in the modules tree.
        //                                           -1 when lost all event packets, -2 when bad.
        Int_t           pkt_count;                // number of entries of event packets for this event in the modules tree
        Int_t           lost_count;               // number of lost event packets for this event
        Int_t           trigger_n;                // sum of the trigger_bit[64] of all the event packets for this event
        UShort_t        status;                   // raw data of Status register of the trigger packet
        Trigg_Status_T  status_bit;               // each bit in status
        UChar_t         trig_sig_con[25];         // raw data of Trigger signals conditions for each frontend
        Trig_Sig_Con_T  trig_sig_con_bit;         // each bit in trig_sig_con[25] for each frontend
        Bool_t          trig_accepted[25];        // raw data of FEE TRIGGER ACCEPTED for each frontend
        Bool_t          trig_rejected[25];        // raw data of FEE TRIGGER REJECTED for each frontend
        UInt_t          raw_dead;                 // raw data of the dead time counter field
        Float_t         dead_ratio;               // increased number of raw_dead divided by time_wait
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

    Long64_t  cur_trigg_num_;
    Long64_t  cur_event_num_[25];
    Long64_t  cur_ped_trigg_num_;
    Long64_t  cur_ped_event_num_[25];

    bool     phy_first_valid_found_;
    Long64_t phy_first_valid_index_;
    Long64_t phy_first_valid_week_;
    Long64_t phy_first_valid_second_;
    Long64_t phy_first_ship_second_;
    Long64_t phy_last_valid_index_;
    Long64_t phy_last_valid_week_;
    Long64_t phy_last_valid_second_;
    Long64_t phy_last_ship_second_;
    Long64_t phy_total_valid_count_;
    string   phy_gps_result_str_;
    string   phy_ship_result_str_;
    bool     ped_first_valid_found_;
    Long64_t ped_first_valid_index_;
    Long64_t ped_first_valid_week_;
    Long64_t ped_first_valid_second_;
    Long64_t ped_first_ship_second_;
    Long64_t ped_last_valid_index_;
    Long64_t ped_last_valid_week_;
    Long64_t ped_last_valid_second_;
    Long64_t ped_last_ship_second_;
    Long64_t ped_total_valid_count_;
    string   ped_gps_result_str_;
    string   ped_ship_result_str_;
    
private:
    void copy_event_pkt_(Modules_T& t_modules_par, const SciEvent& event);
    void copy_trigger_pkt_(Trigger_T& t_trigger_par, const SciTrigger& trigger);
    template <class T> bool bit_extract_(T data, size_t n);

    int    week_of_gps6_(const uint64_t raw_gps);
    double second_of_gps6_(const uint64_t raw_gps);
    double calc_ship_second(const uint64_t raw_ship_time);

public:
    SciDataFile();
    ~SciDataFile();
    
    bool open(const char* filename);
    void close();
    void write_after_decoding();
    void write_modules_alone(const SciEvent& event);
    void write_trigger_alone(const SciTrigger& trigger);
    void write_event_align(const SciTrigger& trigger, const vector<SciEvent>& events_vec);
    void write_ped_modules_alone(const SciEvent& ped_event);
    void write_ped_trigger_alone(const SciTrigger& ped_trigger);
    void write_ped_event_align(const SciTrigger& ped_trigger, const vector<SciEvent>& ped_events_vec);
    void write_meta(const char* key, const char* value);
    void gen_gps_result_str();
    void write_gps_span();
    void print_gps_span();

};

#endif
