#include "SAPDataFile.hpp"

SAPDataFile::SAPDataFile() {
    t_file_out_ = NULL;
    t_pol_event_tree_ = NULL;
}

SAPDataFile::~SAPDataFile() {
    if (t_file_out_ != NULL)
        close();
}

bool SAPDataFile::open(const char* filename) {
    if (t_file_out_ != NULL)
        return false;
    t_file_out_ = new TFile(filename, "RECREATE");
    if (t_file_out_->IsZombie())
        return false;
    t_file_out_->cd();
    t_pol_event_tree_ = new TTree("t_pol_event", "POLAR event data");
    // from SCI
    t_pol_event_tree_->Branch("event_id",           &t_pol_event.event_id,          "event_id/L"                 );
    t_pol_event_tree_->Branch("event_time",         &t_pol_event.event_time,        "event_time/D"               );
    t_pol_event_tree_->Branch("type",               &t_pol_event.type,              "type/I"                     );
    t_pol_event_tree_->Branch("is_ped",             &t_pol_event.is_ped,            "is_ped/O"                   );
    t_pol_event_tree_->Branch("packet_num",         &t_pol_event.packet_num,        "packet_num/I"               );
    t_pol_event_tree_->Branch("ct_pre_is_bad",      &t_pol_event.ct_pre_is_bad,     "ct_pre_is_bad/I"            );
    t_pol_event_tree_->Branch("fe_pre_is_bad",       t_pol_event.fe_pre_is_bad,     "fe_pre_is_bad[25]/I"        );
    t_pol_event_tree_->Branch("ct_time_second",     &t_pol_event.ct_time_second,    "ct_time_second/D"           );
    t_pol_event_tree_->Branch("ct_time_wait",       &t_pol_event.ct_time_wait,      "ct_time_wait/D"             );
    t_pol_event_tree_->Branch("ct_dead_ratio",      &t_pol_event.ct_dead_ratio,     "ct_dead_ratio/F"            );
    t_pol_event_tree_->Branch("fe_time_second",      t_pol_event.fe_time_second,    "fe_time_second[25]/D"       );
    t_pol_event_tree_->Branch("fe_time_wait",        t_pol_event.fe_time_wait,      "fe_time_wait[25]/D"         );
    t_pol_event_tree_->Branch("fe_dead_ratio",       t_pol_event.fe_dead_ratio,     "fe_dead_ratio[25]/F"        );
    t_pol_event_tree_->Branch("trig_accepted",       t_pol_event.trig_accepted,     "trig_accepted[25]/O"        );
    t_pol_event_tree_->Branch("time_aligned",        t_pol_event.time_aligned,      "time_aligned[25]/O"         );
    t_pol_event_tree_->Branch("raw_rate",            t_pol_event.raw_rate,          "raw_rate[25]/I"             );
    t_pol_event_tree_->Branch("pkt_count",          &t_pol_event.pkt_count,         "pkt_count/I"                );
    t_pol_event_tree_->Branch("lost_count",         &t_pol_event.lost_count,        "lost_count/I"               );
    t_pol_event_tree_->Branch("trigger_bit",         t_pol_event.trigger_bit,       "trigger_bit[25][64]/O"      );
    t_pol_event_tree_->Branch("trigger_n",          &t_pol_event.trigger_n,         "trigger_n/I"                );
    t_pol_event_tree_->Branch("multiplicity",        t_pol_event.multiplicity,      "multiplicity[25]/I"         );
    t_pol_event_tree_->Branch("energy_value",        t_pol_event.energy_value,      "energy_value[25][64]/F"     );
    t_pol_event_tree_->Branch("channel_status",      t_pol_event.channel_status,    "channel_status[25][64]/s"   );
    t_pol_event_tree_->Branch("common_noise",        t_pol_event.common_noise,      "common_noise[25]/F"         );
    t_pol_event_tree_->Branch("compress",            t_pol_event.compress,          "compress[25]/I"             );
    t_pol_event_tree_->Branch("dy12_too_high",       t_pol_event.dy12_too_high,     "dy12_too_high[25]/O"        );
    t_pol_event_tree_->Branch("t_out_too_many",      t_pol_event.t_out_too_many,    "t_out_too_many[25]/O"       );
    t_pol_event_tree_->Branch("t_out_2",             t_pol_event.t_out_2,           "t_out_2[25]/O"              );
    t_pol_event_tree_->Branch("t_out_1",             t_pol_event.t_out_1,           "t_out_1[25]/O"              );
    // from AUX
    t_pol_event_tree_->Branch("obox_mode",          &t_pol_event.obox_mode,         "obox_mode/I"                );
    t_pol_event_tree_->Branch("fe_hv",               t_pol_event.fe_hv,             "fe_hv[25]/F"                );
    t_pol_event_tree_->Branch("fe_thr",              t_pol_event.fe_thr,            "fe_thr[25]/F"               );
    t_pol_event_tree_->Branch("fe_temp",             t_pol_event.fe_temp,           "fe_temp[25]/F"              );
    // from PPD
    t_pol_event_tree_->Branch("wgs84_xyz",           t_pol_event.wgs84_xyz,         "wgs84_xyz[3]/D"             );
    t_pol_event_tree_->Branch("det_z_radec",         t_pol_event.det_z_radec,       "det_z_radec[2]/D"           );
    t_pol_event_tree_->Branch("det_x_radec",         t_pol_event.det_x_radec,       "det_x_radec[2]/D"           );
    t_pol_event_tree_->Branch("earth_radec",         t_pol_event.earth_radec,       "earth_radec[2]/D"           );
    t_pol_event_tree_->Branch("sun_radec",           t_pol_event.sun_radec,         "sun_radec[2]/D"             );

    return true;
}

void SAPDataFile::close() {
    if (t_file_out_ == NULL)
        return;
    delete t_pol_event_tree_;
    t_pol_event_tree_ = NULL;
    t_file_out_->Close();
    delete t_file_out_;
    t_file_out_ = NULL;
}

void SAPDataFile::fill_data() {
    if (t_file_out_ == NULL)
        return;
    if (t_pol_event_tree_ == NULL)
        return;
    t_pol_event_tree_->Fill();
}

void SAPDataFile::write_tree() {
    if (t_file_out_ == NULL)
        return;
    if (t_pol_event_tree_ == NULL)
        return;
    t_pol_event_tree_->Write();
}

void SAPDataFile::write_meta(const char* key, const char* value) {
    if (t_file_out_ == NULL)
        return;
    TNamed* cur_meta = new TNamed(key, value);
    cur_meta->Write();
    delete cur_meta;
    cur_meta = NULL;
}

void SAPDataFile::clear_data() {
    // from SCI
    t_pol_event.event_id                              = -1;
    t_pol_event.event_time                            = -1;
    t_pol_event.type                                  = -1;
    t_pol_event.is_ped                                = false;
    t_pol_event.packet_num                            = -1;
    t_pol_event.ct_pre_is_bad                         = false;
    for (int i = 0; i < 25; i++) {
        t_pol_event.fe_pre_is_bad[i]                  = false;
    }
    t_pol_event.ct_time_second                        = -1;
    t_pol_event.ct_time_wait                          = -1;
    t_pol_event.ct_dead_ratio                         = -1;
    for (int i = 0; i < 25; i++) {
        t_pol_event.fe_time_second[i]                 = -1;
        t_pol_event.fe_time_wait[i]                   = -1;
        t_pol_event.fe_dead_ratio[i]                  = -1;
        t_pol_event.trig_accepted[i]                  = false;
        t_pol_event.time_aligned[i]                   = false;
        t_pol_event.raw_rate[i]                       = -1;
    }
    t_pol_event.pkt_count                             = -1;
    t_pol_event.lost_count                            = -1;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            t_pol_event.trigger_bit[i][j]             = false;
        }
    }
    t_pol_event.trigger_n                             = -1;
    for (int i = 0; i < 25; i++) {
        t_pol_event.multiplicity[i]                   = -1;
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            t_pol_event.energy_value[i][j]            = -1;
            t_pol_event.channel_status[i][j]          = 0;
        }
    }
    for (int i = 0; i < 25; i++) {
        t_pol_event.common_noise[i]                   = -1;
        t_pol_event.compress[i]                       = -1;
        t_pol_event.dy12_too_high[i]                  = false;
        t_pol_event.t_out_too_many[i]                 = false;
        t_pol_event.t_out_2[i]                        = false;
        t_pol_event.t_out_1[i]                        = false;
    }
    // from AUX
    t_pol_event.obox_mode                             = -1;
    for (int i = 0; i < 25; i++) {
        t_pol_event.fe_hv[i]                          = 123456;
        t_pol_event.fe_thr[i]                         = 123456;
        t_pol_event.fe_temp[i]                        = 123456;
    }
    // from PPD
    for (int i = 0; i < 3; i++) {
        t_pol_event.wgs84_xyz[i]                      = 123456;
    }
    for (int i = 0; i < 2; i++) {
        t_pol_event.det_z_radec[i]                    = 123456;
        t_pol_event.det_x_radec[i]                    = 123456;
        t_pol_event.earth_radec[i]                    = 123456;
        t_pol_event.sun_radec[i]                      = 123456;
    }
}

