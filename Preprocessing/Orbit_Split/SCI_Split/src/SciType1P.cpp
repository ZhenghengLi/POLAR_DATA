#include "SciType1P.hpp"

void SciType1P::bind_trigger_tree(TTree* t_trigger_tree, Trigger_T& t_trigger) {
    t_trigger_tree->SetBranchAddress("trigg_num",         &t_trigger.trigg_num                   );
    t_trigger_tree->SetBranchAddress("trigg_num_g",       &t_trigger.trigg_num_g                 );
    t_trigger_tree->SetBranchAddress("is_bad",            &t_trigger.is_bad                      );
    t_trigger_tree->SetBranchAddress("pre_is_bad",        &t_trigger.pre_is_bad                  );
    t_trigger_tree->SetBranchAddress("type",              &t_trigger.type                        );
    t_trigger_tree->SetBranchAddress("packet_num",        &t_trigger.packet_num                  );
    t_trigger_tree->SetBranchAddress("time_stamp",        &t_trigger.time_stamp                  );
    t_trigger_tree->SetBranchAddress("time_period",       &t_trigger.time_period                 );
    t_trigger_tree->SetBranchAddress("time_align",        &t_trigger.time_align                  );
    t_trigger_tree->SetBranchAddress("time_second",       &t_trigger.time_second                 );
    t_trigger_tree->SetBranchAddress("time_wait",         &t_trigger.time_wait                   );
    t_trigger_tree->SetBranchAddress("frm_ship_time",     &t_trigger.frm_ship_time               );
    t_trigger_tree->SetBranchAddress("frm_gps_time",      &t_trigger.frm_gps_time                );
    t_trigger_tree->SetBranchAddress("pkt_start",         &t_trigger.pkt_start                   );
    t_trigger_tree->SetBranchAddress("pkt_count",         &t_trigger.pkt_count                   );
    t_trigger_tree->SetBranchAddress("lost_count",        &t_trigger.lost_count                  );
    t_trigger_tree->SetBranchAddress("trigger_n",         &t_trigger.trigger_n                   );
    t_trigger_tree->SetBranchAddress("status",            &t_trigger.status                      );
    t_trigger_tree->SetBranchAddress("status_bit",        &t_trigger.status_bit.science_disable  );
    t_trigger_tree->SetBranchAddress("trig_sig_con",       t_trigger.trig_sig_con                );
    t_trigger_tree->SetBranchAddress("trig_sig_con_bit",   t_trigger.trig_sig_con_bit.fe_busy    );
    t_trigger_tree->SetBranchAddress("trig_accepted",      t_trigger.trig_accepted               );
    t_trigger_tree->SetBranchAddress("trig_rejected",      t_trigger.trig_rejected               );
    t_trigger_tree->SetBranchAddress("raw_dead",          &t_trigger.raw_dead                    );
    t_trigger_tree->SetBranchAddress("dead_ratio",        &t_trigger.dead_ratio                  );
    t_trigger_tree->SetBranchAddress("abs_gps_week",      &t_trigger.abs_gps_week                );
    t_trigger_tree->SetBranchAddress("abs_gps_second",    &t_trigger.abs_gps_second              );
    t_trigger_tree->SetBranchAddress("abs_gps_valid",     &t_trigger.abs_gps_valid               );
    t_trigger_tree->SetBranchAddress("abs_ship_second",   &t_trigger.abs_ship_second             );
}

void SciType1P::bind_modules_tree(TTree* t_modules_tree, Modules_T& t_modules) {
    t_modules_tree->SetBranchAddress("trigg_num",         &t_modules.trigg_num                   );
    t_modules_tree->SetBranchAddress("event_num",         &t_modules.event_num                   );
    t_modules_tree->SetBranchAddress("event_num_g",       &t_modules.event_num_g                 );
    t_modules_tree->SetBranchAddress("is_bad",            &t_modules.is_bad                      );
    t_modules_tree->SetBranchAddress("pre_is_bad",        &t_modules.pre_is_bad                  );
    t_modules_tree->SetBranchAddress("compress",          &t_modules.compress                    );
    t_modules_tree->SetBranchAddress("ct_num",            &t_modules.ct_num                      );
    t_modules_tree->SetBranchAddress("time_stamp",        &t_modules.time_stamp                  );
    t_modules_tree->SetBranchAddress("time_period",       &t_modules.time_period                 );
    t_modules_tree->SetBranchAddress("time_align",        &t_modules.time_align                  );
    t_modules_tree->SetBranchAddress("time_second",       &t_modules.time_second                 );
    t_modules_tree->SetBranchAddress("time_wait",         &t_modules.time_wait                   );
    t_modules_tree->SetBranchAddress("raw_rate",          &t_modules.raw_rate                    );
    t_modules_tree->SetBranchAddress("raw_dead",          &t_modules.raw_dead                    );
    t_modules_tree->SetBranchAddress("dead_ratio",        &t_modules.dead_ratio                  );
    t_modules_tree->SetBranchAddress("status",            &t_modules.status                      );
    t_modules_tree->SetBranchAddress("status_bit",        &t_modules.status_bit.trigger_fe_busy  );
    t_modules_tree->SetBranchAddress("trigger_bit",        t_modules.trigger_bit                 );
    t_modules_tree->SetBranchAddress("energy_adc",         t_modules.energy_adc                  );
    t_modules_tree->SetBranchAddress("common_noise",      &t_modules.common_noise                );
    t_modules_tree->SetBranchAddress("multiplicity",      &t_modules.multiplicity                );
}

void SciType1P::build_trigger_tree(TTree* t_trigger_tree, Trigger_T& t_trigger) {
    t_trigger_tree->Branch("trigg_num",         &t_trigger.trigg_num,         "trigg_num/L"           );
    t_trigger_tree->Branch("trigg_num_g",       &t_trigger.trigg_num_g,       "trigg_num_g/L"         );
    t_trigger_tree->Branch("is_bad",            &t_trigger.is_bad,            "is_bad/I"              );
    t_trigger_tree->Branch("pre_is_bad",        &t_trigger.pre_is_bad,        "pre_is_bad/I"          );
    t_trigger_tree->Branch("type",              &t_trigger.type,              "type/I"                );
    t_trigger_tree->Branch("packet_num",        &t_trigger.packet_num,        "packet_num/I"          );
    t_trigger_tree->Branch("time_stamp",        &t_trigger.time_stamp,        "time_stamp/i"          );
    t_trigger_tree->Branch("time_period",       &t_trigger.time_period,       "time_period/i"         );
    t_trigger_tree->Branch("time_align",        &t_trigger.time_align,        "time_align/i"          );
    t_trigger_tree->Branch("time_second",       &t_trigger.time_second,       "time_second/D"         );
    t_trigger_tree->Branch("time_wait",         &t_trigger.time_wait,         "time_wait/D"           );
    t_trigger_tree->Branch("frm_ship_time",     &t_trigger.frm_ship_time,     "frm_ship_time/l"       );
    t_trigger_tree->Branch("frm_gps_time",      &t_trigger.frm_gps_time,      "frm_gps_time/l"        );
    t_trigger_tree->Branch("pkt_start",         &t_trigger.pkt_start,         "pkt_start/L"           );
    t_trigger_tree->Branch("pkt_count",         &t_trigger.pkt_count,         "pkt_count/I"           );
    t_trigger_tree->Branch("lost_count",        &t_trigger.lost_count,        "lost_count/I"          );
    t_trigger_tree->Branch("trigger_n",         &t_trigger.trigger_n,         "trigger_n/I"           );
    t_trigger_tree->Branch("status",            &t_trigger.status,            "status/s"              );
    t_trigger_tree->Branch("status_bit",        &t_trigger.status_bit.science_disable,
                            "science_disable/O:master_clock_enable:saving_data:taking_event_or_ped:"
                            "fifo_full:fifo_almost_full:fifo_empty:fifo_almost_empty:"
                            "any_waiting:any_waiting_two_hits:any_tmany_thigh:"
                            "packet_type_b2:packet_type_b1:packet_type_b0"                            );
    t_trigger_tree->Branch("trig_sig_con",       t_trigger.trig_sig_con,      "trig_sig_con[25]/b"    );
    t_trigger_tree->Branch("trig_sig_con_bit",   t_trigger.trig_sig_con_bit.fe_busy,
                            "fe_busy[25]/O:fe_waiting[25]:"
                            "fe_hold_b[25]:fe_tmany_thigh[25]:fe_tout_2[25]:fe_tout_1[25]"            );
    t_trigger_tree->Branch("trig_accepted",      t_trigger.trig_accepted,     "trig_accepted[25]/O"   );
    t_trigger_tree->Branch("trig_rejected",      t_trigger.trig_rejected,     "trig_rejected[25]/O"   );
    t_trigger_tree->Branch("raw_dead",          &t_trigger.raw_dead,          "raw_dead/i"            );
    t_trigger_tree->Branch("dead_ratio",        &t_trigger.dead_ratio,        "dead_ratio/F"          );
    t_trigger_tree->Branch("abs_gps_week",      &t_trigger.abs_gps_week,      "abs_gps_week/I"        );
    t_trigger_tree->Branch("abs_gps_second",    &t_trigger.abs_gps_second,    "abs_gps_second/D"      );
    t_trigger_tree->Branch("abs_gps_valid",     &t_trigger.abs_gps_valid,     "abs_gps_valid/O"       );
    t_trigger_tree->Branch("abs_ship_second",   &t_trigger.abs_ship_second,   "abs_ship_second/D"     );
}

void SciType1P::build_modules_tree(TTree* t_modules_tree, Modules_T& t_modules) {
    t_modules_tree->Branch("trigg_num",         &t_modules.trigg_num,         "trigg_num/L"           );
    t_modules_tree->Branch("event_num",         &t_modules.event_num,         "event_num/L"           );
    t_modules_tree->Branch("event_num_g",       &t_modules.event_num_g,       "event_num_g/L"         );
    t_modules_tree->Branch("is_bad",            &t_modules.is_bad,            "is_bad/I"              );
    t_modules_tree->Branch("pre_is_bad",        &t_modules.pre_is_bad,        "pre_is_bad/I"          );
    t_modules_tree->Branch("compress",          &t_modules.compress,          "compress/I"            );
    t_modules_tree->Branch("ct_num",            &t_modules.ct_num,            "ct_num/I"              );
    t_modules_tree->Branch("time_stamp",        &t_modules.time_stamp,        "time_stamp/i"          );
    t_modules_tree->Branch("time_period",       &t_modules.time_period,       "time_period/i"         );
    t_modules_tree->Branch("time_align",        &t_modules.time_align,        "time_align/i"          );
    t_modules_tree->Branch("time_second",       &t_modules.time_second,       "time_second/D"         );
    t_modules_tree->Branch("time_wait",         &t_modules.time_wait,         "time_wait/D"           );
    t_modules_tree->Branch("raw_rate",          &t_modules.raw_rate,          "raw_rate/I"            );
    t_modules_tree->Branch("raw_dead",          &t_modules.raw_dead,          "raw_dead/i"            );
    t_modules_tree->Branch("dead_ratio",        &t_modules.dead_ratio,        "dead_ratio/F"          );
    t_modules_tree->Branch("status",            &t_modules.status,            "status/s"              );
    t_modules_tree->Branch("status_bit",        &t_modules.status_bit.trigger_fe_busy,
                            "trigger_fe_busy/O:fifo_full:fifo_empty:trigger_enable:"
                            "trigger_waiting:trigger_hold_b:timestamp_enable:reduction_mode_b1:"
                            "reduction_mode_b0:subsystem_busy:dynode_2:dynode_1:"
                            "dy12_too_high:t_out_too_many:t_out_2:t_out_1"                            );
    t_modules_tree->Branch("trigger_bit",        t_modules.trigger_bit,       "trigger_bit[64]/O"     );
    t_modules_tree->Branch("energy_adc",         t_modules.energy_adc,        "energy_adc[64]/F"      );
    t_modules_tree->Branch("common_noise",      &t_modules.common_noise,      "common_noise/F"        );
    t_modules_tree->Branch("multiplicity",      &t_modules.multiplicity,      "multiplicity/I"        );
}
