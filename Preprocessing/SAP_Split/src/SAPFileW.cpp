#include "SAPFileW.hpp"

using namespace std;

SAPFileW::SAPFileW() {
    t_file_out_ = NULL;
    t_pol_event_tree_ = NULL;
}

SAPFileW::~SAPFileW() {
    if (t_file_out_ != NULL)
        close();
}

bool SAPFileW::open(const char* filename) {
    if (t_file_out_ != NULL)
        return false;
    t_file_out_ = new TFile(filename, "recreate");
    if (t_file_out_->IsZombie())
        return false;
    t_pol_event_tree_ = new TTree("t_pol_event", "POLAR event data");
    build_pol_event_tree(t_pol_event_tree_, t_pol_event);
    first_flag_ = true;
    return true;
}

void SAPFileW::close() {
    if (t_file_out_ == NULL)
        return;
    delete t_pol_event_tree_;
    t_pol_event_tree_ = NULL;
    t_file_out_->Close();
    delete t_file_out_;
    t_file_out_ = NULL;
}

void SAPFileW::fill_entry() {
    if (t_file_out_ == NULL || t_pol_event_tree_ == NULL)
        return;
    t_pol_event_tree_->Fill();
    if (first_flag_) {
        first_flag_ = false;
        event_time_first_ = t_pol_event.event_time;
    }
    event_time_last_ = t_pol_event.event_time;
}

void SAPFileW::write_tree() {
    if (t_file_out_ == NULL || t_pol_event_tree_ == NULL)
        return;
    t_file_out_->cd();
    t_pol_event_tree_->Write();
}

void SAPFileW::write_meta(const char* key, const char* value) {
    if (t_file_out_ == NULL)
        return;
    t_file_out_->cd();
    TNamed* cur_meta = new TNamed(key, value);
    cur_meta->Write();
    delete cur_meta;
    cur_meta = NULL;
}

string SAPFileW::get_time_span() {
    if (t_file_out_ == NULL || t_pol_event_tree_ == NULL)
        return string("");
    char str_buffer[80];
    sprintf(str_buffer, "%d[0] => %d[%ld]",
            event_time_first_,
            event_time_last_,
            static_cast<long int>(t_pol_event_tree_->GetEntries() - 1));
    return string(str_buffer);
}

