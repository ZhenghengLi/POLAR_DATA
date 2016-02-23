#include "HkDataFile.hpp"

using namespace std;

HkDataFile::HkDataFile() {
    t_out_file_ = NULL;
    t_hk_obox_tree_ = NULL;
    t_hk_ibox_tree_ = NULL;
}

HkDataFile::~HkDataFile() {
    
}

bool HkDataFile::open(const char* filename) {
    if (t_out_file_ != NULL)
        return false;
    t_out_file_ = new TFile(filename, "RECREATE");
    if (t_out_file_->IsZombie())
        return false;

    t_hk_obox_tree_ = new TTree("t_hk_obox", "obox housekeeping packets");
    t_hk_obox_tree_->SetDirectory(t_out_file_);
    t_hk_obox_tree_->Branch("odd_index",            &t_hk_obox.odd_index,            "odd_index/I"           );
    t_hk_obox_tree_->Branch("even_index",           &t_hk_obox.even_index,           "even_index/I"          );
    t_hk_obox_tree_->Branch("odd_is_bad",           &t_hk_obox.odd_is_bad,           "odd_is_bad/I"          );
    t_hk_obox_tree_->Branch("even_is_bad",          &t_hk_obox.even_is_bad,          "even_is_bad/I"         );
    t_hk_obox_tree_->Branch("packet_num",           &t_hk_obox.packet_num,           "packet_num/s"          );
    t_hk_obox_tree_->Branch("timestamp",            &t_hk_obox.timestamp,            "timestamp/i"           );
    t_hk_obox_tree_->Branch("obox_mode",            &t_hk_obox.obox_mode,            "obox_mode/s"           );
    t_hk_obox_tree_->Branch("cpu_status",           &t_hk_obox.cpu_status,           "cpu_status/s"          );
    t_hk_obox_tree_->Branch("trig_status",          &t_hk_obox.trig_status,          "trig_status/s"         );
    t_hk_obox_tree_->Branch("comm_status",          &t_hk_obox.comm_status,          "comm_status/s"         );
    t_hk_obox_tree_->Branch("ct_temp",              &t_hk_obox.ct_temp,              "ct_temp/s"             );
    t_hk_obox_tree_->Branch("chain_temp",           &t_hk_obox.chain_temp,           "chain_temp/s"          );
    t_hk_obox_tree_->Branch("reserved",             &t_hk_obox.reserved,             "reserved/s"            );
    t_hk_obox_tree_->Branch("lv_status",            &t_hk_obox.lv_status,            "lv_status/s"           );
    t_hk_obox_tree_->Branch("fe_pattern",           &t_hk_obox.fe_pattern,           "fe_pattern/s"          );
    t_hk_obox_tree_->Branch("hv_pwm",               &t_hk_obox.hv_pwm,               "hv_pwm/s"              );
    t_hk_obox_tree_->Branch("hv_status",            &t_hk_obox.hv_status,            "hv_status/s"           );
    t_hk_obox_tree_->Branch("hv_current",            t_hk_obox.hv_current,           "hv_current[2]/s"       );
    t_hk_obox_tree_->Branch("fe_temp",               t_hk_obox.fe_temp,              "fe_temp[25]/s"         );
    t_hk_obox_tree_->Branch("fe_hv",                 t_hk_obox.fe_hv,                "fe_hv[25]/s"           );
    t_hk_obox_tree_->Branch("fe_thr",                t_hk_obox.fe_thr,               "fe_thr[25]/s"          );
    t_hk_obox_tree_->Branch("fe_rate",               t_hk_obox.fe_rate,              "fe_rate[25]/s"         );
    t_hk_obox_tree_->Branch("fe_cosmic",             t_hk_obox.fe_cosmic,            "fe_cosmic[25]/s"       );
    t_hk_obox_tree_->Branch("flex_i_p3v3",           t_hk_obox.flex_i_p3v3,          "flex_i_p3v3[5]/s"      );
    t_hk_obox_tree_->Branch("flex_i_p1v7",           t_hk_obox.flex_i_p1v7,          "flex_i_p1v7[5]/s"      );
    t_hk_obox_tree_->Branch("flex_i_n2v5",           t_hk_obox.flex_i_n2v5,          "flex_i_n2v5[5]/s"      );
    t_hk_obox_tree_->Branch("flex_v_p3v3",           t_hk_obox.flex_v_p3v3,          "flex_v_p3v3[5]/s"      );
    t_hk_obox_tree_->Branch("flex_v_p1v7",           t_hk_obox.flex_v_p1v7,          "flex_v_p1v7[5]/s"      );
    t_hk_obox_tree_->Branch("flex_v_n2v5",           t_hk_obox.flex_v_n2v5,          "flex_v_n2v5[5]/s"      );
    t_hk_obox_tree_->Branch("hv_v_hot",             &t_hk_obox.hv_v_hot,             "hv_v_hot/s"            );
    t_hk_obox_tree_->Branch("hv_i_hot",             &t_hk_obox.hv_i_hot,             "hv_i_hot/s"            );
    t_hk_obox_tree_->Branch("ct_v_hot",              t_hk_obox.ct_v_hot,             "ct_v_hot[2]/s"         );
    t_hk_obox_tree_->Branch("ct_i_hot",              t_hk_obox.ct_i_hot,             "ct_i_hot[2]/s"         );
    t_hk_obox_tree_->Branch("hv_v_cold",            &t_hk_obox.hv_v_cold,            "hv_v_cold/s"           );
    t_hk_obox_tree_->Branch("hv_i_cold",            &t_hk_obox.hv_i_cold,            "hv_i_cold/s"           );
    t_hk_obox_tree_->Branch("ct_v_cold",             t_hk_obox.ct_v_cold,            "ct_v_cold[2]/s"        );
    t_hk_obox_tree_->Branch("ct_i_cold",             t_hk_obox.ct_i_cold,            "ct_i_cold[2]/s"        );
    t_hk_obox_tree_->Branch("timestamp_sync",       &t_hk_obox.timestamp_sync,       "timestamp_sync/i"      );
    t_hk_obox_tree_->Branch("command_rec",          &t_hk_obox.command_rec,          "command_rec/s"         );
    t_hk_obox_tree_->Branch("command_exec",         &t_hk_obox.command_exec,         "command_exec/s"        );
    t_hk_obox_tree_->Branch("command_last_num",     &t_hk_obox.command_last_num,     "command_last_num/s"    );
    t_hk_obox_tree_->Branch("command_last_stamp",   &t_hk_obox.command_last_stamp,   "command_last_stamp/s"  );
    t_hk_obox_tree_->Branch("command_last_exec",    &t_hk_obox.command_last_exec,    "command_last_exec/s"   );
    t_hk_obox_tree_->Branch("command_last_arg",      t_hk_obox.command_last_arg,     "command_last_arg[2]/s" );
    t_hk_obox_tree_->Branch("obox_hk_crc",          &t_hk_obox.obox_hk_crc,          "obox_hk_crc/s"         );
    t_hk_obox_tree_->Branch("saa",                  &t_hk_obox.saa,                  "saa/s"                 );
    t_hk_obox_tree_->Branch("sci_head",             &t_hk_obox.sci_head,             "sci_head/s"            );
    t_hk_obox_tree_->Branch("gps_pps_count",        &t_hk_obox.gps_pps_count,        "gps_pps_count/l"       );
    t_hk_obox_tree_->Branch("gps_sync_gen_count",   &t_hk_obox.gps_sync_gen_count,   "gps_sync_gen_count/l"  );
    t_hk_obox_tree_->Branch("gps_sync_send_count",  &t_hk_obox.gps_sync_send_count,  "gps_sync_send_count/l" );
    t_hk_obox_tree_->Branch("hk_head",              &t_hk_obox.hk_head,              "hk_head/s"             );
    t_hk_obox_tree_->Branch("hk_tail",              &t_hk_obox.hk_tail,              "hk_tail/s"             );

    t_hk_ibox_tree_ = new TTree("t_hk_ibox", "ibox housekeeping info");
    t_hk_ibox_tree_->SetDirectory(t_out_file_);
    t_hk_ibox_tree_->Branch("frm_index",            &t_hk_ibox.frm_index,            "frm_index/I"           );
    t_hk_ibox_tree_->Branch("pkt_tag",              &t_hk_ibox.pkt_tag,              "pkt_tag/I"             );
    t_hk_ibox_tree_->Branch("is_bad",               &t_hk_ibox.is_bad,               "is_bad/I"              );
    t_hk_ibox_tree_->Branch("ship_time",            &t_hk_ibox.ship_time,            "ship_time/l"           );
    t_hk_ibox_tree_->Branch("error",                 t_hk_ibox.error,                "error[2]/s"            );
    t_hk_ibox_tree_->Branch("frame_head",           &t_hk_ibox.frame_head,           "frame_head/s"          );
    t_hk_ibox_tree_->Branch("command_head",         &t_hk_ibox.command_head,         "command_head/s"        );
    t_hk_ibox_tree_->Branch("command_num",          &t_hk_ibox.command_num,          "command_num/s"         );
    t_hk_ibox_tree_->Branch("command_code",         &t_hk_ibox.command_code,         "command_code/s"        );
    t_hk_ibox_tree_->Branch("command_arg",           t_hk_ibox.command_arg,          "command_arg[2]/s"      );
    t_hk_ibox_tree_->Branch("head",                 &t_hk_ibox.head,                 "head/s"                );
    t_hk_ibox_tree_->Branch("tail",                 &t_hk_ibox.tail,                 "tail/s"                );
    t_hk_ibox_tree_->Branch("ibox_gps",             &t_hk_ibox.ibox_gps,             "ibox_gps/l"            );
    
    return true;
}

void HkDataFile::write_after_decoding() {
    t_hk_obox_tree_->Write();
    t_hk_ibox_tree_->Write();
}

void HkDataFile::close() {
    delete t_hk_obox_tree_;
    t_hk_obox_tree_ = NULL;

    delete t_hk_ibox_tree_;
    t_hk_ibox_tree_ = NULL;

    t_out_file_->Close();
    delete t_out_file_;
    t_out_file_ = NULL;
}

void HkDataFile::write_ibox_info(const HkFrame& frame) {
    copy_ibox_info_(frame);
    t_hk_ibox_tree_->Fill();
}

void HkDataFile::write_two_packet(const HkOdd& odd_pkt, const HkEven even_pkt) {
    if (t_out_file_ == NULL)
        return;
    copy_odd_packet_(odd_pkt);
    copy_even_packet_(even_pkt);
    t_hk_obox_tree_->Fill();
}

void HkDataFile::write_odd_packet_alone(const HkOdd& odd_pkt) {
    if (t_out_file_ == NULL)
        return;
    clear_obox_branch_data_();
    t_hk_obox.even_index = -1;
    t_hk_obox.even_is_bad = 3;
    copy_odd_packet_(odd_pkt);
    t_hk_obox_tree_->Fill();
}

void HkDataFile::write_even_packet_alone(const HkEven& even_pkt) {
    if (t_out_file_ == NULL)
        return;
    clear_obox_branch_data_();
    t_hk_obox.odd_index = -1;
    t_hk_obox.odd_is_bad = 3;
    copy_even_packet_(even_pkt);
    t_hk_obox_tree_->Fill();
}

void HkDataFile::write_meta(const char* key, const char* value) {
    if (t_out_file_ == NULL)
        return;
    TNamed* cur_meta = new TNamed(key, value);
    cur_meta->Write();
    delete cur_meta;
    cur_meta = NULL;
}

void HkDataFile::copy_odd_packet_(const HkOdd& odd_pkt) {
    t_hk_obox.odd_index              = static_cast<Int_t>(odd_pkt.frm_index);
    t_hk_obox.odd_is_bad             = static_cast<Int_t>(odd_pkt.is_bad);
    t_hk_obox.packet_num             = static_cast<UShort_t>(odd_pkt.packet_num);
    t_hk_obox.timestamp              = static_cast<UInt_t>(odd_pkt.timestamp);
    t_hk_obox.obox_mode              = static_cast<UShort_t>(odd_pkt.obox_mode);
    t_hk_obox.cpu_status             = static_cast<UShort_t>(odd_pkt.cpu_status);
    t_hk_obox.trig_status            = static_cast<UShort_t>(odd_pkt.trig_status);
    t_hk_obox.comm_status            = static_cast<UShort_t>(odd_pkt.comm_status);
    t_hk_obox.ct_temp                = static_cast<UShort_t>(odd_pkt.ct_temp);
    t_hk_obox.chain_temp             = static_cast<UShort_t>(odd_pkt.chain_temp);
    t_hk_obox.reserved               = static_cast<UShort_t>(odd_pkt.reserved);
    t_hk_obox.lv_status              = static_cast<UShort_t>(odd_pkt.lv_status);
    t_hk_obox.fe_pattern             = static_cast<UShort_t>(odd_pkt.fe_pattern);
    t_hk_obox.hv_pwm                 = static_cast<UShort_t>(odd_pkt.hv_pwm);
    t_hk_obox.hv_current[0]          = static_cast<UShort_t>(odd_pkt.hv_current[0]);
    t_hk_obox.hv_current[1]          = static_cast<UShort_t>(odd_pkt.hv_current[1]);
    for (int i = 0; i < 18; i++) {
        t_hk_obox.fe_temp[i]         = static_cast<UShort_t>(odd_pkt.fe_temp[i]);
        t_hk_obox.fe_hv[i]           = static_cast<UShort_t>(odd_pkt.fe_hv[i]);
        t_hk_obox.fe_thr[i]          = static_cast<UShort_t>(odd_pkt.fe_thr[i]);
        t_hk_obox.fe_rate[i]         = static_cast<UShort_t>(odd_pkt.fe_rate[i]);
        t_hk_obox.fe_cosmic[i]       = static_cast<UShort_t>(odd_pkt.fe_cosmic[i]);
    }
}

void HkDataFile::copy_even_packet_(const HkEven& even_pkt) {
    t_hk_obox.even_index             = static_cast<Int_t>(even_pkt.frm_index);
    t_hk_obox.even_is_bad            = static_cast<Int_t>(even_pkt.is_bad);
    for (int i = 0; i < 7; i++) {
        t_hk_obox.fe_temp[i]         = static_cast<UShort_t>(even_pkt.fe_temp[i]);
        t_hk_obox.fe_hv[i]           = static_cast<UShort_t>(even_pkt.fe_hv[i]);
        t_hk_obox.fe_thr[i]          = static_cast<UShort_t>(even_pkt.fe_thr[i]);
        t_hk_obox.fe_rate[i]         = static_cast<UShort_t>(even_pkt.fe_rate[i]);
        t_hk_obox.fe_cosmic[i]       = static_cast<UShort_t>(even_pkt.fe_cosmic[i]);
    }
    for (int i = 0; i < 5; i++) {
        t_hk_obox.flex_i_p3v3[i]     = static_cast<UShort_t>(even_pkt.flex_i_p3v3[i]);
        t_hk_obox.flex_i_p1v7[i]     = static_cast<UShort_t>(even_pkt.flex_i_p1v7[i]);
        t_hk_obox.flex_i_n2v5[i]     = static_cast<UShort_t>(even_pkt.flex_i_n2v5[i]);
        t_hk_obox.flex_v_p3v3[i]     = static_cast<UShort_t>(even_pkt.flex_v_p3v3[i]);
        t_hk_obox.flex_v_p1v7[i]     = static_cast<UShort_t>(even_pkt.flex_v_p1v7[i]);
        t_hk_obox.flex_v_n2v5[i]     = static_cast<UShort_t>(even_pkt.flex_v_n2v5[i]);
    }
    t_hk_obox.hv_v_hot               = static_cast<UShort_t>(even_pkt.hv_v_hot);
    t_hk_obox.hv_i_hot               = static_cast<UShort_t>(even_pkt.hv_i_hot);
    t_hk_obox.hv_v_cold              = static_cast<UShort_t>(even_pkt.hv_v_cold);
    t_hk_obox.hv_i_cold              = static_cast<UShort_t>(even_pkt.hv_i_cold);
    for (int i = 0; i < 2; i++) {
        t_hk_obox.ct_v_hot[i]        = static_cast<UShort_t>(even_pkt.ct_v_hot[i]);
        t_hk_obox.ct_i_hot[i]        = static_cast<UShort_t>(even_pkt.ct_i_hot[i]);
        t_hk_obox.ct_v_cold[i]       = static_cast<UShort_t>(even_pkt.ct_v_cold[i]);
        t_hk_obox.ct_i_cold[i]       = static_cast<UShort_t>(even_pkt.ct_i_cold[i]);
    }
    t_hk_obox.timestamp_sync         = static_cast<UInt_t>(even_pkt.timestamp_sync);
    t_hk_obox.command_rec            = static_cast<UShort_t>(even_pkt.command_rec);
    t_hk_obox.command_exec           = static_cast<UShort_t>(even_pkt.command_exec);
    t_hk_obox.command_last_num       = static_cast<UShort_t>(even_pkt.command_last_num);
    t_hk_obox.command_last_stamp     = static_cast<UShort_t>(even_pkt.command_last_stamp);
    t_hk_obox.command_last_exec      = static_cast<UShort_t>(even_pkt.command_last_exec);
    t_hk_obox.command_last_arg[0]    = static_cast<UShort_t>(even_pkt.command_last_arg[0]);
    t_hk_obox.command_last_arg[1]    = static_cast<UShort_t>(even_pkt.command_last_arg[1]);
    t_hk_obox.obox_hk_crc            = static_cast<UShort_t>(even_pkt.obox_hk_crc);
    t_hk_obox.saa                    = static_cast<UShort_t>(even_pkt.saa);
    t_hk_obox.sci_head               = static_cast<UShort_t>(even_pkt.sci_head);
    t_hk_obox.gps_pps_count          = static_cast<ULong64_t>(even_pkt.gps_pps_count);
    t_hk_obox.gps_sync_gen_count     = static_cast<ULong64_t>(even_pkt.gps_sync_gen_count);
    t_hk_obox.gps_sync_send_count    = static_cast<ULong64_t>(even_pkt.gps_sync_send_count);
    t_hk_obox.hk_head                = static_cast<UShort_t>(even_pkt.hk_head);
    t_hk_obox.hk_tail                = static_cast<UShort_t>(even_pkt.hk_tail);
}

void HkDataFile::copy_ibox_info_(const HkFrame& frame) {
    t_hk_ibox.frm_index              = static_cast<Int_t>(frame.get_index());
    t_hk_ibox.pkt_tag                = static_cast<Int_t>(frame.get_pkt_tag());
    t_hk_ibox.is_bad                 = static_cast<Int_t>(frame.is_bad);
    t_hk_ibox.ship_time              = static_cast<ULong64_t>(frame.ship_time);
    t_hk_ibox.error[0]               = static_cast<UShort_t>(frame.error[0]);
    t_hk_ibox.error[1]               = static_cast<UShort_t>(frame.error[1]);
    t_hk_ibox.frame_head             = static_cast<UShort_t>(frame.frame_head);
    t_hk_ibox.command_head           = static_cast<UShort_t>(frame.command_head);
    t_hk_ibox.command_num            = static_cast<UShort_t>(frame.command_num);
    t_hk_ibox.command_code           = static_cast<UShort_t>(frame.command_code);
    t_hk_ibox.command_arg[0]         = static_cast<UShort_t>(frame.command_arg[0]);
    t_hk_ibox.command_arg[1]         = static_cast<UShort_t>(frame.command_arg[1]);
    t_hk_ibox.head                   = static_cast<UShort_t>(frame.head);
    t_hk_ibox.tail                   = static_cast<UShort_t>(frame.tail);
    t_hk_ibox.ibox_gps               = static_cast<ULong64_t>(frame.ibox_gps);
}

void HkDataFile::clear_obox_branch_data_() {
    t_hk_obox.odd_index = 0;
    t_hk_obox.even_index = 0;
    t_hk_obox.odd_is_bad = 0;
    t_hk_obox.even_is_bad = 0;
}
