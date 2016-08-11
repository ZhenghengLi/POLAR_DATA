#include "HkType1M.hpp"

void HkType1M::bind_hk_obox_tree(TTree* t_hk_obox_tree, Hk_Obox_T& t_hk_obox) {
    t_hk_obox_tree->SetBranchAddress("odd_index",            &t_hk_obox.odd_index            );
    t_hk_obox_tree->SetBranchAddress("even_index",           &t_hk_obox.even_index           );
    t_hk_obox_tree->SetBranchAddress("odd_is_bad",           &t_hk_obox.odd_is_bad           );
    t_hk_obox_tree->SetBranchAddress("even_is_bad",          &t_hk_obox.even_is_bad          );
    t_hk_obox_tree->SetBranchAddress("obox_is_bad",          &t_hk_obox.obox_is_bad          );
    t_hk_obox_tree->SetBranchAddress("packet_num",           &t_hk_obox.packet_num           );
    t_hk_obox_tree->SetBranchAddress("timestamp",            &t_hk_obox.timestamp            );
    t_hk_obox_tree->SetBranchAddress("obox_mode",            &t_hk_obox.obox_mode            );
    t_hk_obox_tree->SetBranchAddress("cpu_status",           &t_hk_obox.cpu_status           );
    t_hk_obox_tree->SetBranchAddress("trig_status",          &t_hk_obox.trig_status          );
    t_hk_obox_tree->SetBranchAddress("comm_status",          &t_hk_obox.comm_status          );
    t_hk_obox_tree->SetBranchAddress("ct_temp",              &t_hk_obox.ct_temp              );
    t_hk_obox_tree->SetBranchAddress("chain_temp",           &t_hk_obox.chain_temp           );
    t_hk_obox_tree->SetBranchAddress("reserved",             &t_hk_obox.reserved             );
    t_hk_obox_tree->SetBranchAddress("lv_status",            &t_hk_obox.lv_status            );
    t_hk_obox_tree->SetBranchAddress("fe_pattern",           &t_hk_obox.fe_pattern           );
    t_hk_obox_tree->SetBranchAddress("lv_temp",              &t_hk_obox.lv_temp              );
    t_hk_obox_tree->SetBranchAddress("hv_pwm",               &t_hk_obox.hv_pwm               );
    t_hk_obox_tree->SetBranchAddress("hv_status",            &t_hk_obox.hv_status            );
    t_hk_obox_tree->SetBranchAddress("hv_current",            t_hk_obox.hv_current           );
    t_hk_obox_tree->SetBranchAddress("fe_status",             t_hk_obox.fe_status            );
    t_hk_obox_tree->SetBranchAddress("fe_temp",               t_hk_obox.fe_temp              );
    t_hk_obox_tree->SetBranchAddress("fe_hv",                 t_hk_obox.fe_hv                );
    t_hk_obox_tree->SetBranchAddress("fe_thr",                t_hk_obox.fe_thr               );
    t_hk_obox_tree->SetBranchAddress("fe_rate",               t_hk_obox.fe_rate              );
    t_hk_obox_tree->SetBranchAddress("fe_cosmic",             t_hk_obox.fe_cosmic            );
    t_hk_obox_tree->SetBranchAddress("flex_i_p3v3",           t_hk_obox.flex_i_p3v3          );
    t_hk_obox_tree->SetBranchAddress("flex_i_p1v7",           t_hk_obox.flex_i_p1v7          );
    t_hk_obox_tree->SetBranchAddress("flex_i_n2v5",           t_hk_obox.flex_i_n2v5          );
    t_hk_obox_tree->SetBranchAddress("flex_v_p3v3",           t_hk_obox.flex_v_p3v3          );
    t_hk_obox_tree->SetBranchAddress("flex_v_p1v7",           t_hk_obox.flex_v_p1v7          );
    t_hk_obox_tree->SetBranchAddress("flex_v_n2v5",           t_hk_obox.flex_v_n2v5          );
    t_hk_obox_tree->SetBranchAddress("hv_v_hot",             &t_hk_obox.hv_v_hot             );
    t_hk_obox_tree->SetBranchAddress("hv_i_hot",             &t_hk_obox.hv_i_hot             );
    t_hk_obox_tree->SetBranchAddress("ct_v_hot",              t_hk_obox.ct_v_hot             );
    t_hk_obox_tree->SetBranchAddress("ct_i_hot",              t_hk_obox.ct_i_hot             );
    t_hk_obox_tree->SetBranchAddress("hv_v_cold",            &t_hk_obox.hv_v_cold            );
    t_hk_obox_tree->SetBranchAddress("hv_i_cold",            &t_hk_obox.hv_i_cold            );
    t_hk_obox_tree->SetBranchAddress("ct_v_cold",             t_hk_obox.ct_v_cold            );
    t_hk_obox_tree->SetBranchAddress("ct_i_cold",             t_hk_obox.ct_i_cold            );
    t_hk_obox_tree->SetBranchAddress("timestamp_sync",       &t_hk_obox.timestamp_sync       );
    t_hk_obox_tree->SetBranchAddress("command_rec",          &t_hk_obox.command_rec          );
    t_hk_obox_tree->SetBranchAddress("command_exec",         &t_hk_obox.command_exec         );
    t_hk_obox_tree->SetBranchAddress("command_last_num",     &t_hk_obox.command_last_num     );
    t_hk_obox_tree->SetBranchAddress("command_last_stamp",   &t_hk_obox.command_last_stamp   );
    t_hk_obox_tree->SetBranchAddress("command_last_exec",    &t_hk_obox.command_last_exec    );
    t_hk_obox_tree->SetBranchAddress("command_last_arg",      t_hk_obox.command_last_arg     );
    t_hk_obox_tree->SetBranchAddress("obox_hk_crc",          &t_hk_obox.obox_hk_crc          );
    t_hk_obox_tree->SetBranchAddress("saa",                  &t_hk_obox.saa                  );
    t_hk_obox_tree->SetBranchAddress("sci_head",             &t_hk_obox.sci_head             );
    t_hk_obox_tree->SetBranchAddress("gps_pps_count",        &t_hk_obox.gps_pps_count        );
    t_hk_obox_tree->SetBranchAddress("gps_sync_gen_count",   &t_hk_obox.gps_sync_gen_count   );
    t_hk_obox_tree->SetBranchAddress("gps_sync_send_count",  &t_hk_obox.gps_sync_send_count  );
    t_hk_obox_tree->SetBranchAddress("ibox_gps",             &t_hk_obox.ibox_gps             );
    t_hk_obox_tree->SetBranchAddress("abs_gps_week",         &t_hk_obox.abs_gps_week         );
    t_hk_obox_tree->SetBranchAddress("abs_gps_second",       &t_hk_obox.abs_gps_second       );
    t_hk_obox_tree->SetBranchAddress("ship_time",            &t_hk_obox.ship_time            );
    t_hk_obox_tree->SetBranchAddress("abs_ship_second",      &t_hk_obox.abs_ship_second      );
}

void HkType1M::bind_hk_ibox_tree(TTree* t_hk_ibox_tree, Hk_Ibox_T& t_hk_ibox) {
    t_hk_ibox_tree->SetBranchAddress("frm_index",            &t_hk_ibox.frm_index            );
    t_hk_ibox_tree->SetBranchAddress("pkt_tag",              &t_hk_ibox.pkt_tag              );
    t_hk_ibox_tree->SetBranchAddress("is_bad",               &t_hk_ibox.is_bad               );
    t_hk_ibox_tree->SetBranchAddress("ship_time",            &t_hk_ibox.ship_time            );
    t_hk_ibox_tree->SetBranchAddress("error",                 t_hk_ibox.error                );
    t_hk_ibox_tree->SetBranchAddress("frame_head",           &t_hk_ibox.frame_head           );
    t_hk_ibox_tree->SetBranchAddress("command_head",         &t_hk_ibox.command_head         );
    t_hk_ibox_tree->SetBranchAddress("command_num",          &t_hk_ibox.command_num          );
    t_hk_ibox_tree->SetBranchAddress("command_code",         &t_hk_ibox.command_code         );
    t_hk_ibox_tree->SetBranchAddress("command_arg",           t_hk_ibox.command_arg          );
    t_hk_ibox_tree->SetBranchAddress("head",                 &t_hk_ibox.head                 );
    t_hk_ibox_tree->SetBranchAddress("tail",                 &t_hk_ibox.tail                 );
    t_hk_ibox_tree->SetBranchAddress("ibox_gps",             &t_hk_ibox.ibox_gps             );
    t_hk_ibox_tree->SetBranchAddress("abs_gps_week",         &t_hk_ibox.abs_gps_week         );
    t_hk_ibox_tree->SetBranchAddress("abs_gps_second",       &t_hk_ibox.abs_gps_second       );
    t_hk_ibox_tree->SetBranchAddress("abs_ship_second",      &t_hk_ibox.abs_ship_second      );
}

void HkType1M::build_hk_obox_tree(TTree* t_hk_obox_tree, Hk_Obox_T& t_hk_obox) {
    t_hk_obox_tree->Branch("odd_index",            &t_hk_obox.odd_index,            "odd_index/I"           );
    t_hk_obox_tree->Branch("even_index",           &t_hk_obox.even_index,           "even_index/I"          );
    t_hk_obox_tree->Branch("odd_is_bad",           &t_hk_obox.odd_is_bad,           "odd_is_bad/I"          );
    t_hk_obox_tree->Branch("even_is_bad",          &t_hk_obox.even_is_bad,          "even_is_bad/I"         );
    t_hk_obox_tree->Branch("obox_is_bad",          &t_hk_obox.obox_is_bad,          "obox_is_bad/I"         );
    t_hk_obox_tree->Branch("packet_num",           &t_hk_obox.packet_num,           "packet_num/s"          );
    t_hk_obox_tree->Branch("timestamp",            &t_hk_obox.timestamp,            "timestamp/i"           );
    t_hk_obox_tree->Branch("obox_mode",            &t_hk_obox.obox_mode,            "obox_mode/b"           );
    t_hk_obox_tree->Branch("cpu_status",           &t_hk_obox.cpu_status,           "cpu_status/s"          );
    t_hk_obox_tree->Branch("trig_status",          &t_hk_obox.trig_status,          "trig_status/b"         );
    t_hk_obox_tree->Branch("comm_status",          &t_hk_obox.comm_status,          "comm_status/b"         );
    t_hk_obox_tree->Branch("ct_temp",              &t_hk_obox.ct_temp,              "ct_temp/F"             );
    t_hk_obox_tree->Branch("chain_temp",           &t_hk_obox.chain_temp,           "chain_temp/F"          );
    t_hk_obox_tree->Branch("reserved",             &t_hk_obox.reserved,             "reserved/s"            );
    t_hk_obox_tree->Branch("lv_status",            &t_hk_obox.lv_status,            "lv_status/s"           );
    t_hk_obox_tree->Branch("fe_pattern",           &t_hk_obox.fe_pattern,           "fe_pattern/i"          );
    t_hk_obox_tree->Branch("lv_temp",              &t_hk_obox.lv_temp,              "lv_temp/F"             );
    t_hk_obox_tree->Branch("hv_pwm",               &t_hk_obox.hv_pwm,               "hv_pwm/s"              );
    t_hk_obox_tree->Branch("hv_status",            &t_hk_obox.hv_status,            "hv_status/s"           );
    t_hk_obox_tree->Branch("hv_current",            t_hk_obox.hv_current,           "hv_current[2]/s"       );
    t_hk_obox_tree->Branch("fe_status",             t_hk_obox.fe_status,            "fe_status[25]/b"       );
    t_hk_obox_tree->Branch("fe_temp",               t_hk_obox.fe_temp,              "fe_temp[25]/F"         );
    t_hk_obox_tree->Branch("fe_hv",                 t_hk_obox.fe_hv,                "fe_hv[25]/F"           );
    t_hk_obox_tree->Branch("fe_thr",                t_hk_obox.fe_thr,               "fe_thr[25]/F"          );
    t_hk_obox_tree->Branch("fe_rate",               t_hk_obox.fe_rate,              "fe_rate[25]/s"         );
    t_hk_obox_tree->Branch("fe_cosmic",             t_hk_obox.fe_cosmic,            "fe_cosmic[25]/s"       );
    t_hk_obox_tree->Branch("flex_i_p3v3",           t_hk_obox.flex_i_p3v3,          "flex_i_p3v3[5]/F"      );
    t_hk_obox_tree->Branch("flex_i_p1v7",           t_hk_obox.flex_i_p1v7,          "flex_i_p1v7[5]/F"      );
    t_hk_obox_tree->Branch("flex_i_n2v5",           t_hk_obox.flex_i_n2v5,          "flex_i_n2v5[5]/F"      );
    t_hk_obox_tree->Branch("flex_v_p3v3",           t_hk_obox.flex_v_p3v3,          "flex_v_p3v3[5]/F"      );
    t_hk_obox_tree->Branch("flex_v_p1v7",           t_hk_obox.flex_v_p1v7,          "flex_v_p1v7[5]/F"      );
    t_hk_obox_tree->Branch("flex_v_n2v5",           t_hk_obox.flex_v_n2v5,          "flex_v_n2v5[5]/F"      );
    t_hk_obox_tree->Branch("hv_v_hot",             &t_hk_obox.hv_v_hot,             "hv_v_hot/F"            );
    t_hk_obox_tree->Branch("hv_i_hot",             &t_hk_obox.hv_i_hot,             "hv_i_hot/F"            );
    t_hk_obox_tree->Branch("ct_v_hot",              t_hk_obox.ct_v_hot,             "ct_v_hot[2]/F"         );
    t_hk_obox_tree->Branch("ct_i_hot",              t_hk_obox.ct_i_hot,             "ct_i_hot[2]/F"         );
    t_hk_obox_tree->Branch("hv_v_cold",            &t_hk_obox.hv_v_cold,            "hv_v_cold/F"           );
    t_hk_obox_tree->Branch("hv_i_cold",            &t_hk_obox.hv_i_cold,            "hv_i_cold/F"           );
    t_hk_obox_tree->Branch("ct_v_cold",             t_hk_obox.ct_v_cold,            "ct_v_cold[2]/F"        );
    t_hk_obox_tree->Branch("ct_i_cold",             t_hk_obox.ct_i_cold,            "ct_i_cold[2]/F"        );
    t_hk_obox_tree->Branch("timestamp_sync",       &t_hk_obox.timestamp_sync,       "timestamp_sync/i"      );
    t_hk_obox_tree->Branch("command_rec",          &t_hk_obox.command_rec,          "command_rec/s"         );
    t_hk_obox_tree->Branch("command_exec",         &t_hk_obox.command_exec,         "command_exec/s"        );
    t_hk_obox_tree->Branch("command_last_num",     &t_hk_obox.command_last_num,     "command_last_num/s"    );
    t_hk_obox_tree->Branch("command_last_stamp",   &t_hk_obox.command_last_stamp,   "command_last_stamp/s"  );
    t_hk_obox_tree->Branch("command_last_exec",    &t_hk_obox.command_last_exec,    "command_last_exec/s"   );
    t_hk_obox_tree->Branch("command_last_arg",      t_hk_obox.command_last_arg,     "command_last_arg[2]/s" );
    t_hk_obox_tree->Branch("obox_hk_crc",          &t_hk_obox.obox_hk_crc,          "obox_hk_crc/s"         );
    t_hk_obox_tree->Branch("saa",                  &t_hk_obox.saa,                  "saa/s"                 );
    t_hk_obox_tree->Branch("sci_head",             &t_hk_obox.sci_head,             "sci_head/s"            );
    t_hk_obox_tree->Branch("gps_pps_count",        &t_hk_obox.gps_pps_count,        "gps_pps_count/l"       );
    t_hk_obox_tree->Branch("gps_sync_gen_count",   &t_hk_obox.gps_sync_gen_count,   "gps_sync_gen_count/l"  );
    t_hk_obox_tree->Branch("gps_sync_send_count",  &t_hk_obox.gps_sync_send_count,  "gps_sync_send_count/l" );
    t_hk_obox_tree->Branch("ibox_gps",             &t_hk_obox.ibox_gps,             "ibox_gps/l"            );
    t_hk_obox_tree->Branch("abs_gps_week",         &t_hk_obox.abs_gps_week,         "abs_gps_week/I"        );
    t_hk_obox_tree->Branch("abs_gps_second",       &t_hk_obox.abs_gps_second,       "abs_gps_second/D"      );
    t_hk_obox_tree->Branch("ship_time",            &t_hk_obox.ship_time,            "ship_time/l"           );
    t_hk_obox_tree->Branch("abs_ship_second",      &t_hk_obox.abs_ship_second,      "abs_ship_second/D"     );
}

void HkType1M::build_hk_ibox_tree(TTree* t_hk_ibox_tree, Hk_Ibox_T& t_hk_ibox) {
    t_hk_ibox_tree->Branch("frm_index",            &t_hk_ibox.frm_index,            "frm_index/I"           );
    t_hk_ibox_tree->Branch("pkt_tag",              &t_hk_ibox.pkt_tag,              "pkt_tag/I"             );
    t_hk_ibox_tree->Branch("is_bad",               &t_hk_ibox.is_bad,               "is_bad/I"              );
    t_hk_ibox_tree->Branch("ship_time",            &t_hk_ibox.ship_time,            "ship_time/l"           );
    t_hk_ibox_tree->Branch("error",                 t_hk_ibox.error,                "error[2]/s"            );
    t_hk_ibox_tree->Branch("frame_head",           &t_hk_ibox.frame_head,           "frame_head/s"          );
    t_hk_ibox_tree->Branch("command_head",         &t_hk_ibox.command_head,         "command_head/s"        );
    t_hk_ibox_tree->Branch("command_num",          &t_hk_ibox.command_num,          "command_num/s"         );
    t_hk_ibox_tree->Branch("command_code",         &t_hk_ibox.command_code,         "command_code/s"        );
    t_hk_ibox_tree->Branch("command_arg",           t_hk_ibox.command_arg,          "command_arg[2]/s"      );
    t_hk_ibox_tree->Branch("head",                 &t_hk_ibox.head,                 "head/s"                );
    t_hk_ibox_tree->Branch("tail",                 &t_hk_ibox.tail,                 "tail/s"                );
    t_hk_ibox_tree->Branch("ibox_gps",             &t_hk_ibox.ibox_gps,             "ibox_gps/l"            );
    t_hk_ibox_tree->Branch("abs_gps_week",         &t_hk_ibox.abs_gps_week,         "abs_gps_week/I"        );
    t_hk_ibox_tree->Branch("abs_gps_second",       &t_hk_ibox.abs_gps_second,       "abs_gps_second/D"      );
    t_hk_ibox_tree->Branch("abs_ship_second",      &t_hk_ibox.abs_ship_second,      "abs_ship_second/D"     );
}
