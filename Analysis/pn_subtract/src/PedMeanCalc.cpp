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
}

PedMeanCalc::~PedMeanCalc() {
    delete_ped_hist();
}

void PedMeanCalc::fill_ped_data(SciIterator& sciIter, PedDataFile& ped_data_file) {
    sciIter.ped_modules_set_start();
    while (sciIter.ped_modules_next()) {
        int idx = sciIter.t_ped_modules.ct_num - 1;
        copy(sciIter.t_ped_modules.energy_adc, sciIter.t_ped_modules.energy_adc + 64,
             ped_data_file.t_ped_data[idx].ped_adc);
        ped_data_file.mod_fill(idx);
    }
}

void PedMeanCalc::create_ped_hist() {
    delete_ped_hist();
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            // histogram
            ped_hist_[i][j] = new TH1F(Form("ped_hist_%d_%d", i + 1, j + 1),
                                       Form("Pedestal of CH %d_%d", i + 1, j + 1),
                                       PED_BINS, 0, PED_MAX);
            // function
            ped_gaus_[i][j] = new TF1(Form("ped_gaus_%d_%d", i + 1, j + 1), "gaus(0)", 0, PED_MAX);
            ped_gaus_[i][j]->SetParameters(1, 400, 25);
            ped_gaus_[i][j]->SetParName(0, "Amplitude");
            ped_gaus_[i][j]->SetParName(1, "Mean");
            ped_gaus_[i][j]->SetParName(2, "Sigma");
        }
    }
    is_all_created_ = true;
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
}

