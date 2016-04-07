#include "SciTransfer.hpp"

SciTransfer::SciTransfer() {
    t_file_in_ = NULL;
    t_modules_tree_in_ = NULL;
    t_trigger_tree_in_ = NULL;
    t_ped_modules_tree_in_ = NULL;
    t_ped_trigger_tree_in_ = NULL;

    t_file_out_ = NULL;
    t_modules_tree_out_ = NULL;
    t_trigger_tree_out_ = NULL;
    t_ped_modules_tree_out_ = NULL;
    t_ped_trigger_tree_out_ = NULL;
}

SciTransfer::~SciTransfer() {
    if (t_file_in_ != NULL)
        close_read();
    if (t_file_out_ != NULL)
        close_write();
}

bool SciTransfer::open_read(const char* filename) {
    if (t_file_in_ != NULL)
        return false;
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_trigger_tree_in_ = static_cast<TTree*>(t_file_in_->Get("t_trigger"));
    if (t_trigger_tree_in_ == NULL)
        return false;
    t_modules_tree_in_ = static_cast<TTree*>(t_file_in_->Get("t_modules"));
    if (t_modules_tree_in_ == NULL)
        return false;
    t_ped_trigger_tree_in_ = static_cast<TTree*>(t_file_in_->Get("t_ped_trigger"));
    if (t_ped_trigger_tree_in_ == NULL)
        return false;
    t_ped_modules_tree_in_ = static_cast<TTree*>(t_file_in_->Get("t_ped_modules"));
    if (t_ped_modules_tree_in_ == NULL)
        return false;
    m_dattype_named_in_ = static_cast<TNamed*>(t_file_in_->Get("m_dattype"));
    if (m_dattype_named_in_ == NULL)
        return false;
    m_version_named_in_ = static_cast<TNamed*>(t_file_in_->Get("m_version"));
    if (m_version_named_in_ == NULL)
        return false;
    m_gentime_named_in_ = static_cast<TNamed*>(t_file_in_->Get("m_gentime"));
    if (m_gentime_named_in_ == NULL)
        return false;
    m_rawfile_named_in_ = static_cast<TNamed*>(t_file_in_->Get("m_rawfile"));
    if (m_rawfile_named_in_ == NULL)
        return false;
    m_dcdinfo_named_in_ = static_cast<TNamed*>(t_file_in_->Get("m_dcdinfo"));
    if (m_dcdinfo_named_in_ == NULL)
        return false;

    // get total entries
    t_trigger_tot_entries_ = t_trigger_tree_in_->GetEntries();
    if (t_trigger_tot_entries_ < 1) {
        close_read();
        return false;
    }
    t_modules_tot_entries_ = t_modules_tree_in_->GetEntries();
    if (t_modules_tot_entries_ < 1) {
        close_read();
        return false;
    }
    t_ped_trigger_tot_entries_ = t_ped_trigger_tree_in_->GetEntries();
    if (t_ped_trigger_tot_entries_ < 1) {
        close_read();
        return false;
    }
    t_ped_modules_tot_entries_ = t_ped_modules_tree_in_->GetEntries();
    if (t_ped_modules_tot_entries_ < 1) {
        close_read();
        return false;
    }

    read_set_start();
    
    // t_trigger
    t_trigger_tree_in_->SetBranchAddress("trigg_num",         &t_trigger.trigg_num                   );
    t_trigger_tree_in_->SetBranchAddress("trigg_num_g",       &t_trigger.trigg_num_g                 );
    t_trigger_tree_in_->SetBranchAddress("is_bad",            &t_trigger.is_bad                      );
    t_trigger_tree_in_->SetBranchAddress("pre_is_bad",        &t_trigger.pre_is_bad                  );
    t_trigger_tree_in_->SetBranchAddress("type",              &t_trigger.type                        );
    t_trigger_tree_in_->SetBranchAddress("packet_num",        &t_trigger.packet_num                  );
    t_trigger_tree_in_->SetBranchAddress("time_stamp",        &t_trigger.time_stamp                  );
    t_trigger_tree_in_->SetBranchAddress("time_period",       &t_trigger.time_period                 );
    t_trigger_tree_in_->SetBranchAddress("time_align",        &t_trigger.time_align                  );
    t_trigger_tree_in_->SetBranchAddress("time_second",       &t_trigger.time_second                 );
    t_trigger_tree_in_->SetBranchAddress("time_wait",         &t_trigger.time_wait                   );
    t_trigger_tree_in_->SetBranchAddress("frm_ship_time",     &t_trigger.frm_ship_time               );
    t_trigger_tree_in_->SetBranchAddress("frm_gps_time",      &t_trigger.frm_gps_time                );
    t_trigger_tree_in_->SetBranchAddress("pkt_start",         &t_trigger.pkt_start                   );
    t_trigger_tree_in_->SetBranchAddress("pkt_count",         &t_trigger.pkt_count                   );
    t_trigger_tree_in_->SetBranchAddress("lost_count",        &t_trigger.lost_count                  );
    t_trigger_tree_in_->SetBranchAddress("trigger_n",         &t_trigger.trigger_n                   );
    t_trigger_tree_in_->SetBranchAddress("status",            &t_trigger.status                      );
    t_trigger_tree_in_->SetBranchAddress("status_bit",        &t_trigger.status_bit.science_disable  );  
    t_trigger_tree_in_->SetBranchAddress("trig_sig_con",       t_trigger.trig_sig_con                );
    t_trigger_tree_in_->SetBranchAddress("trig_sig_con_bit",   t_trigger.trig_sig_con_bit.fe_busy    );  
    t_trigger_tree_in_->SetBranchAddress("trig_accepted",      t_trigger.trig_accepted               );
    t_trigger_tree_in_->SetBranchAddress("trig_rejected",      t_trigger.trig_rejected               );
    t_trigger_tree_in_->SetBranchAddress("raw_dead",          &t_trigger.raw_dead                    );
    t_trigger_tree_in_->SetBranchAddress("dead_ratio",        &t_trigger.dead_ratio                  );

    // t_modules
    t_modules_tree_in_->SetBranchAddress("trigg_num",         &t_modules.trigg_num                   );
    t_modules_tree_in_->SetBranchAddress("event_num",         &t_modules.event_num                   );
    t_modules_tree_in_->SetBranchAddress("event_num_g",       &t_modules.event_num_g                 );
    t_modules_tree_in_->SetBranchAddress("is_bad",            &t_modules.is_bad                      );
    t_modules_tree_in_->SetBranchAddress("pre_is_bad",        &t_modules.pre_is_bad                  );
    t_modules_tree_in_->SetBranchAddress("compress",          &t_modules.compress                    );
    t_modules_tree_in_->SetBranchAddress("ct_num",            &t_modules.ct_num                      );
    t_modules_tree_in_->SetBranchAddress("time_stamp",        &t_modules.time_stamp                  );
    t_modules_tree_in_->SetBranchAddress("time_period",       &t_modules.time_period                 );
    t_modules_tree_in_->SetBranchAddress("time_align",        &t_modules.time_align                  );
    t_modules_tree_in_->SetBranchAddress("time_second",       &t_modules.time_second                 );
    t_modules_tree_in_->SetBranchAddress("time_wait",         &t_modules.time_wait                   );
    t_modules_tree_in_->SetBranchAddress("raw_rate",          &t_modules.raw_rate                    );
    t_modules_tree_in_->SetBranchAddress("raw_dead",          &t_modules.raw_dead                    );
    t_modules_tree_in_->SetBranchAddress("dead_ratio",        &t_modules.dead_ratio                  );
    t_modules_tree_in_->SetBranchAddress("status",            &t_modules.status                      );
    t_modules_tree_in_->SetBranchAddress("status_bit",        &t_modules.status_bit.trigger_fe_busy  );
    t_modules_tree_in_->SetBranchAddress("trigger_bit",        t_modules.trigger_bit                 );
    t_modules_tree_in_->SetBranchAddress("energy_adc",         t_modules.energy_adc                  );
    t_modules_tree_in_->SetBranchAddress("common_noise",      &t_modules.common_noise                );
    t_modules_tree_in_->SetBranchAddress("multiplicity",      &t_modules.multiplicity                );

    // t_ped_trigger
    t_ped_trigger_tree_in_->SetBranchAddress("trigg_num",         &t_ped_trigger.trigg_num                   );
    t_ped_trigger_tree_in_->SetBranchAddress("trigg_num_g",       &t_ped_trigger.trigg_num_g                 );
    t_ped_trigger_tree_in_->SetBranchAddress("is_bad",            &t_ped_trigger.is_bad                      );
    t_ped_trigger_tree_in_->SetBranchAddress("pre_is_bad",        &t_ped_trigger.pre_is_bad                  );
    t_ped_trigger_tree_in_->SetBranchAddress("type",              &t_ped_trigger.type                        );
    t_ped_trigger_tree_in_->SetBranchAddress("packet_num",        &t_ped_trigger.packet_num                  );
    t_ped_trigger_tree_in_->SetBranchAddress("time_stamp",        &t_ped_trigger.time_stamp                  );
    t_ped_trigger_tree_in_->SetBranchAddress("time_period",       &t_ped_trigger.time_period                 );
    t_ped_trigger_tree_in_->SetBranchAddress("time_align",        &t_ped_trigger.time_align                  );
    t_ped_trigger_tree_in_->SetBranchAddress("time_second",       &t_ped_trigger.time_second                 );
    t_ped_trigger_tree_in_->SetBranchAddress("time_wait",         &t_ped_trigger.time_wait                   );
    t_ped_trigger_tree_in_->SetBranchAddress("frm_ship_time",     &t_ped_trigger.frm_ship_time               );
    t_ped_trigger_tree_in_->SetBranchAddress("frm_gps_time",      &t_ped_trigger.frm_gps_time                );
    t_ped_trigger_tree_in_->SetBranchAddress("pkt_start",         &t_ped_trigger.pkt_start                   );
    t_ped_trigger_tree_in_->SetBranchAddress("pkt_count",         &t_ped_trigger.pkt_count                   );
    t_ped_trigger_tree_in_->SetBranchAddress("lost_count",        &t_ped_trigger.lost_count                  );
    t_ped_trigger_tree_in_->SetBranchAddress("trigger_n",         &t_ped_trigger.trigger_n                   );
    t_ped_trigger_tree_in_->SetBranchAddress("status",            &t_ped_trigger.status                      );
    t_ped_trigger_tree_in_->SetBranchAddress("status_bit",        &t_ped_trigger.status_bit.science_disable  );  
    t_ped_trigger_tree_in_->SetBranchAddress("trig_sig_con",       t_ped_trigger.trig_sig_con                );
    t_ped_trigger_tree_in_->SetBranchAddress("trig_sig_con_bit",   t_ped_trigger.trig_sig_con_bit.fe_busy    );  
    t_ped_trigger_tree_in_->SetBranchAddress("trig_accepted",      t_ped_trigger.trig_accepted               );
    t_ped_trigger_tree_in_->SetBranchAddress("trig_rejected",      t_ped_trigger.trig_rejected               );
    t_ped_trigger_tree_in_->SetBranchAddress("raw_dead",          &t_ped_trigger.raw_dead                    );
    t_ped_trigger_tree_in_->SetBranchAddress("dead_ratio",        &t_ped_trigger.dead_ratio                  );

    // t_ped_modules
    t_ped_modules_tree_in_->SetBranchAddress("trigg_num",         &t_ped_modules.trigg_num                   );
    t_ped_modules_tree_in_->SetBranchAddress("event_num",         &t_ped_modules.event_num                   );
    t_ped_modules_tree_in_->SetBranchAddress("event_num_g",       &t_ped_modules.event_num_g                 );
    t_ped_modules_tree_in_->SetBranchAddress("is_bad",            &t_ped_modules.is_bad                      );
    t_ped_modules_tree_in_->SetBranchAddress("pre_is_bad",        &t_ped_modules.pre_is_bad                  );
    t_ped_modules_tree_in_->SetBranchAddress("compress",          &t_ped_modules.compress                    );
    t_ped_modules_tree_in_->SetBranchAddress("ct_num",            &t_ped_modules.ct_num                      );
    t_ped_modules_tree_in_->SetBranchAddress("time_stamp",        &t_ped_modules.time_stamp                  );
    t_ped_modules_tree_in_->SetBranchAddress("time_period",       &t_ped_modules.time_period                 );
    t_ped_modules_tree_in_->SetBranchAddress("time_align",        &t_ped_modules.time_align                  );
    t_ped_modules_tree_in_->SetBranchAddress("time_second",       &t_ped_modules.time_second                 );
    t_ped_modules_tree_in_->SetBranchAddress("time_wait",         &t_ped_modules.time_wait                   );
    t_ped_modules_tree_in_->SetBranchAddress("raw_rate",          &t_ped_modules.raw_rate                    );
    t_ped_modules_tree_in_->SetBranchAddress("raw_dead",          &t_ped_modules.raw_dead                    );
    t_ped_modules_tree_in_->SetBranchAddress("dead_ratio",        &t_ped_modules.dead_ratio                  );
    t_ped_modules_tree_in_->SetBranchAddress("status",            &t_ped_modules.status                      );
    t_ped_modules_tree_in_->SetBranchAddress("status_bit",        &t_ped_modules.status_bit.trigger_fe_busy  );
    t_ped_modules_tree_in_->SetBranchAddress("trigger_bit",        t_ped_modules.trigger_bit                 );
    t_ped_modules_tree_in_->SetBranchAddress("energy_adc",         t_ped_modules.energy_adc                  );
    t_ped_modules_tree_in_->SetBranchAddress("common_noise",      &t_ped_modules.common_noise                );
    t_ped_modules_tree_in_->SetBranchAddress("multiplicity",      &t_ped_modules.multiplicity                );

    t_trigger_tree_in_->GetEntry(0);
    phy_first_gps.update6(t_trigger.frm_gps_time);
    phy_first_timestamp = t_trigger.time_stamp;
    t_trigger_tree_in_->GetEntry(t_trigger_tot_entries_ - 1);
    phy_last_gps.update6(t_trigger.frm_gps_time);
    phy_last_timestamp = t_trigger.time_stamp;

    t_ped_trigger_tree_in_->GetEntry(0);
    ped_first_gps.update6(t_ped_trigger.frm_gps_time);
    ped_first_timestamp = t_ped_trigger.time_stamp;
    t_ped_trigger_tree_in_->GetEntry(t_ped_trigger_tot_entries_ - 1);
    ped_last_gps.update6(t_ped_trigger.frm_gps_time);
    ped_last_timestamp = t_ped_trigger.time_stamp;
    
    return true;
}

bool SciTransfer::open_write(const char* filename) {
    if (t_file_out_ != NULL)
        return false;
    t_file_out_ = new TFile(filename, "RECREATE");
    if (t_file_out_->IsZombie())
        return false;

    // t_modules
    t_modules_tree_out_ = new TTree("t_modules", "physical modules packets");
    t_modules_tree_out_->SetDirectory(t_file_out_);
    t_modules_tree_out_->Branch("trigg_num",         &t_modules.trigg_num,         "trigg_num/L"           );  
    t_modules_tree_out_->Branch("event_num",         &t_modules.event_num,         "event_num/L"           );  
    t_modules_tree_out_->Branch("event_num_g",       &t_modules.event_num_g,       "event_num_g/L"         );  
    t_modules_tree_out_->Branch("is_bad",            &t_modules.is_bad,            "is_bad/I"              );  
    t_modules_tree_out_->Branch("pre_is_bad",        &t_modules.pre_is_bad,        "pre_is_bad/I"          );  
    t_modules_tree_out_->Branch("compress",          &t_modules.compress,          "compress/I"            );  
    t_modules_tree_out_->Branch("ct_num",            &t_modules.ct_num,            "ct_num/I"              );  
    t_modules_tree_out_->Branch("time_stamp",        &t_modules.time_stamp,        "time_stamp/i"          );  
    t_modules_tree_out_->Branch("time_period",       &t_modules.time_period,       "time_period/i"         );  
    t_modules_tree_out_->Branch("time_align",        &t_modules.time_align,        "time_align/i"          );  
    t_modules_tree_out_->Branch("time_second",       &t_modules.time_second,       "time_second/D"         );  
    t_modules_tree_out_->Branch("time_wait",         &t_modules.time_wait,         "time_wait/D"           );  
    t_modules_tree_out_->Branch("raw_rate",          &t_modules.raw_rate,          "raw_rate/I"            );  
    t_modules_tree_out_->Branch("raw_dead",          &t_modules.raw_dead,          "raw_dead/i"            );  
    t_modules_tree_out_->Branch("dead_ratio",        &t_modules.dead_ratio,        "dead_ratio/F"          );  
    t_modules_tree_out_->Branch("status",            &t_modules.status,            "status/s"              );  
    t_modules_tree_out_->Branch("status_bit",        &t_modules.status_bit.trigger_fe_busy, 
                            "trigger_fe_busy/O:fifo_full:fifo_empty:trigger_enable:"
                            "trigger_waiting:trigger_hold_b:timestamp_enable:reduction_mode_b1:"
                            "reduction_mode_b0:subsystem_busy:dynode_2:dynode_1:"
                            "dy12_too_high:t_out_too_many:t_out_2:t_out_1"                             );  
    t_modules_tree_out_->Branch("trigger_bit",        t_modules.trigger_bit,       "trigger_bit[64]/O"     );  
    t_modules_tree_out_->Branch("energy_adc",         t_modules.energy_adc,        "energy_adc[64]/F"      );  
    t_modules_tree_out_->Branch("common_noise",      &t_modules.common_noise,      "common_noise/F"        );  
    t_modules_tree_out_->Branch("multiplicity",      &t_modules.multiplicity,      "multiplicity/I"        );  

    // t_trigger
    t_trigger_tree_out_ = new TTree("t_trigger", "physical trigger packets");
    t_trigger_tree_out_->SetDirectory(t_file_out_);
    t_trigger_tree_out_->Branch("trigg_num",         &t_trigger.trigg_num,         "trigg_num/L"           );  
    t_trigger_tree_out_->Branch("trigg_num_g",       &t_trigger.trigg_num_g,       "trigg_num_g/L"         );  
    t_trigger_tree_out_->Branch("is_bad",            &t_trigger.is_bad,            "is_bad/I"              );  
    t_trigger_tree_out_->Branch("pre_is_bad",        &t_trigger.pre_is_bad,        "pre_is_bad/I"          );  
    t_trigger_tree_out_->Branch("type",              &t_trigger.type,              "type/I"                );
    t_trigger_tree_out_->Branch("packet_num",        &t_trigger.packet_num,        "packet_num/I"          );
    t_trigger_tree_out_->Branch("time_stamp",        &t_trigger.time_stamp,        "time_stamp/i"          );
    t_trigger_tree_out_->Branch("time_period",       &t_trigger.time_period,       "time_period/i"         );
    t_trigger_tree_out_->Branch("time_align",        &t_trigger.time_align,        "time_align/i"          );
    t_trigger_tree_out_->Branch("time_second",       &t_trigger.time_second,       "time_second/D"         );
    t_trigger_tree_out_->Branch("time_wait",         &t_trigger.time_wait,         "time_wait/D"           );
    t_trigger_tree_out_->Branch("frm_ship_time",     &t_trigger.frm_ship_time,     "frm_ship_time/l"       );
    t_trigger_tree_out_->Branch("frm_gps_time",      &t_trigger.frm_gps_time,      "frm_gps_time/l"        );
    t_trigger_tree_out_->Branch("pkt_start",         &t_trigger.pkt_start,         "pkt_start/L"           );
    t_trigger_tree_out_->Branch("pkt_count",         &t_trigger.pkt_count,         "pkt_count/I"           );
    t_trigger_tree_out_->Branch("lost_count",        &t_trigger.lost_count,        "lost_count/I"          );
    t_trigger_tree_out_->Branch("trigger_n",         &t_trigger.trigger_n,         "trigger_n/I"           );
    t_trigger_tree_out_->Branch("status",            &t_trigger.status,            "status/s"              );
    t_trigger_tree_out_->Branch("status_bit",        &t_trigger.status_bit.science_disable,
                            "science_disable/O:master_clock_enable:saving_data:taking_event_or_ped:"
                            "fifo_full:fifo_almost_full:fifo_empty:fifo_almost_empty:"
                            "any_waiting:any_waiting_two_hits:any_tmany_thigh:"
                            "packet_type_b2:packet_type_b1:packet_type_b0"                             );
    t_trigger_tree_out_->Branch("trig_sig_con",       t_trigger.trig_sig_con,      "trig_sig_con[25]/b"    );
    t_trigger_tree_out_->Branch("trig_sig_con_bit",   t_trigger.trig_sig_con_bit.fe_busy,
                            "fe_busy[25]/O:fe_waiting[25]:"
                            "fe_hold_b[25]:fe_tmany_thigh[25]:fe_tout_2[25]:fe_tout_1[25]"             ); 
    t_trigger_tree_out_->Branch("trig_accepted",      t_trigger.trig_accepted,     "trig_accepted[25]/O"   );
    t_trigger_tree_out_->Branch("trig_rejected",      t_trigger.trig_rejected,     "trig_rejected[25]/O"   );
    t_trigger_tree_out_->Branch("raw_dead",          &t_trigger.raw_dead,          "raw_dead/i"            );
    t_trigger_tree_out_->Branch("dead_ratio",        &t_trigger.dead_ratio,        "dead_ratio/F"          );
    t_trigger_tree_out_->Branch("abs_gps_week",      &t_trigger.abs_gps_week,      "abs_gps_week/I"        );
    t_trigger_tree_out_->Branch("abs_gps_second",    &t_trigger.abs_gps_second,    "abs_gps_second/D"      );
    t_trigger_tree_out_->Branch("abs_gps_valid",     &t_trigger.abs_gps_valid,     "abs_gps_valid/O"       );

    // t_ped_modules
    t_ped_modules_tree_out_ = new TTree("t_ped_modules", "pedestal modules packets");
    t_ped_modules_tree_out_->SetDirectory(t_file_out_);
    t_ped_modules_tree_out_->Branch("trigg_num",         &t_ped_modules.trigg_num,         "trigg_num/L"           );
    t_ped_modules_tree_out_->Branch("event_num",         &t_ped_modules.event_num,         "event_num/L"           );
    t_ped_modules_tree_out_->Branch("event_num_g",       &t_ped_modules.event_num_g,       "event_num_g/L"         );
    t_ped_modules_tree_out_->Branch("is_bad",            &t_ped_modules.is_bad,            "is_bad/I"              );
    t_ped_modules_tree_out_->Branch("pre_is_bad",        &t_ped_modules.pre_is_bad,        "pre_is_bad/I"          );
    t_ped_modules_tree_out_->Branch("compress",          &t_ped_modules.compress,          "compress/I"            );
    t_ped_modules_tree_out_->Branch("ct_num",            &t_ped_modules.ct_num,            "ct_num/I"              );
    t_ped_modules_tree_out_->Branch("time_stamp",        &t_ped_modules.time_stamp,        "time_stamp/i"          );
    t_ped_modules_tree_out_->Branch("time_period",       &t_ped_modules.time_period,       "time_period/i"         );
    t_ped_modules_tree_out_->Branch("time_align",        &t_ped_modules.time_align,        "time_align/i"          );
    t_ped_modules_tree_out_->Branch("time_second",       &t_ped_modules.time_second,       "time_second/D"         );
    t_ped_modules_tree_out_->Branch("time_wait",         &t_ped_modules.time_wait,         "time_wait/D"           );
    t_ped_modules_tree_out_->Branch("raw_rate",          &t_ped_modules.raw_rate,          "raw_rate/I"            );
    t_ped_modules_tree_out_->Branch("raw_dead",          &t_ped_modules.raw_dead,          "raw_dead/i"            );
    t_ped_modules_tree_out_->Branch("dead_ratio",        &t_ped_modules.dead_ratio,        "dead_ratio/F"          );
    t_ped_modules_tree_out_->Branch("status",            &t_ped_modules.status,            "status/s"              );
    t_ped_modules_tree_out_->Branch("status_bit",        &t_ped_modules.status_bit.trigger_fe_busy, 
                                "trigger_fe_busy/O:fifo_full:fifo_empty:trigger_enable:"
                                "trigger_waiting:trigger_hold_b:timestamp_enable:reduction_mode_b1:"
                                "reduction_mode_b0:subsystem_busy:dynode_2:dynode_1:"
                                "dy12_too_high:t_out_too_many:t_out_2:t_out_1"                                 );
    t_ped_modules_tree_out_->Branch("trigger_bit",        t_ped_modules.trigger_bit,       "trigger_bit[64]/O"     );
    t_ped_modules_tree_out_->Branch("energy_adc",         t_ped_modules.energy_adc,        "energy_adc[64]/F"      );
    t_ped_modules_tree_out_->Branch("common_noise",      &t_ped_modules.common_noise,      "common_noise/F"        );
    t_ped_modules_tree_out_->Branch("multiplicity",      &t_ped_modules.multiplicity,      "multiplicity/I"        );

    // t_ped_trigger
    t_ped_trigger_tree_out_ = new TTree("t_ped_trigger", "pedestal trigger packets");
    t_ped_trigger_tree_out_->SetDirectory(t_file_out_);
    t_ped_trigger_tree_out_->Branch("trigg_num",         &t_ped_trigger.trigg_num,         "trigg_num/L"           );
    t_ped_trigger_tree_out_->Branch("trigg_num_g",       &t_ped_trigger.trigg_num_g,       "trigg_num_g/L"         );
    t_ped_trigger_tree_out_->Branch("is_bad",            &t_ped_trigger.is_bad,            "is_bad/I"              );
    t_ped_trigger_tree_out_->Branch("pre_is_bad",        &t_ped_trigger.pre_is_bad,        "pre_is_bad/I"          );
    t_ped_trigger_tree_out_->Branch("type",              &t_ped_trigger.type,              "type/I"                );
    t_ped_trigger_tree_out_->Branch("packet_num",        &t_ped_trigger.packet_num,        "packet_num/I"          );
    t_ped_trigger_tree_out_->Branch("time_stamp",        &t_ped_trigger.time_stamp,        "time_stamp/i"          );
    t_ped_trigger_tree_out_->Branch("time_period",       &t_ped_trigger.time_period,       "time_period/i"         );
    t_ped_trigger_tree_out_->Branch("time_align",        &t_ped_trigger.time_align,        "time_align/i"          );
    t_ped_trigger_tree_out_->Branch("time_second",       &t_ped_trigger.time_second,       "time_second/D"         );
    t_ped_trigger_tree_out_->Branch("time_wait",         &t_ped_trigger.time_wait,         "time_wait/D"           );
    t_ped_trigger_tree_out_->Branch("frm_ship_time",     &t_ped_trigger.frm_ship_time,     "frm_ship_time/l"       );
    t_ped_trigger_tree_out_->Branch("frm_gps_time",      &t_ped_trigger.frm_gps_time,      "frm_gps_time/l"        );
    t_ped_trigger_tree_out_->Branch("pkt_start",         &t_ped_trigger.pkt_start,         "pkt_start/L"           );
    t_ped_trigger_tree_out_->Branch("pkt_count",         &t_ped_trigger.pkt_count,         "pkt_count/I"           );
    t_ped_trigger_tree_out_->Branch("lost_count",        &t_ped_trigger.lost_count,        "lost_count/I"          );
    t_ped_trigger_tree_out_->Branch("trigger_n",         &t_ped_trigger.trigger_n,         "trigger_n/I"           );
    t_ped_trigger_tree_out_->Branch("status",            &t_ped_trigger.status,            "status/s"              );
    t_ped_trigger_tree_out_->Branch("status_bit",        &t_ped_trigger.status_bit.science_disable,
                                "science_disable/O:master_clock_enable:saving_data:taking_event_or_ped:"
                                "fifo_full:fifo_almost_full:fifo_empty:fifo_almost_empty:"
                                "any_waiting:any_waiting_two_hits:any_tmany_thigh:"
                                "packet_type_b2:packet_type_b1:packet_type_b0"                                 );
    t_ped_trigger_tree_out_->Branch("trig_sig_con",       t_ped_trigger.trig_sig_con,      "trig_sig_con[25]/b"    );
    t_ped_trigger_tree_out_->Branch("trig_sig_con_bit",   t_ped_trigger.trig_sig_con_bit.fe_busy,
                                "fe_busy[25]/O:fe_waiting[25]:"
                                "fe_hold_b[25]:fe_tmany_thigh[25]:fe_tout_2[25]:fe_tout_1[25]"                 ); 
    t_ped_trigger_tree_out_->Branch("trig_accepted",      t_ped_trigger.trig_accepted,     "trig_accepted[25]/O"   );
    t_ped_trigger_tree_out_->Branch("trig_rejected",      t_ped_trigger.trig_rejected,     "trig_rejected[25]/O"   );
    t_ped_trigger_tree_out_->Branch("raw_dead",          &t_ped_trigger.raw_dead,          "raw_dead/i"            );
    t_ped_trigger_tree_out_->Branch("dead_ratio",        &t_ped_trigger.dead_ratio,        "dead_ratio/F"          );
    t_ped_trigger_tree_out_->Branch("abs_gps_week",      &t_ped_trigger.abs_gps_week,      "abs_gps_week/I"        );
    t_ped_trigger_tree_out_->Branch("abs_gps_second",    &t_ped_trigger.abs_gps_second,    "abs_gps_second/D"      );
    t_ped_trigger_tree_out_->Branch("abs_gps_valid",     &t_ped_trigger.abs_gps_valid,     "abs_gps_valid/O"       );
    
    return true;
}

void SciTransfer::close_read() {
    if (t_file_in_ == NULL)
        return;

    t_file_in_->Close();
    t_file_in_ = NULL;
    
    t_trigger_tree_in_ = NULL;
    t_modules_tree_in_ = NULL;
    t_ped_trigger_tree_in_ = NULL;
    t_ped_modules_tree_in_ = NULL;
}

void SciTransfer::close_write() {
    if (t_file_in_ == NULL)
        return;
    
    delete t_trigger_tree_out_;
    t_trigger_tree_out_ = NULL;
    
    delete t_modules_tree_out_;
    t_modules_tree_out_ = NULL;
    
    delete t_ped_trigger_tree_out_;
    t_ped_trigger_tree_out_ = NULL;
    
    delete t_ped_modules_tree_out_;
    t_ped_modules_tree_out_ = NULL;
    
    t_file_out_->Close();
    delete t_file_out_;
    t_file_out_ = NULL;
}

void SciTransfer::read_set_start() {
    t_modules_cur_index_ = -1;
    t_modules_reach_end_ = false;
    t_trigger_cur_index_ = -1;
    t_trigger_reach_end_ = false;
    t_ped_modules_cur_index_ = -1;
    t_ped_modules_reach_end_ = false;
    t_ped_trigger_cur_index_ = -1;
    t_ped_trigger_reach_end_ = false;
}

bool SciTransfer::modules_next() {
    if (t_file_in_ == NULL)
        return false;
    if (t_modules_reach_end_)
        return false;
    t_modules_cur_index_++;
    if (t_modules_cur_index_ < t_modules_tot_entries_) {
        t_modules_tree_in_->GetEntry(t_modules_cur_index_);
        return true;
    } else {
        t_modules_reach_end_ = true;
        return false;
    }
}

bool SciTransfer::trigger_next() {
    if (t_file_in_ == NULL)
        return false;
    if (t_trigger_reach_end_)
        return false;
    t_trigger_cur_index_++;
    if (t_trigger_cur_index_ < t_trigger_tot_entries_) {
        t_trigger_tree_in_->GetEntry(t_trigger_cur_index_);
        phy_cur_gps.update6(t_trigger.frm_gps_time);
        phy_cur_timestamp = t_trigger.time_stamp;
        return true;
    } else {
        t_trigger_reach_end_ = true;
        return false;
    }
}

bool SciTransfer::ped_modules_next() {
    if (t_file_in_ == NULL)
        return false;
    if (t_ped_modules_reach_end_)
        return false;
    t_ped_modules_cur_index_++;
    if (t_ped_modules_cur_index_ < t_ped_modules_tot_entries_) {
        t_ped_modules_tree_in_->GetEntry(t_ped_modules_cur_index_);
        return true;
    } else {
        t_ped_modules_reach_end_ = true;
        return false;
    }
}

bool SciTransfer::ped_trigger_next() {
    if (t_file_in_ == NULL)
        return false;
    if (t_ped_trigger_reach_end_)
        return false;
    t_ped_trigger_cur_index_++;
    if (t_ped_trigger_cur_index_ < t_ped_trigger_tot_entries_) {
        t_ped_trigger_tree_in_->GetEntry(t_ped_trigger_cur_index_);
        ped_cur_gps.update6(t_ped_trigger.frm_gps_time);
        ped_cur_timestamp = t_ped_trigger.time_stamp;
        return true;
    } else {
        t_ped_trigger_reach_end_ = true;
        return false;
    }
}

void SciTransfer::modules_fill() {
    if (t_file_out_ == NULL)
        return;
    t_modules_tree_out_->Fill();
}

void SciTransfer::trigger_fill() {
    if (t_file_out_ == NULL)
        return;
    t_trigger_tree_out_->Fill();
}

void SciTransfer::ped_modules_fill() {
    if (t_file_out_ == NULL)
        return;
    t_ped_modules_tree_out_->Fill();
}

void SciTransfer::ped_trigger_fill() {
    if (t_file_out_ == NULL)
        return;
    t_ped_trigger_tree_out_->Fill();
}

void SciTransfer::write_all_tree() {
    if (t_file_out_ == NULL)
        return;
    t_modules_tree_out_->Write();
    t_trigger_tree_out_->Write();
    t_ped_modules_tree_out_->Write();
    t_ped_trigger_tree_out_->Write();
}

void SciTransfer::write_meta(const char* key, const char* value) {
    if (t_file_out_ == NULL)
        return;
    TNamed * cur_meta = new TNamed(key, value);
    cur_meta->Write();
    delete cur_meta;
    cur_meta = NULL;
}
