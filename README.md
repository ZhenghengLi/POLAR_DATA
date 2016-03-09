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
Int_t              | **is_bad**               | if the packet is invalid or has CRC error: 3 when short, 2 when invalid, 1 when crc error, 0 when good, -1 when timestamp is 0.
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
Int_t              | **is_bad**               | if the packet is invalid or has CRC error: 3 when short, 2 when invalid, 1 when crc error, 0 when good, -1 when timestamp is 0.
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

Type               | Name                     | Bit Position  | Description 
-------------------|--------------------------|---------------|-------------------------
Bool_t             | **trigger_fe_busy**      | 15            | Flag indicating Frontend Unit is busys.
Bool_t             | **fifo_full**            | 14            | Flag indicating FIFO memory for events is full.
Bool_t             | **fifo_empty**           | 13            | Flag indicating FIFO memory for events is empty.
Bool_t             | **trigger_enable**       | 12            | Flag indicating trigger is enabled.
Bool_t             | **trigger_waiting**      | 11            | Flag indicating FE is waiting for trigger acceptance.
Bool_t             | **trigger_hold_b**       | 10            | Flag indicating HOLD B signal on FE is asserted.
Bool_t             | **timestamp_enable**     |  9            | Flag indicating timestamp is enabled.
Bool_t             | **reduction_mode_b1**    |  8            | bit 1 of Field indicating the reduction mode of the Frontend Unit.
Bool_t             | **reduction_mode_b0**    |  7            | bit 0 of Field indicating the reduction mode of the Frontend Unit.
Bool_t             | **subsystem_busy**       |  6            | Flag indicating one of three subsystems is busy.
Bool_t             | **dynode_2**             |  5            | Flag indicating DYNODE 2 triggered.
Bool_t             | **dynode_1**             |  4            | Flag indicating DYNODE 1 triggered.
Bool_t             | **dy12_too_high**        |  3            | Flag indicating DY12 TOO HIGH triggered.
Bool_t             | **t_out_too_many**       |  2            | Flag indicating T OUT TOO MANY triggered.
Bool_t             | **t_out_2**              |  1            | Flag indicating T OUT 2 triggered.
Bool_t             | **t_out_1**              |  0            | Flag indicating T OUT 1 triggered.

### struct Trigg_Status_T

Type               | Name                     | Bit Position  | Description
-------------------|--------------------------|---------------|-------------------------
Bool_t             | **science_disable**      | 15            | Flag indicating the science packets generation by Central Trigger Unit is disabled.
Bool_t             | **master_clock_enable**  | 14            | Flag indicating the Master Clock generation in enabled.
Bool_t             | **saving_data**          | 13            | Flag indicating the science packet is being stored in FIFO.
Bool_t             | **taking_event_or_ped**  | 12            | Flag indicating the Central Trigger Unit state machine is doing the event or pedestal acquisition.
Bool_t             | **fifo_full**            | 11            | Flag indicating FIFO in Central Processing Unit is full.
Bool_t             | **fifo_almost_full**     | 10            | Flag indicating FIFO in Central Processing Unit is almost full.
Bool_t             | **fifo_empty**           |  9            | Flag indicating FIFO in Central Processing Unit is empty.
Bool_t             | **fifo_almost_empty**    |  8            | Flag indicating FIFO in Central Processing Unit is almost empty.
Bool_t             | **any_waiting**          |  7            | Flag indicating at least one FEE sent the WAITING signal to Central Processing Unit.
Bool_t             | **any_waiting_two_hits** |  6            | Flag indicating at least one FEE, that has two hits, sent the WAITING signal to Central Processing Unit.
Bool_t             | **any_tmany_thigh**      |  5            | Flag indicating at least one FEE, that has TooMany or TooHigh flags set, sent the WAITING signal to Central Processing Unit.
Bool_t             | **packet_type_b2**       |  4            | bit 2 of Field indicating the type of science packet being processed by the state machine of Central Trigger Unit.
Bool_t             | **packet_type_b1**       |  3            | bit 1 of Field indicating the type of science packet being processed by the state machine of Central Trigger Unit.
Bool_t             | **packet_type_b0**       |  2            | bit 0 of Field indicating the type of science packet being processed by the state machine of Central Trigger Unit.

### struct Trig_Sig_Con_T

Type               | Name                     | Bit Position  | Description
-------------------|--------------------------|---------------|-------------------------
Bool_t             | **fe_busy[25]**          |  5            | Flag indicating the status of the FE BUSY signal from this Frontend Unit.
Bool_t             | **fe_waiting[25]**       |  4            | Flag indicating the status of the FE WAITING signal from this Frontend Unit.
Bool_t             | **fe_hold_b[25]**        |  3            | Flag indicating the status of the FE HOLD B signal from this Frontend Unit.
Bool_t             | **fe_tmany_thigh[25]**   |  2            | Flag indicating the status of the FE TMANY THIGH signal from this Frontend Unit.
Bool_t             | **fe_tout_2[25]**        |  1            | Flag indicating the status of the FE TOUT 2 signal from this Frontend Unit.
Bool_t             | **fe_tout_1[25]**        |  0            | Flag indicating the status of the FE TOUT 1 signal from this Frontend Unit.

## Data Structure Convention of HK

### struct Hk_Obox_T

Type               | Name                     | Description
-------------------|--------------------------|------------------------
Int_t              | **odd_index**            | Frame Index of odd packet, -1 when lost
Int_t              | **even_index**           | Frame Index of even packet, -1 when lost
Int_t              | **odd_is_bad**           | 3 when lost, 2 when invalid, 1 when crc error, 0 when good
Int_t              | **even_is_bad**          | 3 when lost, 2 when invalid, 1 when crc error, 0 when good
Int_t              | **obox_is_bad**          | 3 when half, 2 when invalid, 1 when crc error, 0 when good
UShort_t           | **packet_num**           | raw data of OBOX packet number
UInt_t             | **timestamp**            | raw data of OBOX Timestamp
UChar_t            | **obox_mode**            | raw data of OBOX operational mode
UShort_t           | **cpu_status**           | raw data of OBOX CT CPU status
UChar_t            | **trig_status**          | raw data of OBOX CT Trigger status
UChar_t            | **comm_status**          | raw data of OBOX CT Communication status
Float_t            | **ct_temp**              | physical value of Central Trigger temperature
Float_t            | **chain_temp**           | physical value of Sensor chain temperature
UShort_t           | **reserved**             | raw data of Reserved
UShort_t           | **lv_status**            | raw data of LV power supply status
UInt_t             | **fe_pattern**           | raw data of FEs powered
Float_t            | **lv_temp**              | physical value of LV power supply temperature
UShort_t           | **hv_pwm**               | raw data of HV PWM setting
UShort_t           | **hv_status**            | raw data of HV power supply status
UShort_t           | **hv_current[2]**        | raw data of HV current readout1 and readout2
UChar_t            | **fe_status[25]**        | raw data of module status
Float_t            | **fe_temp[25]**          | physical value of module temperature
Float_t            | **fe_hv[25]**            | physical value of HV voltage setting
Float_t            | **fe_thr[25]**           | physical value of threshold setting
UShort_t           | **fe_rate[25]**          | raw data of count ratr
UShort_t           | **fe_cosmic[25]**        | raw data of too many / too high rate
Float_t            | **flex_i_p3v3[5]**       | physical value of Current at P3V3 rail
Float_t            | **flex_i_p1v7[5]**       | physical value of Current at P1V7 rail
Float_t            | **flex_i_n2v5[5]**       | physical value of Current at N2V5 rail
Float_t            | **flex_v_p3v3[5]**       | physical value of Voltage at P3V3 rail
Float_t            | **flex_v_p1v7[5]**       | physical value of Voltage at P1V7 rail
Float_t            | **flex_v_n2v5[5]**       | physical value of Voltage at N2V5 rail
Float_t            | **hv_v_hot**             | physical value of Voltage at HV Hot P3V3 rail
Float_t            | **hv_i_hot**             | physical value of Current at HV Hot P3V3 rail
Float_t            | **ct_v_hot[2]**          | physical value of Voltage at CT Hot P3V3 and 1V5 rail
Float_t            | **ct_i_hot[2]**          | physical value of Current at CT Hot P3V3 and 1V5 rail
Float_t            | **hv_v_cold**            | physical value of Voltage at HV Cold P3V3 rail
Float_t            | **hv_i_cold**            | physical value of Current at HV Cold P3V3 rail
Float_t            | **ct_v_cold[2]**         | physical value of Voltage at CT Cold P3V3 and 1V5 rail
Float_t            | **ct_i_cold[2]**         | physical value of Current at CT Cold P3V3 and 1V5 rail
UInt_t             | **timestamp_sync**       | raw data of Timestamp at last sync
UShort_t           | **command_rec**          | raw data of Command received counter
UShort_t           | **command_exec**         | raw data of Command executed counter
UShort_t           | **command_last_num**     | raw data of Command last executed number
UShort_t           | **command_last_stamp**   | raw data of Command last executed Timestamp
UShort_t           | **command_last_exec**    | raw data of Command last executed code
UShort_t           | **command_last_arg[2]**  | raw data of Command last executed argument 1 and argument 2
UShort_t           | **obox_hk_crc**          | raw data of OBOX HK packet CRC
UShort_t           | **saa**                  | raw data of SAA flag
UShort_t           | **sci_head**             | raw data of OBOX science packet header counter
ULong64_t          | **gps_pps_count**        | raw data of Time_PPS
ULong64_t          | **gps_sync_gen_count**   | raw data of Time_synchComGen
ULong64_t          | **gps_sync_send_count**  | raw data of Time_synchComTx
UShort_t           | **hk_head**              | raw data of OBOX HK header counter
UShort_t           | **hk_tail**              | raw data of OBOX HK tail counter

### struct Hk_Ibox_T

Type               | Name                     | Description
-------------------|--------------------------|------------------------
Int_t              | **frm_index**            | raw data of Frame Index
Int_t              | **pkt_tag**              | raw data of Packet tag
Int_t              | **is_bad**               | 2 when invalid, 1 when crc error, 0 when good
ULong64_t          | **ship_time**            | raw data of Ship time
UShort_t           | **error[2]**             | raw data of Command feedback error number1 and number2
UShort_t           | **frame_head**           | raw data of Frame header
UShort_t           | **command_head**         | raw data of Command frame header
UShort_t           | **command_num**          | raw data of Command number
UShort_t           | **command_code**         | raw data of Command code
UShort_t           | **command_arg[2]**       | raw data of Command argument1 and argument2
UShort_t           | **head**                 | raw data of OBOX HK header counter
UShort_t           | **tail**                 | raw data of OBOX HK tail counter
ULong64_t          | **ibox_gps**             | raw data of IBOX GPS time

