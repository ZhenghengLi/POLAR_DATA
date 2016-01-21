#include "SciDataFile.hpp"

using namespace std;

SciDataFile::SciDataFile() {
    t_out_file_          = NULL;
    t_modules_tree_      = NULL;
    t_trigger_tree_      = NULL;
    t_ped_modules_tree_  = NULL;
    t_ped_trigger_tree_  = NULL;
}

SciDataFile::~SciDataFile() {

}

bool SciDataFile::open(const char* filename) {
    if (t_out_file_ != NULL)
        return false;
    t_out_file_ = new TFile(filename, "RECREATE");
    if (t_out_file_->IsZombie())
        return false;

    t_modules_tree_ = new TTree("t_modules", "modules packets");
    t_modules_tree_->SetDirectory(t_out_file_);
    t_modules_tree_->Branch("trigg_num",         &t_modules.trigg_num,         "trigg_num/L"           );
    t_modules_tree_->Branch("event_num",         &t_modules.event_num,         "event_num/L"           );
    t_modules_tree_->Branch("event_num_g",       &t_modules.event_num_g,       "event_num_g/L"         );
    t_modules_tree_->Branch("ped_entry",         &t_modules.ped_entry,         "ped_entry/L"           );
    t_modules_tree_->Branch("is_bad",            &t_modules.is_bad,            "is_bad/I"              );
    t_modules_tree_->Branch("pre_is_bad",        &t_modules.pre_is_bad,        "pre_is_bad/I"          );
    t_modules_tree_->Branch("compress",          &t_modules.compress,          "compress/I"            );
    t_modules_tree_->Branch("ct_num",            &t_modules.ct_num,            "ct_num/I"              );
    t_modules_tree_->Branch("time_stamp",        &t_modules.time_stamp,        "time_stamp/i"          );
    t_modules_tree_->Branch("time_period",       &t_modules.time_period,       "time_period/i"         );
    t_modules_tree_->Branch("time_wait",         &t_modules.time_wait,         "time_wait/i"           );
    t_modules_tree_->Branch("time_align",        &t_modules.time_align,        "time_align/i"          );
    t_modules_tree_->Branch("raw_rate",          &t_modules.raw_rate,          "raw_rate/I"            );
    t_modules_tree_->Branch("raw_dead",          &t_modules.raw_dead,          "raw_dead/I"            );
    t_modules_tree_->Branch("dead_ratio",        &t_modules.dead_ratio,        "dead_ratio/F"          );
    t_modules_tree_->Branch("status",            &t_modules.status,            "status/s"              );
    t_modules_tree_->Branch("trigger_bit",        t_modules.trigger_bit,       "trigger_bit[64]/O"     );
    t_modules_tree_->Branch("energy_adc",         t_modules.energy_adc,        "energy_adc[64]/F"      );
    t_modules_tree_->Branch("common_noise",      &t_modules.common_noise,      "common_noise/F"        );

    t_trigger_tree_ = new TTree("t_trigger", "trigger packets");
    t_trigger_tree_->SetDirectory(t_out_file_);
    t_trigger_tree_->Branch("trigg_num",         &t_trigger.trigg_num,         "trigg_num/L"           );
    t_trigger_tree_->Branch("trigg_num_g",       &t_trigger.trigg_num_g,       "trigg_num_g/L"         );
    t_trigger_tree_->Branch("ped_entry",         &t_trigger.ped_entry,         "ped_entry/L"           );
    t_trigger_tree_->Branch("is_bad",            &t_trigger.is_bad,            "is_bad/I"              );
    t_trigger_tree_->Branch("pre_is_bad",        &t_trigger.pre_is_bad,        "pre_is_bad/I"          );
    t_trigger_tree_->Branch("type",              &t_trigger.type,              "type/I"                );
    t_trigger_tree_->Branch("time_stamp",        &t_trigger.time_stamp,        "time_stamp/i"          );
    t_trigger_tree_->Branch("time_period",       &t_trigger.time_period,       "time_period/i"         );
    t_trigger_tree_->Branch("time_wait",         &t_trigger.time_wait,         "time_wait/i"           );
    t_trigger_tree_->Branch("time_align",        &t_trigger.time_align,        "time_align/i"          );
    t_trigger_tree_->Branch("frm_ship_time",     &t_trigger.frm_ship_time,     "frm_ship_time/l"       );
    t_trigger_tree_->Branch("frm_gps_time",      &t_trigger.frm_gps_time,      "frm_gps_time/l"        );
    t_trigger_tree_->Branch("pkt_start",         &t_trigger.pkt_start,         "pkt_start/L"           );
    t_trigger_tree_->Branch("pkt_count",         &t_trigger.pkt_count,         "pkt_count/I"           );
    t_trigger_tree_->Branch("lost_count",        &t_trigger.lost_count,        "lost_count/I"          );
    t_trigger_tree_->Branch("trigger_n",         &t_trigger.trigger_n,         "trigger_n/I"           );
    t_trigger_tree_->Branch("status",            &t_trigger.status,            "status/s"              );
    t_trigger_tree_->Branch("trig_sig_con",       t_trigger.trig_sig_con,      "trig_sig_con[25]/b"    );
    t_trigger_tree_->Branch("trig_accepted",      t_trigger.trig_accepted,     "trig_accepted[25]/O"   );
    t_trigger_tree_->Branch("trig_rejected",      t_trigger.trig_rejected,     "trig_rejected[25]/O"   );
    t_trigger_tree_->Branch("raw_dead",          &t_trigger.raw_dead,          "raw_dead/I"            );
    t_trigger_tree_->Branch("dead_ratio",        &t_trigger.dead_ratio,        "dead_ratio/F"          );

    t_ped_modules_tree_ = new TTree("t_ped_modules", "pedestal modules packets");
    t_ped_modules_tree_->SetDirectory(t_out_file_);
    t_ped_modules_tree_->Branch("trigg_num",         &t_ped_modules.trigg_num,         "trigg_num/L"           );
    t_ped_modules_tree_->Branch("event_num",         &t_ped_modules.event_num,         "event_num/L"           );
    t_ped_modules_tree_->Branch("event_num_g",       &t_ped_modules.event_num_g,       "event_num_g/L"         );
    t_ped_modules_tree_->Branch("ped_entry",         &t_ped_modules.ped_entry,         "ped_entry/L"           );
    t_ped_modules_tree_->Branch("is_bad",            &t_ped_modules.is_bad,            "is_bad/I"              );
    t_ped_modules_tree_->Branch("pre_is_bad",        &t_ped_modules.pre_is_bad,        "pre_is_bad/I"          );
    t_ped_modules_tree_->Branch("compress",          &t_ped_modules.compress,          "compress/I"            );
    t_ped_modules_tree_->Branch("ct_num",            &t_ped_modules.ct_num,            "ct_num/I"              );
    t_ped_modules_tree_->Branch("time_stamp",        &t_ped_modules.time_stamp,        "time_stamp/i"          );
    t_ped_modules_tree_->Branch("time_period",       &t_ped_modules.time_period,       "time_period/i"         );
    t_ped_modules_tree_->Branch("time_wait",         &t_ped_modules.time_wait,         "time_wait/i"           );
    t_ped_modules_tree_->Branch("time_align",        &t_ped_modules.time_align,        "time_align/i"          );
    t_ped_modules_tree_->Branch("raw_rate",          &t_ped_modules.raw_rate,          "raw_rate/I"            );
    t_ped_modules_tree_->Branch("raw_dead",          &t_ped_modules.raw_dead,          "raw_dead/I"            );
    t_ped_modules_tree_->Branch("dead_ratio",        &t_ped_modules.dead_ratio,        "dead_ratio/F"          );
    t_ped_modules_tree_->Branch("status",            &t_ped_modules.status,            "status/s"              );
    t_ped_modules_tree_->Branch("trigger_bit",        t_ped_modules.trigger_bit,       "trigger_bit[64]/O"     );
    t_ped_modules_tree_->Branch("energy_adc",         t_ped_modules.energy_adc,        "energy_adc[64]/F"      );
    t_ped_modules_tree_->Branch("common_noise",      &t_ped_modules.common_noise,      "common_noise/F"        );

    t_ped_trigger_tree_ = new TTree("t_ped_trigger", "pedestal trigger packets");
    t_ped_trigger_tree_->SetDirectory(t_out_file_);
    t_ped_trigger_tree_->Branch("trigg_num",         &t_ped_trigger.trigg_num,         "trigg_num/L"           );
    t_ped_trigger_tree_->Branch("trigg_num_g",       &t_ped_trigger.trigg_num_g,       "trigg_num_g/L"         );
    t_ped_trigger_tree_->Branch("ped_entry",         &t_ped_trigger.ped_entry,         "ped_entry/L"           );
    t_ped_trigger_tree_->Branch("is_bad",            &t_ped_trigger.is_bad,            "is_bad/I"              );
    t_ped_trigger_tree_->Branch("pre_is_bad",        &t_ped_trigger.pre_is_bad,        "pre_is_bad/I"          );
    t_ped_trigger_tree_->Branch("type",              &t_ped_trigger.type,              "type/I"                );
    t_ped_trigger_tree_->Branch("time_stamp",        &t_ped_trigger.time_stamp,        "time_stamp/i"          );
    t_ped_trigger_tree_->Branch("time_period",       &t_ped_trigger.time_period,       "time_period/i"         );
    t_ped_trigger_tree_->Branch("time_wait",         &t_ped_trigger.time_wait,         "time_wait/i"           );
    t_ped_trigger_tree_->Branch("time_align",        &t_ped_trigger.time_align,        "time_align/i"          );
    t_ped_trigger_tree_->Branch("frm_ship_time",     &t_ped_trigger.frm_ship_time,     "frm_ship_time/l"       );
    t_ped_trigger_tree_->Branch("frm_gps_time",      &t_ped_trigger.frm_gps_time,      "frm_gps_time/l"        );
    t_ped_trigger_tree_->Branch("pkt_start",         &t_ped_trigger.pkt_start,         "pkt_start/L"           );
    t_ped_trigger_tree_->Branch("pkt_count",         &t_ped_trigger.pkt_count,         "pkt_count/I"           );
    t_ped_trigger_tree_->Branch("lost_count",        &t_ped_trigger.lost_count,        "lost_count/I"          );
    t_ped_trigger_tree_->Branch("trigger_n",         &t_ped_trigger.trigger_n,         "trigger_n/I"           );
    t_ped_trigger_tree_->Branch("status",            &t_ped_trigger.status,            "status/s"              );
    t_ped_trigger_tree_->Branch("trig_sig_con",       t_ped_trigger.trig_sig_con,      "trig_sig_con[25]/b"    );
    t_ped_trigger_tree_->Branch("trig_accepted",      t_ped_trigger.trig_accepted,     "trig_accepted[25]/O"   );
    t_ped_trigger_tree_->Branch("trig_rejected",      t_ped_trigger.trig_rejected,     "trig_rejected[25]/O"   );
    t_ped_trigger_tree_->Branch("raw_dead",          &t_ped_trigger.raw_dead,          "raw_dead/I"            );
    t_ped_trigger_tree_->Branch("dead_ratio",        &t_ped_trigger.dead_ratio,        "dead_ratio/F"          );

    return true;
}

void SciDataFile::close() {

}

void SciDataFile::write_module_alone(const SciEvent& event, Int_t bad_status) {

}

void SciDataFile::write_trigger_alone(const SciTrigger& trigger, Int_t bad_status) {

}

void SciDataFile::write_event_align(const SciTrigger& trigger, const vector<SciEvent>& events_vec) {

}

void SciDataFile::write_ped_module_alone(const SciEvent& ped_event, Int_t ped_bad_status) {

}

void SciDataFile::write_ped_trigger_alone(const SciTrigger& ped_trigger, Int_t ped_bad_statsu) {

}

void SciDataFile::write_ped_event_align(const SciTrigger& ped_trigger, const vector<SciEvent>& ped_events_vec) {

}

void SciDataFile::copy_event_pkt_(Modules_T& t_modules_par, const SciEvent& event) {

}

void SciDataFile::copy_trigger_pkt_(Trigger_T& t_trigger_par, const SciTrigger& trigger) {

}
