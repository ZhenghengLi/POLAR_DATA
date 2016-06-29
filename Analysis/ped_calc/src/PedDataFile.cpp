#include "PedDataFile.hpp"

PedDataFile::PedDataFile() {

}

PedDataFile::~PedDataFile() {
    close();
}

bool PedDataFile::open(const char* filename, char m) {
    if (t_ped_file_ != NULL)
        return false;

    if (m == 'w' || m == 'W')
        mode_ = 'w';
    else if (m == 'r' || m == 'R')
        mode_ = 'r';
    else
        return false;

    if (mode_ == 'r')
        t_ped_file_ = new TFile(filename, "READ");
    else
        t_ped_file_ = new TFile(filename, "UPDATE");
    if (t_ped_file_->IsZombie())
        return false;

    for (int i = 0; i < 25; i++) {
        sprintf(name_, "t_ped_data_ct_%d", i + 1);
        sprintf(title_, "pedestal data of ct %d", i + 1);
        t_ped_data_tree_[i] = static_cast<TTree*>(t_ped_file_->Get(name_));
        is_first_created_[i] = false;
        if (t_ped_data_tree_[i] == NULL) {
            if (mode_ == 'r') {
                return false;
            } else {
                t_ped_data_tree_[i] = new TTree(name_, title_);
                t_ped_data_tree_[i]->SetDirectory(t_ped_file_);
                is_first_created_[i] = true;
            }
        }
        if (is_first_created_[i]) {
            t_ped_data_tree_[i]->Branch("ped_adc", t_ped_data[i].ped_adc, "ped_adc[64]/F");
        } else {
            t_ped_data_tree_[i]->SetBranchAddress("ped_adc", t_ped_data[i].ped_adc);
        }
        if (mode_ == 'r') {
            m_fromfile_ = static_cast<TNamed*>(t_ped_file_->Get("m_fromfile"));
            m_gps_span_ = static_cast<TNamed*>(t_ped_file_->Get("m_gps_span"));
            if (m_fromfile_ == NULL || m_gps_span_ == NULL)
                return false;
            mod_set_start(i);
        }
    }
    
    return true;
}

void PedDataFile::close() {
    if (t_ped_file_ == NULL)
        return;
    for (int i = 0; i < 25; i++) {
        delete t_ped_data_tree_[i];
        t_ped_data_tree_[i] = NULL;
    }
    t_ped_file_->Close();
    delete t_ped_file_;
    t_ped_file_ = NULL;
}

void PedDataFile::mod_fill(int ct_idx) {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    if (ct_idx < 0 || ct_idx > 24)
        return;
    t_ped_data_tree_[ct_idx]->Fill();
}

void PedDataFile::write_all_tree() {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    t_ped_file_->cd();
    for (int i = 0; i < 25; i++) {
        t_ped_data_tree_[i]->Write("", TObject::kOverwrite);
    }
}

void PedDataFile::write_meta(const char* key, const char* value, bool append_flag) {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    TNamed* cur_meta = static_cast<TNamed*>(t_ped_file_->Get(key));
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

void PedDataFile::write_fromfile(const char* filename) {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    write_meta("m_fromfile", TSystem().BaseName(filename));
}

void PedDataFile::write_gps_span(const char* begin_gps, const char* end_gps) {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    write_meta("m_gps_span", Form("%s => %s", begin_gps, end_gps));
}

void PedDataFile::write_lasttime() {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    TTimeStamp * cur_time = new TTimeStamp();
    write_meta("m_lasttime", cur_time->AsString("lc"), false);
    delete cur_time;
    cur_time = NULL;
}

void PedDataFile::mod_set_start(int ct_idx) {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'w')
        return;
    if (ct_idx < 0 || ct_idx > 24)
        return;
    mod_cur_entry_[ct_idx] = -1;
    mod_reach_end_[ct_idx] = false;
}

Long64_t PedDataFile::mod_get_cur_entry(int ct_idx) {
    if (t_ped_file_ == NULL)
        return -1;
    if (mode_ == 'w')
        return -1;
    if (ct_idx < 0 || ct_idx > 24)
        return -1;
    return mod_cur_entry_[ct_idx];
}

Long64_t PedDataFile::mod_get_tot_entries(int ct_idx) {
    if (t_ped_file_ == NULL)
        return -1;
    if (mode_ == 'w')
        return -1;
    if (ct_idx < 0 || ct_idx > 24)
        return -1;
    return t_ped_data_tree_[ct_idx]->GetEntries();
}

bool PedDataFile::mod_next(int ct_idx) {
    if (t_ped_file_ == NULL)
        return false;
    if (mode_ == 'w')
        return false;
    if (ct_idx < 0 || ct_idx > 24)
        return false;
    if (mod_reach_end_[ct_idx])
        return false;
    mod_cur_entry_[ct_idx]++;
    if (mod_cur_entry_[ct_idx] < t_ped_data_tree_[ct_idx]->GetEntries()) {
        t_ped_data_tree_[ct_idx]->GetEntry(mod_cur_entry_[ct_idx]);
        return true;
    } else {
        mod_reach_end_[ct_idx] = true;
        return false;
    }
}

string PedDataFile::get_fromfile_str() {
    if (t_ped_file_ == NULL)
        return "";
    if (mode_ == 'w')
        return "";
    return string(m_fromfile_->GetTitle());
}

string PedDataFile::get_gps_span_str() {
    if (t_ped_file_ == NULL)
        return "";
    if (mode_ == 'w')
        return "";
    return string(m_gps_span_->GetTitle());
}

char PedDataFile::get_mode() {
    if (t_ped_file_ == NULL)
        return '0';
    else
        return mode_;
}
