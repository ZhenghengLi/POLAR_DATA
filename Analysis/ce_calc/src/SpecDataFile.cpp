#include "SpecDataFile.hpp"

using namespace std;

SpecDataFile::SpecDataFile() {
    t_spec_file_ = NULL;
    t_source_event_tree_ = NULL;
    m_fromfile_ = NULL;
    m_gps_span_ = NULL;
    mode_ = '0';
    
}

SpecDataFile::~SpecDataFile() {
    close();
}

bool SpecDataFile::open(const char* filename, char m) {
    if (t_spec_file_ != NULL)
        return false;

    if (m == 'w' || m == 'W')
        mode_ = 'w';
    else if (m == 'r' || m == 'R')
        mode_ = 'r';
    else
        return false;

    if (mode_ == 'r')
        t_spec_file_ = new TFile(filename, "READ");
    else
        t_spec_file_ = new TFile(filename, "UPDATE");
    if (t_spec_file_->IsZombie())
        return false;

    t_source_event_tree_ = static_cast<TTree*>(t_spec_file_->Get("t_source_event"));
    is_first_created_ = false;
    if (t_source_event_tree_ == NULL) {
        if (mode_ == 'r') {
            return false;
        } else {
            t_source_event_tree_ = new TTree("t_source_event", "source event data");
            t_source_event_tree_->SetDirectory(t_spec_file_);
            is_first_created_ = true;
        }
    }
    if (is_first_created_) {
        t_source_event_tree_->Branch("type",           &t_source_event.type,           "type/I"                );
        t_source_event_tree_->Branch("trig_accepted",   t_source_event.trig_accepted,  "trig_accepted[25]/O"   );
        t_source_event_tree_->Branch("trigger_bit",     t_source_event.trigger_bit,    "trigger_bit[1600]/O"   );
        t_source_event_tree_->Branch("trigger_n",      &t_source_event.trigger_n,      "trigger_n/I"           );
        t_source_event_tree_->Branch("multiplicity",    t_source_event.multiplicity,   "multiplicity[25]/I"    );
        t_source_event_tree_->Branch("energy_adc",      t_source_event.energy_adc,     "energy_adc[1600]/F"    );
    } else {
        t_source_event_tree_->SetBranchAddress("type",           &t_source_event.type           );
        t_source_event_tree_->SetBranchAddress("trig_accepted",   t_source_event.trig_accepted  );
        t_source_event_tree_->SetBranchAddress("trigger_bit",     t_source_event.trigger_bit    );
        t_source_event_tree_->SetBranchAddress("trigger_n",      &t_source_event.trigger_n      );
        t_source_event_tree_->SetBranchAddress("multiplicity",    t_source_event.multiplicity   );
        t_source_event_tree_->SetBranchAddress("energy_adc",      t_source_event.energy_adc     );
    }
    if (mode_ == 'r') {
        m_fromfile_ = static_cast<TNamed*>(t_spec_file_->Get("m_fromfile"));
        m_gps_span_ = static_cast<TNamed*>(t_spec_file_->Get("m_gps_span"));
        if (m_fromfile_ == NULL || m_gps_span_ == NULL)
            return false;
        event_set_start();
    }
    
    return true;
}

void SpecDataFile::close() {

}

void SpecDataFile::clear_cur_entry() {

}

void SpecDataFile::event_fill() {

}

void SpecDataFile::write_all_tree() {

}

void SpecDataFile::write_meta(const char* key, const char* value, bool append_flag) {

}

void SpecDataFile::write_fromfile(const char* filename) {

}

void SpecDataFile::write_gps_span(const char* begin_gps, const char* end_gps) {

}

void SpecDataFile::write_lasttime() {

}

void SpecDataFile::event_set_start() {

}

Long64_t SpecDataFile::event_get_cur_entry() {

    return 0;
}

Long64_t SpecDataFile::event_get_tot_entries() {

    return 0;
}

bool SpecDataFile::event_next() {

    return true;
}

string SpecDataFile::get_fromfile_str() {

    return "";
}

string SpecDataFile::get_gps_span_str() {

    return "";
}

char SpecDataFile::get_mode() {

    return '0';
}


