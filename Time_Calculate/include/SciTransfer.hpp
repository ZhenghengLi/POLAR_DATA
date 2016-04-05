#ifndef SCITRANSFER_H
#define SCITRANSFER_H

#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include "RootInc.hpp"

using namespace std;

class SciTransfer {
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
        Int_t           abs_gps_week;             // week of absolute gps time of this event.
        Double_t        abs_gps_second;           // second of absolute gps time of this event.
        Bool_t          abs_gps_valid;            // if the absolute gps time is valid.
    };

public:
    Modules_T t_modules;
    Trigger_T t_trigger;
    Modules_T t_ped_modules;
    Trigger_T t_ped_trigger;
    
private:
    TFile*   t_file_in_;
    TTree*   t_modules_tree_in_;
    TTree*   t_trigger_tree_in_;
    TTree*   t_ped_modules_tree_in_;
    TTree*   t_ped_trigger_tree_in_;

    Long64_t t_modules_tot_entries_;
    Long64_t t_modules_cur_index_;
    bool     t_modules_reach_end_;
    Long64_t t_trigger_tot_entries_;
    Long64_t t_trigger_cur_index_;
    bool     t_trigger_reach_end_;
    Long64_t t_ped_modules_tot_entries_;
    Long64_t t_ped_modules_cur_index_;
    bool     t_ped_modules_reach_end_;
    Long64_t t_ped_trigger_tot_entries_;
    Long64_t t_ped_trigger_cur_index_;
    bool     t_ped_trigger_reach_end_;

    TNamed*  m_dattype_named_in_;
    TNamed*  m_version_named_in_;
    TNamed*  m_gentime_named_in_;
    TNamed*  m_rawfile_named_in_;
    TNamed*  m_dcdinfo_named_in_;
    
    TFile*   t_file_out_;
    TTree*   t_modules_tree_out_;
    TTree*   t_trigger_tree_out_;
    TTree*   t_ped_modules_tree_out_;
    TTree*   t_ped_trigger_tree_out_;

public:
    SciTransfer();
    ~SciTransfer();

    bool open_read(const char* filename);
    bool open_write(const char* filename);
    void close_read();
    void close_write();

    void read_set_start();
    bool modules_next();
    bool trigger_next();
    bool ped_modules_next();
    bool ped_trigger_next();

    void modules_fill();
    void trigger_fill();
    void ped_modules_fill();
    void ped_trigger_fill();
    void write_all_tree();
    void write_meta(const char* key, const char* value);

    const TNamed* get_dattype() const {
        return m_dattype_named_in_;
    }

    const TNamed* get_version() const {
        return m_version_named_in_;
    }

    const TNamed* get_gentime() const {
        return m_gentime_named_in_;
    }

    const TNamed* get_rawfile() const {
        return m_rawfile_named_in_;
    }

    const TNamed* get_dcdinfo() const {
        return m_dcdinfo_named_in_;
    }
};

#endif
