#include "PedMeanCalc.hpp"

using namespace std;

PedMeanCalc::PedMeanCalc() {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ped_gaus_[i][j] = NULL;
            ped_hist_[i][j] = NULL;
        }
    }
    is_all_created_ = false;
    is_all_filled_  = false;
    is_all_fitted_  = false;
    is_all_read_    = false;
}

PedMeanCalc::~PedMeanCalc() {
    delete_ped_hist();
}

void PedMeanCalc::fill_ped_data(SciIterator& sciIter, PedDataFile& ped_data_file) {
    if (ped_data_file.get_mode() != 'w')
        return;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Filling pedestal data of all modules ..." << endl;
    cout << "[ " << flush;
    sciIter.ped_modules_set_start();
    while (sciIter.ped_modules_next()) {
        cur_percent = static_cast<int>(100 * sciIter.ped_modules_get_cur_entry() / sciIter.ped_modules_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        int idx = sciIter.t_ped_modules.ct_num - 1;
        copy(sciIter.t_ped_modules.energy_adc, sciIter.t_ped_modules.energy_adc + 64,
             ped_data_file.t_ped_data[idx].ped_adc);
        ped_data_file.mod_fill(idx);
    }
    cout << " DONE ]" << endl;
}

void PedMeanCalc::create_ped_hist() {
    if (is_all_created_)
        return;
    delete_ped_hist();
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            // histogram
            ped_hist_[i][j] = new TH1F(Form("ped_hist_%d_%d", i + 1, j + 1),
                                       Form("Pedestal of CH %d_%d", i + 1, j + 1),
                                       PED_BINS, 0, PED_MAX);
            ped_hist_[i][j]->SetDirectory(NULL);
            // function
            ped_gaus_[i][j] = new TF1(Form("ped_gaus_%d_%d", i + 1, j + 1), "gaus(0)", 0, PED_MAX);
            ped_gaus_[i][j]->SetParameters(1, 400, 25);
            ped_gaus_[i][j]->SetParName(0, "Amplitude");
            ped_gaus_[i][j]->SetParName(1, "Mean");
            ped_gaus_[i][j]->SetParName(2, "Sigma");
        }
    }
    is_all_created_ = true;
    is_all_filled_  = false;
    is_all_fitted_  = false;
}

void PedMeanCalc::delete_ped_hist() {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            if (ped_hist_[i][j] != NULL) {
                delete ped_hist_[i][j];
                ped_hist_[i][j] = NULL;
            }
            if (ped_gaus_[i][j] != NULL) {
                delete ped_gaus_[i][j];
                ped_gaus_[i][j] = NULL;
            }
        }
    }
    is_all_created_ = false;
    is_all_filled_ = false;
    is_all_fitted_ = false;
}

void PedMeanCalc::fill_ped_hist(PedDataFile& ped_data_file) {
    if (ped_data_file.get_mode() != 'r')
        return;
    if (!is_all_created_)
        return;
    if (is_all_filled_)
        return;
    for (int i = 0; i < 25; i++) {
        ped_data_file.mod_set_start(i);
        while (ped_data_file.mod_next(i)) {
            for (int j = 0; j < 64; j++) {
                ped_hist_[i][j]->Fill(ped_data_file.t_ped_data[i].ped_adc[j]);
            }
        }
    }
    is_all_filled_ = true;
}

void PedMeanCalc::fit_ped_hist() {
    if (!is_all_filled_)
        return;
    if (is_all_fitted_)
        return;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            mean_0[i][j]  = ped_hist_[i][j]->GetMean();
            sigma_0[i][j] = ped_hist_[i][j]->GetRMS();
            if (ped_hist_[i][j]->GetEntries() < 20) {
                cout << "- WARNING: entries of CH " << i + 1 << "_" << j + 1
                     << "is too small, use the arithmetic mean and sigma instead." << endl;
                mean[i][j] = mean_0[i][j];
                sigma[i][j] = sigma_0[i][j];
                continue;
            }
            Float_t min_adc = mean_0[i][j] - 200 > 0 ? mean_0[i][j] - 200 : 0;
            Float_t max_adc = mean_0[i][j] + 200 < PED_MAX ? mean_0[i][j] + 200 : PED_MAX;
            ped_gaus_[i][j]->SetParameter(1, mean_0[i][j]);
            ped_gaus_[i][j]->SetParameter(2, sigma_0[i][j] - 1);
            ped_gaus_[i][j]->SetRange(min_adc, max_adc);
            ped_hist_[i][j]->Fit(ped_gaus_[i][j], "RNQ");
            ped_hist_[i][j]->Fit(ped_gaus_[i][j], "RNQ");
            mean[i][j]  = ped_gaus_[i][j]->GetParameter(1);
            sigma[i][j] = abs(ped_gaus_[i][j]->GetParameter(2));
            if (mean[i][j] < 0 || mean[i][j] > PED_MAX) {
                cout << "- WARNING: mean of CH " << i + 1 << "_" << j + 1
                     << " is out of range, use the arithmetic mean instead." << endl;
                mean[i][j]  = mean_0[i][j];
                sigma[i][j] = sigma_0[i][j];
            } else if (sigma[i][j] / sigma_0[i][j] > 2.0) {
                cout << "- WARNING: sigma of CH " << i + 1 << "_" << j + 1
                     << " is too large, use the arithmetic mean instead." << endl;
                mean[i][j]  = mean_0[i][j];
                sigma[i][j] = sigma_0[i][j];
            }
        }
    }
    is_all_fitted_ = true;
}

void PedMeanCalc::draw_ped_map() {
    if (!is_all_fitted_)
        return;
    ped_map_ = static_cast<TH2F*>(gROOT->FindObject("ped_map"));
    if (ped_map_ == NULL) {
        ped_map_ = new TH2F("ped_map", "Pedestal Map of 1600 Channels", 40, 0, 40, 40, 0, 40);
        ped_map_->SetDirectory(NULL);
        ped_map_->GetXaxis()->SetNdivisions(40);
        ped_map_->GetYaxis()->SetNdivisions(40);
        for (int i = 0; i < 40; i++) {
            if (i % 8 == 0) {
                ped_map_->GetXaxis()->SetBinLabel(i + 1, Form("%d", i));
                ped_map_->GetYaxis()->SetBinLabel(i + 1, Form("%d", i));
            }
        }
        for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 64; j++) {
                ped_map_->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, mean[i][j]);
            }
        }
        
    }
    ped_map_->Draw("COLZ");
}

void PedMeanCalc::draw_ped_mean() {
    if (!is_all_read_)
        return;
    ped_mean_ = static_cast<TH2F*>(gROOT->FindObject("ped_mean"));
    if (ped_mean_ == NULL) {
        ped_mean_ = new TH2F("ped_mean", "Pedestal Mean of 1600 Channels", 40, 0, 40, 40, 0, 40);
        ped_mean_->SetDirectory(NULL);
        ped_mean_->GetXaxis()->SetNdivisions(40);
        ped_mean_->GetYaxis()->SetNdivisions(40);
        for (int i = 0; i < 40; i++) {
            if (i % 8 == 0) {
                ped_mean_->GetXaxis()->SetBinLabel(i + 1, Form("%d", i));
                ped_mean_->GetYaxis()->SetBinLabel(i + 1, Form("%d", i));
            }
        }
        for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 64; j++) {
                ped_mean_->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, mean[i][j]);
            }
        }
        
    }
    ped_mean_->Draw("COLZ");
}

void PedMeanCalc::draw_ped_sigma() {
    if (!is_all_read_)
        return;
    ped_sigma_ = static_cast<TH2F*>(gROOT->FindObject("ped_sigma"));
    if (ped_sigma_ == NULL) {
        ped_sigma_ = new TH2F("ped_sigma", "Pedestal Sigma of 1600 Channels", 40, 0, 40, 40, 0, 40);
        ped_sigma_->SetDirectory(NULL);
        ped_sigma_->GetXaxis()->SetNdivisions(40);
        ped_sigma_->GetYaxis()->SetNdivisions(40);
        for (int i = 0; i < 40; i++) {
            if (i % 8 == 0) {
                ped_sigma_->GetXaxis()->SetBinLabel(i + 1, Form("%d", i));
                ped_sigma_->GetYaxis()->SetBinLabel(i + 1, Form("%d", i));
            }
        }
        for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 64; j++) {
                ped_sigma_->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, sigma[i][j]);
            }
        }
        
    }
    ped_sigma_->Draw("COLZ");
}

void PedMeanCalc::draw_ped_hist(int ct_i, int ch_j) {
    if (!is_all_fitted_)
        return;
    if (ped_hist_[ct_i][ch_j]->GetEntries() < 20)
        return;
    ped_hist_[ct_i][ch_j]->Fit(ped_gaus_[ct_i][ch_j], "RQ");
}

bool PedMeanCalc::write_ped_vector(const char* filename, PedDataFile& ped_data_file) {
    if (!is_all_fitted_)
        return false;
    TFile* ped_vec_file = new TFile(filename, "RECREATE");
    if (ped_vec_file->IsZombie())
        return false;
    TVectorF* tmp_vec;
    for (int i = 0; i < 25; i++) {
        tmp_vec = new TVectorF(64, mean[i]);
        tmp_vec->Write(Form("ped_mean_vec_ct_%d", i + 1));
        delete tmp_vec;
        tmp_vec = new TVectorF(64, sigma[i]);
        tmp_vec->Write(Form("ped_sigma_vec_ct_%d", i + 1));
        delete tmp_vec;
        tmp_vec = NULL;
    }
    TNamed* tmp_meta;
    // m_dattype
    tmp_meta = new TNamed("m_dattype", "POLAR PEDESTAL VECTOR");
    tmp_meta->Write();
    delete tmp_meta;
    // m_version
    tmp_meta = new TNamed("m_version",  (SW_NAME + " " + SW_VERSION).c_str());
    tmp_meta->Write();
    delete tmp_meta;
    // m_gentime
    TTimeStamp* cur_time = new TTimeStamp();
    tmp_meta = new TNamed("m_gentime", cur_time->AsString("lc"));
    tmp_meta->Write();
    delete cur_time;
    cur_time = NULL;
    delete tmp_meta;
    // m_fromfile
    tmp_meta = new TNamed("m_fromfile", ped_data_file.get_fromfile_str().c_str());
    tmp_meta->Write();
    delete tmp_meta;
    // m_gps_span
    tmp_meta = new TNamed("m_gps_span", ped_data_file.get_gps_span_str().c_str());
    tmp_meta->Write();
    delete tmp_meta;
    tmp_meta = NULL;
    ped_vec_file->Close();
    delete ped_vec_file;
    ped_vec_file = NULL;
    return true;
}

bool PedMeanCalc::read_ped_vector(const char* filename) {
    TFile* ped_vec_file = new TFile(filename, "READ");
    if (ped_vec_file->IsZombie())
        return false;
    TVectorF* mean_vec;
    TVectorF* sigma_vec;
    for (int i = 0; i < 25; i++) {
        mean_vec  = static_cast<TVectorF*>(ped_vec_file->Get(Form("ped_mean_vec_ct_%d", i + 1)));
        sigma_vec = static_cast<TVectorF*>(ped_vec_file->Get(Form("ped_sigma_vec_ct_%d", i + 1)));
        if (mean_vec == NULL || sigma_vec == NULL)
            return false;
        for (int j = 0; j < 64; j++) {
            mean[i][j]  = (*mean_vec)(j);
            sigma[i][j] = (*sigma_vec)(j);
        }
        delete mean_vec;
        mean_vec = NULL;
        delete sigma_vec;
        sigma_vec = NULL;
    }
    ped_vec_file->Close();
    delete ped_vec_file;
    ped_vec_file = NULL;
    is_all_read_ = true;
    return true;
}
