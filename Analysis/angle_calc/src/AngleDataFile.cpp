#include "AngleDataFile.hpp"

using namespace std;

AngleDataFile::AngleDataFile() {
    t_angle_file_ = NULL;
    t_angle_tree_ = NULL;
    m_fromfile_   = NULL;
    m_gps_span_   = NULL;
    mode_ = '0';
}

AngleDataFile::~AngleDataFile() {
    close();
}

bool AngleDataFile::open(const char* filename, char m) {
    if(t_angle_file_ != NULL)
        return false;

    if (m == 'w' || m == 'W')
        mode_ = 'w';
    else if (m == 'r' || m == 'R')
        mode_ = 'r';
    else
        return false;

    if (mode_ == 'r')
        t_angle_file_ = new TFile(filename, "READ");
    else
        t_angle_file_ = new TFile(filename, "UPDATE");
    if (t_angle_file_->IsZombie())
        return false;

    t_angle_tree_ = static_cast<TTree*>(t_angle_file_->Get("t_angle"));
    is_first_created_ = false;
    if (t_angle_tree_ == NULL) {
        if (mode_ == 'r') {
            return false;
        } else {
            t_angle_tree_ = new TTree("t_angle", "scattering angle data");
            t_angle_tree_->SetDirectory(t_angle_file_);
            is_first_created_ = true;
        }
    }
    if (is_first_created_) {
        t_angle_tree_->Branch("abs_gps_week",    &t_angle.abs_gps_week,     "abs_gps_week/I"    );
        t_angle_tree_->Branch("abs_gps_second",  &t_angle.abs_gps_second,   "abs_gps_second/D"  );
        t_angle_tree_->Branch("abs_gps_valid",   &t_angle.abs_gps_valid,    "abs_gps_valid/O"   );
        t_angle_tree_->Branch("abs_ship_second", &t_angle.abs_ship_second,  "abs_ship_second/D" );
        t_angle_tree_->Branch("first_ij",         t_angle.first_ij,         "first_ij[2]/I"     );
        t_angle_tree_->Branch("second_ij",        t_angle.second_ij,        "second_ij[2]/I"    );
        t_angle_tree_->Branch("rand_angle",      &t_angle.rand_angle,       "rand_angle/F"      );
    } else {
        t_angle_tree_->SetBranchAddress("abs_gps_week",    &t_angle.abs_gps_week     );
        t_angle_tree_->SetBranchAddress("abs_gps_second",  &t_angle.abs_gps_second   );
        t_angle_tree_->SetBranchAddress("abs_gps_valid",   &t_angle.abs_gps_valid    );
        t_angle_tree_->SetBranchAddress("abs_ship_second", &t_angle.abs_ship_second  );
        t_angle_tree_->SetBranchAddress("first_ij",         t_angle.first_ij         );
        t_angle_tree_->SetBranchAddress("second_ij",        t_angle.second_ij        );
        t_angle_tree_->SetBranchAddress("rand_angle",      &t_angle.rand_angle       );
    }
    if (mode_ == 'r') {
        m_fromfile_ = static_cast<TNamed*>(t_angle_file_->Get("m_fromfile"));
        m_gps_span_ = static_cast<TNamed*>(t_angle_file_->Get("m_gps_span"));
        if (m_fromfile_ == NULL || m_gps_span_ == NULL)
            return false;
        angle_set_start();
    }
    
    return true;
}

void AngleDataFile::close() {
    if (t_angle_file_ == NULL)
        return;
    delete t_angle_tree_;
    t_angle_tree_ = NULL;
    t_angle_file_->Close();
    delete t_angle_file_;
    t_angle_file_ = NULL;
}

void AngleDataFile::angle_fill() {
    if (t_angle_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    t_angle_tree_->Fill();
}

void AngleDataFile::write_all_tree() {
    if (t_angle_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    t_angle_file_->cd();
    t_angle_tree_->Write("", TObject::kOverwrite);
}

void AngleDataFile::write_meta(const char* key, const char* value, bool append_flag) {
    if (t_angle_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    TNamed* cur_meta = static_cast<TNamed*>(t_angle_file_->Get(key));
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

void AngleDataFile::write_fromfile(const char* filename) {
    if (t_angle_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    write_meta("m_fromfile", TSystem().BaseName(filename));
}

void AngleDataFile::write_gps_span(const char* gps_span) {
    if (t_angle_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    write_meta("m_gps_span", gps_span);
}

void AngleDataFile::write_lasttime() {
    if (t_angle_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    TTimeStamp * cur_time = new TTimeStamp();
    write_meta("m_lasttime", cur_time->AsString("lc"), false);
    delete cur_time;
    cur_time = NULL;
}

void AngleDataFile::angle_set_start() {
    if (t_angle_file_ == NULL)
        return;
    if (mode_ == 'w')
        return;
    angle_cur_entry_ = -1;
    angle_reach_end_ = false;
}

Long64_t AngleDataFile::angle_get_cur_entry() {
    if (t_angle_file_ == NULL)
        return -1;
    if (mode_ == 'w')
        return -1;
    return angle_cur_entry_;
}

Long64_t AngleDataFile::angle_get_tot_entries() {
    if (t_angle_file_ == NULL)
        return -1;
    if (mode_ == 'w')
        return -1;
    return t_angle_tree_->GetEntries();
}

bool AngleDataFile::angle_next() {
    if (t_angle_file_ == NULL)
        return false;
    if (mode_ == 'w')
        return false;
    if (angle_reach_end_)
        return false;
    angle_cur_entry_++;
    if (angle_cur_entry_ < t_angle_tree_->GetEntries()) {
        t_angle_tree_->GetEntry(angle_cur_entry_);
        return true;
    } else {
        angle_reach_end_ = true;
        return false;
    }
}

string AngleDataFile::get_fromfile_str() {
    if (t_angle_file_ == NULL)
        return "";
    if (mode_ == 'w')
        return "";
    return string(m_fromfile_->GetTitle());
}

string AngleDataFile::get_gps_span_str() {
    if (t_angle_file_ == NULL)
        return "";
    if (mode_ == 'w')
        return "";
    return string(m_gps_span_->GetTitle());
}

char AngleDataFile::get_mode() {
    if (t_angle_file_ == NULL)
        return '0';
    else 
        return mode_;
}
