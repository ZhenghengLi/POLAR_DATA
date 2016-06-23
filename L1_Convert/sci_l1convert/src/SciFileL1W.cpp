#include "SciFileL1W.hpp"

using namespace std;

SciFileL1W::SciFileL1W() {
    t_file_out_ = NULL;
    t_pol_event_tree_ = NULL;
    first_valid_found_ = false;
}

SciFileL1W::~SciFileL1W() {
    close();
}

bool SciFileL1W::open(const char* filename) {
    if (t_file_out_ != NULL)
        return false;
    t_file_out_ = new TFile(filename, "RECREATE");
    if (t_file_out_->IsZombie())
        return false;

    t_pol_event_tree_ = new TTree("t_pol_event", "POLAR events");
    t_pol_event_tree_->SetDirectory(t_file_out_);
    t_pol_event_tree_->Branch("abs_gps_week",      &t_pol_event.abs_gps_week,      "abs_gps_week/I"           );
    t_pol_event_tree_->Branch("abs_gps_second",    &t_pol_event.abs_gps_second,    "abs_gps_second/D"         );
    t_pol_event_tree_->Branch("abs_gps_valid",     &t_pol_event.abs_gps_valid,     "abs_gps_valid/O"          );
    t_pol_event_tree_->Branch("type",              &t_pol_event.type,              "type/I"                   );
    t_pol_event_tree_->Branch("trig_accepted",      t_pol_event.trig_accepted,     "trig_accepted[25]/O"      );
    t_pol_event_tree_->Branch("time_aligned",       t_pol_event.time_aligned,      "time_aligned[25]/O"       );
    t_pol_event_tree_->Branch("pkt_count",         &t_pol_event.pkt_count,         "pkt_count/I"              );
    t_pol_event_tree_->Branch("lost_count",        &t_pol_event.lost_count,        "lost_count/I"             );
    t_pol_event_tree_->Branch("trigger_bit",        t_pol_event.trigger_bit,       "trigger_bit[1600]/O"      );
    t_pol_event_tree_->Branch("trigger_n",         &t_pol_event.trigger_n,         "trigger_n/I"              );
    t_pol_event_tree_->Branch("multiplicity",       t_pol_event.multiplicity,      "multiplicity[25]/I"       );
    t_pol_event_tree_->Branch("energy_adc",         t_pol_event.energy_adc,        "energy_adc[1600]/F"       );
    t_pol_event_tree_->Branch("compress",           t_pol_event.compress,          "compress[25]/I"           );
    t_pol_event_tree_->Branch("common_noise",       t_pol_event.common_noise,      "common_noise[25]/I"       );

    pol_event_cur_index_ = -1;
    
    first_valid_found_   = false;
    first_valid_index_   = -1;
    first_valid_week_    = -1;
    first_valid_second_  = -1;
    last_valid_index_    = -1;
    last_valid_week_     = -1;
    last_valid_second_   = -1;
    total_valid_count_   = 0;
    
    return true;
}

void SciFileL1W::close() {
    if (t_file_out_ == NULL)
        return;

    delete t_pol_event_tree_;
    t_pol_event_tree_ = NULL;

    t_file_out_->Close();
    delete t_file_out_;
    t_file_out_ = NULL;
}

void SciFileL1W::fill_event() {
    t_pol_event_tree_->Fill();
    // record first and last GPS time and entry
    pol_event_cur_index_++;
    if (t_pol_event.abs_gps_week >= 0 && t_pol_event.abs_gps_second >= 0 && t_pol_event.abs_gps_valid) {
        total_valid_count_++;
        if (!first_valid_found_) {
            first_valid_found_  = true;
            first_valid_index_  = pol_event_cur_index_;
            first_valid_week_   = t_pol_event.abs_gps_week;
            first_valid_second_ = t_pol_event.abs_gps_second;
        }
        last_valid_index_       = pol_event_cur_index_;
        last_valid_week_        = t_pol_event.abs_gps_week;
        last_valid_second_      = t_pol_event.abs_gps_second;
    }
}

void SciFileL1W::write_tree() {
    t_pol_event_tree_->Write();
}

void SciFileL1W::write_meta(const char* key, const char* value) {
    if (t_file_out_ == NULL)
        return;
    TNamed* cur_meta = new TNamed(key, value);
    cur_meta->Write();
    delete cur_meta;
    cur_meta = NULL;
}

void SciFileL1W::gen_gps_result_str() {
    char str_buffer[200];
    sprintf(str_buffer, "%d:%d[%ld] => %d:%d[%ld]; %ld/%ld",
            static_cast<int>(first_valid_week_),
            static_cast<int>(first_valid_second_),
            static_cast<long int>(first_valid_index_),
            static_cast<int>(last_valid_week_),
            static_cast<int>(last_valid_second_),
            static_cast<long int>(last_valid_index_),
            static_cast<long int>(total_valid_count_),
            static_cast<long int>(t_pol_event_tree_->GetEntries()));
    gps_result_str_.assign(str_buffer);
}

void SciFileL1W::write_gps_span() {
    write_meta("m_gpsspan", gps_result_str_.c_str());
}

void SciFileL1W::print_gps_span() {
    cout << "================================================================================" << endl;
    cout << "gps_span: { " << gps_result_str_ << " }" << endl;
    cout << "================================================================================" << endl;
}
