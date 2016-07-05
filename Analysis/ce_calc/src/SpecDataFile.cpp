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
    if (t_spec_file_ == NULL)
        return;
    delete t_source_event_tree_;
    t_source_event_tree_ = NULL;
    t_spec_file_->Close();
    delete t_spec_file_;
    t_spec_file_ = NULL;
}

void SpecDataFile::clear_cur_entry() {
    t_source_event.type = 0;
    for (int i = 0; i < 25; i++) {
        t_source_event.trig_accepted[i] = false;
        t_source_event.multiplicity[i] = 0;
    }
    t_source_event.trigger_n = 0;
    for (int i = 0; i < 1600; i++) {
        t_source_event.trigger_bit[i] = false;
        t_source_event.energy_adc[i] = 0;
    }
}

void SpecDataFile::event_fill() {
    if (t_spec_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    t_source_event_tree_->Fill();
}

void SpecDataFile::write_all_tree() {
    if (t_spec_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    t_spec_file_->cd();
    t_source_event_tree_->Write("", TObject::kOverwrite);
}

void SpecDataFile::write_meta(const char* key, const char* value, bool append_flag) {
    if (t_spec_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    TNamed* cur_meta = static_cast<TNamed*>(t_spec_file_->Get(key));
    if (cur_meta == NULL) {
        cur_meta = new TNamed(key, value);
    } else {
        string tmp_title;
        if (append_flag) {
            tmp_title = cur_meta->GetTitle();
            tmp_title += "; ";
            tmp_title += value;
        } else {
            tmp_title = value;
        }   
        cur_meta->SetTitle(tmp_title.c_str());
    }   
    cur_meta->Write("", TObject::kOverwrite);
    delete cur_meta;
    cur_meta = NULL;
}

void SpecDataFile::write_fromfile(const char* filename) {
    if (t_spec_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    write_meta("m_fromfile", TSystem().BaseName(filename));
}

void SpecDataFile::write_gps_span(const char* begin_gps, const char* end_gps) {
    if (t_spec_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    write_meta("m_gps_span", Form("%s => %s", begin_gps, end_gps));
}

void SpecDataFile::write_lasttime() {
    if (t_spec_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    TTimeStamp * cur_time = new TTimeStamp();
    write_meta("m_lasttime", cur_time->AsString("lc"), false);
    delete cur_time;
    cur_time = NULL;
}

void SpecDataFile::event_set_start() {
    if (t_spec_file_ == NULL)
        return;
    if (mode_ == 'w')
        return;
    event_cur_entry_ = -1;
    event_reach_end_ = false;
}

Long64_t SpecDataFile::event_get_cur_entry() {
    if (t_spec_file_ == NULL)
        return -1;
    if (mode_ == 'w')
        return -1;
    return event_cur_entry_;
}

Long64_t SpecDataFile::event_get_tot_entries() {
    if (t_spec_file_ == NULL)
        return -1;
    if (mode_ == 'w')
        return -1;
    return t_source_event_tree_->GetEntries();
}

bool SpecDataFile::event_next() {
    if (t_spec_file_ == NULL)
        return false;
    if (mode_ == 'w')
        return false;
    if (event_reach_end_)
        return false;
    event_cur_entry_++;
    if (event_cur_entry_ < t_source_event_tree_->GetEntries()) {
        t_source_event_tree_->GetEntry(event_cur_entry_);
        return true;
    } else {
        event_reach_end_ = true;
        return false;
    }
}

string SpecDataFile::get_fromfile_str() {
    if (t_spec_file_ == NULL)
        return "";
    if (mode_ == 'w')
        return "";
    return string(m_fromfile_->GetTitle());
}

string SpecDataFile::get_gps_span_str() {
    if (t_spec_file_ == NULL)
        return "";
    if (mode_ == 'w')
        return "";
    return string(m_gps_span_->GetTitle());
}

char SpecDataFile::get_mode() {
    if (t_spec_file_ == NULL)
        return '0';
    else
        return mode_;
}
