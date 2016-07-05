#include "XtalkDataFile.hpp"

using namespace std;

XtalkDataFile::XtalkDataFile() {
    t_xtalk_file_ = NULL;
    for (int i = 0; i < 25; i++) {
        t_xtalk_data_tree_[i] = NULL;
    }
    m_fromfile_ = NULL;
    m_gps_span_ = NULL;
    mode_ = '0';
}

XtalkDataFile::~XtalkDataFile() {
    close();
}

bool XtalkDataFile::open(const char* filename, char m) {
    if (t_xtalk_file_ != NULL)
        return false;

    if (m == 'w' || m == 'W')
        mode_ = 'w';
    else if (m == 'r' || m == 'R')
        mode_ = 'r';
    else
        return false;

    if (mode_ == 'r')
        t_xtalk_file_ = new TFile(filename, "READ");
    else
        t_xtalk_file_ = new TFile(filename, "UPDATE");
    if (t_xtalk_file_->IsZombie())
        return false;
    
    for (int i = 0; i < 25; i++) {
        sprintf(name_, "t_xtalk_data_ct_%02d", i + 1);
        sprintf(title_, "crosstalk data of ct %02d", i + 1);
        t_xtalk_data_tree_[i] = static_cast<TTree*>(t_xtalk_file_->Get(name_));
        is_first_created_[i] = false;
        if (t_xtalk_data_tree_[i] == NULL) {
            if (mode_ == 'r') {
                return false;
            } else {
                t_xtalk_data_tree_[i] = new TTree(name_, title_);
                t_xtalk_data_tree_[i]->SetDirectory(t_xtalk_file_);
                is_first_created_[i] = true;
            }
        }
        if (is_first_created_[i]) {
            t_xtalk_data_tree_[i]->Branch("jx",   &t_xtalk_data[i].jx,   "jx/I" );
            t_xtalk_data_tree_[i]->Branch("jy",   &t_xtalk_data[i].jy,   "jy/I" );
            t_xtalk_data_tree_[i]->Branch("x",    &t_xtalk_data[i].x,    "x/F"  );
            t_xtalk_data_tree_[i]->Branch("y",    &t_xtalk_data[i].y,    "y/F"  );
        } else {
            t_xtalk_data_tree_[i]->SetBranchAddress("jx",   &t_xtalk_data[i].jx );
            t_xtalk_data_tree_[i]->SetBranchAddress("jy",   &t_xtalk_data[i].jy );
            t_xtalk_data_tree_[i]->SetBranchAddress("x",    &t_xtalk_data[i].x  );
            t_xtalk_data_tree_[i]->SetBranchAddress("y",    &t_xtalk_data[i].y  );
        }
        if (mode_ == 'r') {
            m_fromfile_ = static_cast<TNamed*>(t_xtalk_file_->Get("m_fromfile"));
            m_gps_span_ = static_cast<TNamed*>(t_xtalk_file_->Get("m_gps_span"));
            if (m_fromfile_ == NULL || m_gps_span_ == NULL)
                return false;
            mod_set_start(i);
        }
    }

    return true;
}

void XtalkDataFile::close() {
    if (t_xtalk_file_ == NULL)
        return;
    for (int i = 0; i < 25; i++) {
        delete t_xtalk_data_tree_[i];
        t_xtalk_data_tree_[i] = NULL;
    }
    t_xtalk_file_->Close();
    delete t_xtalk_file_;
    t_xtalk_file_ = NULL;
}

void XtalkDataFile::mod_fill(int ct_idx) {
    if (t_xtalk_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    if (ct_idx < 0 || ct_idx > 24)
        return;
    t_xtalk_data_tree_[ct_idx]->Fill();
}

void XtalkDataFile::write_all_tree() {
    if (t_xtalk_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    t_xtalk_file_->cd();
    for (int i = 0; i < 25; i++) {
        t_xtalk_data_tree_[i]->Write("", TObject::kOverwrite);
    }
}

void XtalkDataFile::write_meta(const char* key, const char* value, bool append_flag) {
    if (t_xtalk_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    TNamed* cur_meta = static_cast<TNamed*>(t_xtalk_file_->Get(key));
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

void XtalkDataFile::write_fromfile(const char* filename) {
    if (t_xtalk_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    write_meta("m_fromfile", TSystem().BaseName(filename));
}

void XtalkDataFile::write_gps_span(const char* begin_gps, const char* end_gps) {
    if (t_xtalk_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    write_meta("m_gps_span", Form("%s => %s", begin_gps, end_gps));
}

void XtalkDataFile::write_lasttime() {
    if (t_xtalk_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    TTimeStamp * cur_time = new TTimeStamp();
    write_meta("m_lasttime", cur_time->AsString("lc"), false);
    delete cur_time;
    cur_time = NULL;
}

void XtalkDataFile::mod_set_start(int ct_idx) {
    if (t_xtalk_file_ == NULL)
        return;
    if (mode_ == 'w')
        return;
    if (ct_idx < 0 || ct_idx > 24)
        return;
    mod_cur_entry_[ct_idx] = -1;
    mod_reach_end_[ct_idx] = false;
}

Long64_t XtalkDataFile::mod_get_cur_entry(int ct_idx) {
    if (t_xtalk_file_ == NULL)
        return -1;
    if (mode_ == 'w')
        return -1;
    if (ct_idx < 0 || ct_idx > 24)
        return -1;
    return mod_cur_entry_[ct_idx];
}

Long64_t XtalkDataFile::mod_get_tot_entries(int ct_idx) {
    if (t_xtalk_file_ == NULL)
        return -1;
    if (mode_ == 'w')
        return -1;
    if (ct_idx < 0 || ct_idx > 24)
        return -1;
    return t_xtalk_data_tree_[ct_idx]->GetEntries();
}

bool XtalkDataFile::mod_next(int ct_idx) {
    if (t_xtalk_file_ == NULL)
        return false;
    if (mode_ == 'w')
        return false;
    if (ct_idx < 0 || ct_idx > 24)
        return false;
    if (mod_reach_end_[ct_idx])
        return false;
    mod_cur_entry_[ct_idx]++;
    if (mod_cur_entry_[ct_idx] < t_xtalk_data_tree_[ct_idx]->GetEntries()) {
        t_xtalk_data_tree_[ct_idx]->GetEntry(mod_cur_entry_[ct_idx]);
        return true;
    } else {
        mod_reach_end_[ct_idx] = true;
        return false;
    }
}

string XtalkDataFile::get_fromfile_str() {
    if (t_xtalk_file_ == NULL)
        return "";
    if (mode_ == 'w')
        return "";
    return string(m_fromfile_->GetTitle());
}

string XtalkDataFile::get_gps_span_str() {
    if (t_xtalk_file_ == NULL)
        return "";
    if (mode_ == 'w')
        return "";
    return string(m_gps_span_->GetTitle());
}

char XtalkDataFile::get_mode() {
    if (t_xtalk_file_ == NULL)
        return '0';
    else
        return mode_;
}
