#include "SciType.hpp"

void SciType::bind_trigger_tree(TTree* t_trigger_tree, Trigger_T& t_trigger) {
    t_trigger_tree->SetBranchAddress("trigg_num",         &t_trigger.trigg_num                   );
    t_trigger_tree->SetBranchAddress("trigg_num_g",       &t_trigger.trigg_num_g                 );
    t_trigger_tree->SetBranchAddress("is_bad",            &t_trigger.is_bad                      );
    t_trigger_tree->SetBranchAddress("pre_is_bad",        &t_trigger.pre_is_bad                  );
    t_trigger_tree->SetBranchAddress("type",              &t_trigger.type                        );
    t_trigger_tree->SetBranchAddress("packet_num",        &t_trigger.packet_num                  );
    t_trigger_tree->SetBranchAddress("time_stamp",        &t_trigger.time_stamp                  );
    t_trigger_tree->SetBranchAddress("time_period",       &t_trigger.time_period                 );
    t_trigger_tree->SetBranchAddress("time_wait",         &t_trigger.time_wait                   );
    t_trigger_tree->SetBranchAddress("time_align",        &t_trigger.time_align                  );
    t_trigger_tree->SetBranchAddress("time_second",       &t_trigger.time_second                 );
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
}

void SciType::bind_modules_tree(TTree* t_modules_tree, Modules_T& t_modules) {
    t_modules_tree->SetBranchAddress("trigg_num",         &t_modules.trigg_num                   );
    t_modules_tree->SetBranchAddress("event_num",         &t_modules.event_num                   );
    t_modules_tree->SetBranchAddress("event_num_g",       &t_modules.event_num_g                 );
    t_modules_tree->SetBranchAddress("is_bad",            &t_modules.is_bad                      );
    t_modules_tree->SetBranchAddress("pre_is_bad",        &t_modules.pre_is_bad                  );
    t_modules_tree->SetBranchAddress("compress",          &t_modules.compress                    );
    t_modules_tree->SetBranchAddress("ct_num",            &t_modules.ct_num                      );
    t_modules_tree->SetBranchAddress("time_stamp",        &t_modules.time_stamp                  );
    t_modules_tree->SetBranchAddress("time_period",       &t_modules.time_period                 );
    t_modules_tree->SetBranchAddress("time_wait",         &t_modules.time_wait                   );
    t_modules_tree->SetBranchAddress("time_align",        &t_modules.time_align                  );
    t_modules_tree->SetBranchAddress("time_second",       &t_modules.time_second                 );
    t_modules_tree->SetBranchAddress("raw_rate",          &t_modules.raw_rate                    );
    t_modules_tree->SetBranchAddress("raw_dead",          &t_modules.raw_dead                    );
    t_modules_tree->SetBranchAddress("dead_ratio",        &t_modules.dead_ratio                  );
    t_modules_tree->SetBranchAddress("status",            &t_modules.status                      );
    t_modules_tree->SetBranchAddress("status_bit",        &t_modules.status_bit.trigger_fe_busy  );
    t_modules_tree->SetBranchAddress("trigger_bit",        t_modules.trigger_bit                 );
    t_modules_tree->SetBranchAddress("energy_adc",         t_modules.energy_adc                  );
    t_modules_tree->SetBranchAddress("common_noise",      &t_modules.common_noise                );
}
