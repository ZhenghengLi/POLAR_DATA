#include "POLEvent.hpp"

void POLEvent::bind_pol_event_tree(TTree* t_pol_event_tree) {
    // from SCI
    t_pol_event_tree->SetBranchAddress("event_id",           &event_id          );
    t_pol_event_tree->SetBranchAddress("event_time",         &event_time        );
    t_pol_event_tree->SetBranchAddress("type",               &type              );
    t_pol_event_tree->SetBranchAddress("is_ped",             &is_ped            );
    t_pol_event_tree->SetBranchAddress("packet_num",         &packet_num        );
    t_pol_event_tree->SetBranchAddress("ct_pre_is_bad",      &ct_pre_is_bad     );
    t_pol_event_tree->SetBranchAddress("fe_pre_is_bad",       fe_pre_is_bad     );
    t_pol_event_tree->SetBranchAddress("ct_time_second",     &ct_time_second    );
    t_pol_event_tree->SetBranchAddress("ct_time_wait",       &ct_time_wait      );
    t_pol_event_tree->SetBranchAddress("ct_dead_ratio",      &ct_dead_ratio     );
    t_pol_event_tree->SetBranchAddress("fe_time_second",      fe_time_second    );
    t_pol_event_tree->SetBranchAddress("fe_time_wait",        fe_time_wait      );
    t_pol_event_tree->SetBranchAddress("fe_dead_ratio",       fe_dead_ratio     );
    t_pol_event_tree->SetBranchAddress("trig_accepted",       trig_accepted     );
    t_pol_event_tree->SetBranchAddress("time_aligned",        time_aligned      );
    t_pol_event_tree->SetBranchAddress("raw_rate",            raw_rate          );
    t_pol_event_tree->SetBranchAddress("pkt_count",          &pkt_count         );
    t_pol_event_tree->SetBranchAddress("lost_count",         &lost_count        );
    t_pol_event_tree->SetBranchAddress("trigger_bit",         trigger_bit       );
    t_pol_event_tree->SetBranchAddress("trigger_n",          &trigger_n         );
    t_pol_event_tree->SetBranchAddress("multiplicity",        multiplicity      );
    t_pol_event_tree->SetBranchAddress("energy_value",        energy_value      );
    t_pol_event_tree->SetBranchAddress("channel_status",      channel_status    );
    t_pol_event_tree->SetBranchAddress("common_noise",        common_noise      );
    t_pol_event_tree->SetBranchAddress("compress",            compress          );
    t_pol_event_tree->SetBranchAddress("dy12_too_high",       dy12_too_high     );
    t_pol_event_tree->SetBranchAddress("t_out_too_many",      t_out_too_many    );
    t_pol_event_tree->SetBranchAddress("t_out_2",             t_out_2           );
    t_pol_event_tree->SetBranchAddress("t_out_1",             t_out_1           );
    // from AUX
    t_pol_event_tree->SetBranchAddress("aux_interval",       &aux_interval      );
    t_pol_event_tree->SetBranchAddress("obox_mode",          &obox_mode         );
    t_pol_event_tree->SetBranchAddress("fe_hv",               fe_hv             );
    t_pol_event_tree->SetBranchAddress("fe_thr",              fe_thr            );
    t_pol_event_tree->SetBranchAddress("fe_temp",             fe_temp           );
    // from PPD
    t_pol_event_tree->SetBranchAddress("ppd_interval",       &ppd_interval      );
    t_pol_event_tree->SetBranchAddress("wgs84_xyz",           wgs84_xyz         );
    t_pol_event_tree->SetBranchAddress("det_z_radec",         det_z_radec       );
    t_pol_event_tree->SetBranchAddress("det_x_radec",         det_x_radec       );
    t_pol_event_tree->SetBranchAddress("earth_radec",         earth_radec       );
    t_pol_event_tree->SetBranchAddress("sun_radec",           sun_radec         );
}

void POLEvent::build_pol_event_tree(TTree* t_pol_event_tree) {
    // from SCI
    t_pol_event_tree->Branch("event_id",           &event_id,          "event_id/L"                 );
    t_pol_event_tree->Branch("event_time",         &event_time,        "event_time/D"               );
    t_pol_event_tree->Branch("type",               &type,              "type/I"                     );
    t_pol_event_tree->Branch("is_ped",             &is_ped,            "is_ped/O"                   );
    t_pol_event_tree->Branch("packet_num",         &packet_num,        "packet_num/I"               );
    t_pol_event_tree->Branch("ct_pre_is_bad",      &ct_pre_is_bad,     "ct_pre_is_bad/I"            );
    t_pol_event_tree->Branch("fe_pre_is_bad",       fe_pre_is_bad,     "fe_pre_is_bad[25]/I"        );
    t_pol_event_tree->Branch("ct_time_second",     &ct_time_second,    "ct_time_second/D"           );
    t_pol_event_tree->Branch("ct_time_wait",       &ct_time_wait,      "ct_time_wait/D"             );
    t_pol_event_tree->Branch("ct_dead_ratio",      &ct_dead_ratio,     "ct_dead_ratio/F"            );
    t_pol_event_tree->Branch("fe_time_second",      fe_time_second,    "fe_time_second[25]/D"       );
    t_pol_event_tree->Branch("fe_time_wait",        fe_time_wait,      "fe_time_wait[25]/D"         );
    t_pol_event_tree->Branch("fe_dead_ratio",       fe_dead_ratio,     "fe_dead_ratio[25]/F"        );
    t_pol_event_tree->Branch("trig_accepted",       trig_accepted,     "trig_accepted[25]/O"        );
    t_pol_event_tree->Branch("time_aligned",        time_aligned,      "time_aligned[25]/O"         );
    t_pol_event_tree->Branch("raw_rate",            raw_rate,          "raw_rate[25]/I"             );
    t_pol_event_tree->Branch("pkt_count",          &pkt_count,         "pkt_count/I"                );
    t_pol_event_tree->Branch("lost_count",         &lost_count,        "lost_count/I"               );
    t_pol_event_tree->Branch("trigger_bit",         trigger_bit,       "trigger_bit[25][64]/O"      );
    t_pol_event_tree->Branch("trigger_n",          &trigger_n,         "trigger_n/I"                );
    t_pol_event_tree->Branch("multiplicity",        multiplicity,      "multiplicity[25]/I"         );
    t_pol_event_tree->Branch("energy_value",        energy_value,      "energy_value[25][64]/F"     );
    t_pol_event_tree->Branch("channel_status",      channel_status,    "channel_status[25][64]/s"   );
    t_pol_event_tree->Branch("common_noise",        common_noise,      "common_noise[25]/F"         );
    t_pol_event_tree->Branch("compress",            compress,          "compress[25]/I"             );
    t_pol_event_tree->Branch("dy12_too_high",       dy12_too_high,     "dy12_too_high[25]/O"        );
    t_pol_event_tree->Branch("t_out_too_many",      t_out_too_many,    "t_out_too_many[25]/O"       );
    t_pol_event_tree->Branch("t_out_2",             t_out_2,           "t_out_2[25]/O"              );
    t_pol_event_tree->Branch("t_out_1",             t_out_1,           "t_out_1[25]/O"              );
    // from AUX
    t_pol_event_tree->Branch("aux_interval",       &aux_interval,      "aux_interval/F"             );
    t_pol_event_tree->Branch("obox_mode",          &obox_mode,         "obox_mode/I"                );
    t_pol_event_tree->Branch("fe_hv",               fe_hv,             "fe_hv[25]/F"                );
    t_pol_event_tree->Branch("fe_thr",              fe_thr,            "fe_thr[25]/F"               );
    t_pol_event_tree->Branch("fe_temp",             fe_temp,           "fe_temp[25]/F"              );
    // from PPD
    t_pol_event_tree->Branch("ppd_interval",       &ppd_interval,      "ppd_interval/F"             );
    t_pol_event_tree->Branch("wgs84_xyz",           wgs84_xyz,         "wgs84_xyz[3]/D"             );
    t_pol_event_tree->Branch("det_z_radec",         det_z_radec,       "det_z_radec[2]/D"           );
    t_pol_event_tree->Branch("det_x_radec",         det_x_radec,       "det_x_radec[2]/D"           );
    t_pol_event_tree->Branch("earth_radec",         earth_radec,       "earth_radec[2]/D"           );
    t_pol_event_tree->Branch("sun_radec",           sun_radec,         "sun_radec[2]/D"             );
}

void POLEvent::deactive_all(TTree* t_pol_event_tree) {
    t_pol_event_tree->SetBranchStatus("*", false);
}

void POLEvent::deactive(TTree* t_pol_event_tree, const char* branch_name) {
    t_pol_event_tree->SetBranchStatus(branch_name, false);
}

void POLEvent::active_all(TTree* t_pol_event_tree) {
    t_pol_event_tree->SetBranchStatus("*", true);
}

void POLEvent::active(TTree* t_pol_event_tree, const char* branch_name) {
    t_pol_event_tree->SetBranchStatus(branch_name, true);
}

Long64_t POLEvent::find_entry(TTree* t_pol_event_tree, double met_time) {
    if (t_pol_event_tree == NULL) return -1;
    t_pol_event_tree->SetBranchStatus("event_time", true);
    Long64_t head_entry = 0;
    Long64_t tail_entry = t_pol_event_tree->GetEntries() - 1;
    t_pol_event_tree->GetEntry(head_entry);
    double first_time = event_time;
    t_pol_event_tree->GetEntry(tail_entry);
    double last_time = event_time;
    if (met_time < first_time) return -1;
    if (met_time > last_time) return -1;
    Long64_t center_entry = 0;
    while (tail_entry - head_entry > 1) {
        center_entry = (head_entry + tail_entry) / 2;
        t_pol_event_tree->GetEntry(center_entry);
        if (met_time > event_time) {
            head_entry = center_entry;
        } else if (met_time < event_time) {
            tail_entry = center_entry;
        } else {
            return center_entry;
        }
    }
    return tail_entry;
}

