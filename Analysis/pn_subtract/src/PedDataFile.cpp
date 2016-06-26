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
        if (mode_ == 'r') {
            t_ped_data_tree_[i]->SetBranchAddress("ped_adc", t_ped_data[i].ped_adc);
            mod_set_start(i + 1);
        } else {
            if (is_first_created_[i]) {
                t_ped_data_tree_[i]->Branch("ped_adc", t_ped_data[i].ped_adc, "ped_adc[64]/F");
            } else {
                t_ped_data_tree_[i]->SetBranchAddress("ped_adc", t_ped_data[i].ped_adc);
            }
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

void PedDataFile::mod_fill(int ct_num) {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    if (ct_num < 1 || ct_num > 25)
        return;
    t_ped_data_tree_[ct_num - 1]->Fill();
}

void PedDataFile::write_all_tree() {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    t_ped_file_->cd();
    for (int i = 0; i < 25; i++) {
        t_ped_data_tree_[i]->Write();
    }
}

void PedDataFile::write_meta(const char* key, const char* value) {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    TNamed* cur_meta = new TNamed(key, value);
    cur_meta->Write();
    delete cur_meta;
    cur_meta = NULL;
}

void PedDataFile::write_fromfile(const char* filename) {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    TSystem sys;
    TNamed* cur_meta = static_cast<TNamed*>(t_ped_file_->Get("m_fromfile"));
    if (cur_meta == NULL) {
        write_meta("m_fromfile", sys.BaseName(filename));
    } else {
        string fromfile_list = cur_meta->GetTitle();
        fromfile_list += "; ";
        fromfile_list += sys.BaseName(filename);
        cur_meta->SetTitle(fromfile_list.c_str());
        cur_meta->Write();
    }
}

void PedDataFile::write_gps_span(const char* gps_span) {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    TNamed* cur_meta = static_cast<TNamed*>(t_ped_file_->Get("m_gps_span"));
    if (cur_meta == NULL) {
        write_meta("m_gps_span", gps_span);
    } else {
        string gps_span_list = cur_meta->GetTitle();
        gps_span_list += "; ";
        gps_span_list += gps_span;
        cur_meta->SetTitle(gps_span_list.c_str());
        cur_meta->Write();
    }
}

void PedDataFile::write_lasttime() {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'r')
        return;
    TTimeStamp * cur_time = new TTimeStamp();
    TNamed* cur_meta = static_cast<TNamed*>(t_ped_file_->Get("m_lasttime"));
    if (cur_meta == NULL) {
        write_meta("m_lasttime", cur_time->AsString("lc"));
    } else {
        cur_meta->SetTitle(cur_time->AsString("lc"));
        cur_meta->Write();
    }
}

void PedDataFile::mod_set_start(int ct_num) {
    if (t_ped_file_ == NULL)
        return;
    if (mode_ == 'w')
        return;
    if (ct_num < 1 || ct_num > 25)
        return;
    mod_cur_entry_[ct_num - 1] = -1;
    mod_reach_end_[ct_num - 1] = false;
}

Long64_t PedDataFile::mod_get_cur_entry(int ct_num) {
    if (t_ped_file_ == NULL)
        return -1;
    if (mode_ == 'w')
        return -1;
    if (ct_num < 1 || ct_num > 25)
        return -1;
    return mod_cur_entry_[ct_num - 1];
}

Long64_t PedDataFile::mod_get_tot_entries(int ct_num) {
    if (t_ped_file_ == NULL)
        return -1;
    if (mode_ == 'w')
        return -1;
    if (ct_num < 1 || ct_num > 25)
        return -1;
    return t_ped_data_tree_[ct_num - 1]->GetEntries();
}

bool PedDataFile::mod_next(int ct_num) {
    if (t_ped_file_ == NULL)
        return false;
    if (mode_ == 'w')
        return false;
    if (ct_num < 1 || ct_num > 25)
        return false;
    int idx = ct_num - 1;
    if (mod_reach_end_[idx])
        return false;
    mod_cur_entry_[idx]++;
    if (mod_cur_entry_[idx] < t_ped_data_tree_[idx]->GetEntries()) {
        t_ped_data_tree_[idx]->GetEntry(mod_cur_entry_[idx]);
        return true;
    } else {
        mod_reach_end_[idx] = true;
        return false;
    }
}
