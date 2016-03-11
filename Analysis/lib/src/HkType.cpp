#include "HkType.hpp"

void HkType::bind_hk_obox_tree(TTree* t_hk_obox_tree, Hk_Obox_T& t_hk_obox) {
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
}

void HkType::bind_hk_ibox_tree(TTree* t_hk_ibox_tree, Hk_Ibox_T& t_hk_ibox) {
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
}

