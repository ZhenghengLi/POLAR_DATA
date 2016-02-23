# POLAR_RawData_Decode_Project

This project was started at Sep 19, 2015 after I came to IHEP and proposed to do the decoding of the raw data produced from POLAR detector and all the latter related science analysis. C++ is the main language and the ROOT library is used. The raw data will be transformed and stored to .root files. Monte Carlo simulation using Geant4 will also be included into this project because the data analysis is very dependent on the result of simulation.

This project is now under developing ...

## Data Structure Convention of SCI

### struct Modules_T

Type               | Name                     | Description
-------------------|--------------------------|------------------------
Long64_t           | **trigg_num**            | Sequential number of the trigger packet of an event. Start from 0, -1 if it has no corresponding trigger packet. -2 when bad. Pedestal and no pedestal packets use different number counter.
Long64_t           | **event_num**            | Sequential number of the event packet of a module. Start from 0. -1 when bad. Pedestal and no pedestal packets use different number counter.
Long64_t           | **event_num_g**          | Order number at the sequence of appearing in the raw data file. Start from 0. -1 when bad. Pedestal and no pedestal packets use the same number counter.
Int_t              | **is_bad**               | if the packet is invalid or has CRC error: 3 when short, 2 when invalid, 1 when crc error, 0 when good
Int_t              | **pre_is_bad**           | if the previous packet is invalid or has CRC error
Int_t              | **compress**             | compress mode: 0 for default, 1 for simple, 2 for pedestal, 3 for full reduction 
Int_t              | **ct_num**               | CT number, from 1 to 25
UInt_t             | **time_stamp**           | raw data of TIMESTAMP field of the packet
UInt_t             | **time_period**          | overflow counter of time_stamp
UInt_t             | **time_wait**            | time_stamp difference since previous event
UInt_t             | **time_align**           | 23 LSB of time_stamp
Int_t              | **raw_rate**             | raw data of RATE field of the packet
UInt_t             | **raw_dead**             | raw data of DEADTIME field of the packet
Float_t            | **dead_ratio**           | increased number of raw_dead divided by time_wait
UShort_t           | **status**               | raw data of the 16 bits STATUS field of the packet
Event_Status_T     | **status_bit**           | each bit in status. see [Event_Status_T](#struct-event_status_t)
Bool_t             | **trigger_bit[64]**      | raw data of the TRIGGERBIT, the same as pattern[64]
Float_t            | **energy_adc[64]**       | ADC of energy of the 64 channels, the same as pm[64]
Float_t            | **common_noise**         | raw data of COMMON NOISE field for compress mode 3, 0 for other compress mode


### struct Trigger_T

Type               | Name                     | Description
-------------------|--------------------------|------------------------
Long64_t           | **trigg_num**            | Sequential number of the trigger packet. Start from 0. -1 when bad. Pedestal and no pedestal packets use different number counter.
Long64_t           | **trigg_num_g**          | Order number at the sequence of appearing in the raw data file. Start from 0. Pedestal and no pedestal packets use the same number counter.
Int_t              | **is_bad**               | if the packet is invalid or has CRC error: 3 when short, 2 when invalid, 1 when crc error, 0 when good
Int_t              | **pre_is_bad**           | if the previous packet is invalid or has CRC error
Int_t              | **type**                 | the 4 types of trigger packet: 0x00F0 for pedestal, 0x00FF nor normal, 0xF000 for single, 0xFF00 for cosmic
Int_t              | **packet_num**           | raw data of packet number of the trigger packet
UInt_t             | **time_stamp**           | raw data of Timestamp register of the trigger packet
UInt_t             | **time_period**          | overflow counter of time_stamp
UInt_t             | **time_wait**            | time_stamp difference since previous event
UInt_t             | **time_align**           | 23 MSB of time_stamp
ULong64_t          | **frm_ship_time**        | raw data of the ship time from frame in which this packet is.
ULong64_t          | **frm_gps_time**         | raw data of the GPS time from frame in which this packet is. Other forms of frm_ship_time and frm_gps_time will be converted when doing UTC calculating.
Long64_t           | **pkt_start**            | first entry index of all the adjacent event packets of this event in the modules tree. -1 when lost all event packets, -2 when bad.
Int_t              | **pkt_count**            | number of entries of event packets for this event in the modules tree
Int_t              | **lost_count**           | number of lost event packets for this event
Int_t              | **trigger_n**            | sum of the trigger_bit[64] of all the event packets for this event
UShort_t           | **status**               | raw data of Status register of the trigger packet
Trigg_Status_T     | **status_bit**           | each bit in status. see [Trigg_Status_T](#struct-trigg_status_t)
UChar_t            | **trig_sig_con[25]**     | raw data of Trigger signals conditions for each frontend
Trig_Sig_Con_T     | **trig_sig_con_bit**     | each bit in trig_sig_con[25] for each frontend. see [Trig_Sig_Con_T](#struct-trig_sig_con_t)
Bool_t             | **trig_accepted[25]**    | raw data of FEE TRIGGER ACCEPTED for each frontend
Bool_t             | **trig_rejected[25]**    | raw data of FEE TRIGGER REJECTED for each frontend
UInt_t             | **raw_dead**             | raw data of the dead time counter field
Float_t            | **dead_ratio**           | increased number of raw_dead divided by time_wait

### struct Event_Status_T

Type               | Name                     | Bit Position
-------------------|--------------------------|------------------------
Bool_t             | **trigger_fe_busy**      | 15
Bool_t             | **fifo_full**            | 14
Bool_t             | **fifo_empty**           | 13
Bool_t             | **trigger_enable**       | 12
Bool_t             | **trigger_waiting**      | 11
Bool_t             | **trigger_hold_b**       | 10
Bool_t             | **timestamp_enable**     |  9
Bool_t             | **reduction_mode_b1**    |  8
Bool_t             | **reduction_mode_b0**    |  7
Bool_t             | **subsystem_busy**       |  6
Bool_t             | **dynode_2**             |  5
Bool_t             | **dynode_1**             |  4
Bool_t             | **dy12_too_high**        |  3
Bool_t             | **t_out_too_many**       |  2
Bool_t             | **t_out_2**              |  1
Bool_t             | **t_out_1**              |  0

### struct Trigg_Status_T

Type               | Name                     | Bit Position
-------------------|--------------------------|------------------------
Bool_t             | **science_disable**      | 15
Bool_t             | **master_clock_enable**  | 14
Bool_t             | **saving_data**          | 13
Bool_t             | **taking_event_or_ped**  | 12
Bool_t             | **fifo_full**            | 11
Bool_t             | **fifo_almost_full**     | 10
Bool_t             | **fifo_empty**           |  9
Bool_t             | **fifo_almost_empty**    |  8
Bool_t             | **any_waiting**          |  7
Bool_t             | **any_waiting_two_hits** |  6
Bool_t             | **any_tmany_thigh**      |  5
Bool_t             | **packet_type_b2**       |  4
Bool_t             | **packet_type_b1**       |  3
Bool_t             | **packet_type_b0**       |  2

### struct Trig_Sig_Con_T

Type               | Name                     | Bit Position
-------------------|--------------------------|------------------------
Bool_t             | **fe_busy[25]**          |  5
Bool_t             | **fe_waiting[25]**       |  4
Bool_t             | **fe_hold_b[25]**        |  3
Bool_t             | **fe_tmany_thigh[25]**   |  2
Bool_t             | **fe_tout_2[25]**        |  1
Bool_t             | **fe_tout_1[25]**        |  0

## Data Structure Convention of HK

### struct Hk_Obox_T

Type               | Name                     | Description
-------------------|--------------------------|------------------------
Int_t              | **odd_index**            | -1 when lost
Int_t              | **even_index**           | -1 when lost
Int_t              | **odd_is_bad**           | 3 when lost, 2 when invalid, 1 when crc error, 0 when good
Int_t              | **even_is_bad**          | 3 when lost, 2 when invalid, 1 when crc error, 0 when good
UShort_t           | **packet_num**           | raw data of obox packet number
UInt_t             | **timestamp**            | raw data of obox timestamp
UShort_t           | **obox_mode**            | raw data of obox operational mode
UShort_t           | **cpu_status**           | raw data of ct_cpu status
UShort_t           | **trig_status**          | raw data of ct_trig status
UShort_t           | **comm_status**          | raw data of ct_com status
UShort_t           | **ct_temp**              | raw data of ct temperature
UShort_t           | **chain_temp**           | raw data of sensor chain temperature
UShort_t           | **reserved**             | raw data of reserved information in obox hk packet
UShort_t           | **lv_status**            | raw data of lv power supply status
UShort_t           | **fe_pattern**           | raw data of fes and flexes powered
UShort_t           | **hv_pwm**               | raw data of hv pwm setting
UShort_t           | **hv_status**            | raw data of hvps status
UShort_t           | **hv_current[2]**        | raw data of hv current readout1 and readout2
UShort_t           | **fe_temp[25]**          | raw data of ?
UShort_t           | **fe_hv[25]**            | raw data of ?
UShort_t           | **fe_thr[25]**           | raw data of ?
UShort_t           | **fe_rate[25]**          | raw data of ?
UShort_t           | **fe_cosmic[25]**        | raw data of ?
UShort_t           | **flex_i_p3v3[5]**       | raw data of flex1-5 +3.3v current
UShort_t           | **flex_i_p1v7[5]**       | raw data of flex1-5 +1.7v current
UShort_t           | **flex_i_n2v5[5]**       | raw data of flex1-5 -2.5v current
UShort_t           | **flex_v_p3v3[5]**       | raw data of flex1-5 +3.3v voltage
UShort_t           | **flex_v_p1v7[5]**       | raw data of flex1-5 +1.7v voltage
UShort_t           | **flex_v_n2v5[5]**       | raw data of flex1-5 -2.5v voltage
UShort_t           | **hv_v_hot**             | raw data of ?
UShort_t           | **hv_i_hot**             | raw data of ?
UShort_t           | **ct_v_hot[2]**          | raw data of ?
UShort_t           | **ct_i_hot[2]**          | raw data of ?
UShort_t           | **hv_v_cold**            | raw data of ?
UShort_t           | **hv_i_cold**            | raw data of ?
UShort_t           | **ct_v_cold[2]**         | raw data of ?
UShort_t           | **ct_i_cold[2]**         | raw data of ?
UInt_t             | **timestamp_sync**       | raw data of last timestamp at sync
UShort_t           | **command_rec**          | raw data of obox received commands cnt
UShort_t           | **command_exec**         | raw data of obox executed commands cnt
UShort_t           | **command_last_num**     | raw data of cmd feedback: executed number
UShort_t           | **command_last_stamp**   | raw data of cmd feedback: executed timestamp
UShort_t           | **command_last_exec**    | raw data of cmd feedback: executed code
UShort_t           | **command_last_arg[2]**  | raw data of cmd feedback: executed arg1 and arg2
UShort_t           | **obox_hk_crc**          | raw data of obox hk packet crc followd
UShort_t           | **saa**                  | raw data of saa flag
UShort_t           | **sci_head**             | raw data of obox science packet header counter
ULong64_t          | **gps_pps_count**        | raw data of Time_PPS
ULong64_t          | **gps_sync_gen_count**   | raw data of Time_synchComGen
ULong64_t          | **gps_sync_send_count**  | raw data of Time_synchComTx
UShort_t           | **hk_head**              | raw data of obox hk header counter
UShort_t           | **hk_tail**              | raw data of obox hk tail counter

### struct Hk_Ibox_T

Type               | Name                     | Description
-------------------|--------------------------|------------------------
Int_t              | **frm_index**            | raw data of frame index
Int_t              | **pkt_tag**              | raw data of packet tag
Int_t              | **is_bad**               | 2 when invalid, 1 when crc error, 0 when good
ULong64_t          | **ship_time**            | raw data of ship time
UShort_t           | **error[2]**             | raw data of command feedback error number1 and number2
UShort_t           | **frame_head**           | raw data of frame header
UShort_t           | **command_head**         | raw data of command frame header
UShort_t           | **command_num**          | raw data of command number
UShort_t           | **command_code**         | raw data of comand code
UShort_t           | **command_arg[2]**       | raw data of command argument1 and argument2
UShort_t           | **head**                 | raw data of obox hk header counter
UShort_t           | **tail**                 | raw data of obox hk tail counter
ULong64_t          | **ibox_gps**             | raw data of ibox gps time

