#include "Nonlinearity.hpp"

Nonlinearity::Nonlinearity() {
    for (int i = 0; i < 25; i++) {
        nonlin_norm_fun_p0_[i].ResizeTo(64);
        nonlin_norm_fun_p1_[i].ResizeTo(64);
        nonlin_norm_fun_p2_[i].ResizeTo(64);
        for (int j = 0; j < 64; j++) {
            nonlin_norm_fun[i][j] = NULL;
        }
    }
}

Nonlinearity::~Nonlinearity() {

}

bool Nonlinearity::read_nonlin_par(const char* nonlin_filename) {
    TFile* nonlin_file = new TFile(nonlin_filename, "read");
    if (nonlin_file->IsZombie()) {
        return false;
    }
    TVectorF* tmp_vec;
    for (int i = 0; i < 25; i++) {
        tmp_vec = static_cast<TVectorF*>(nonlin_file->Get(Form("nonlin_norm_fun_p0_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("nonlin_norm_fun_p0_%02d", i + 1) << endl;
            return false;
        } else {
            nonlin_norm_fun_p0_[i] = *tmp_vec;
        }
        tmp_vec = static_cast<TVectorF*>(nonlin_file->Get(Form("nonlin_norm_fun_p1_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("nonlin_norm_fun_p1_%02d", i + 1) << endl;
            return false;
        } else {
            nonlin_norm_fun_p1_[i] = *tmp_vec;
        }
        tmp_vec = static_cast<TVectorF*>(nonlin_file->Get(Form("nonlin_norm_fun_p2_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("nonlin_norm_fun_p2_%02d", i + 1) << endl;
            return false;
        } else {
            nonlin_norm_fun_p2_[i] = *tmp_vec;
        }
    }
    nonlin_file->Close();
    delete nonlin_file;
    nonlin_file = NULL;
    return true;
}

void Nonlinearity::gen_nonlin_fun() {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            if (nonlin_norm_fun[i][j] != NULL) {
                delete nonlin_norm_fun[i][j];
            }
            nonlin_norm_fun[i][j] = new TF1(
                    Form("nonlin_norm_fun_%02d_%02d", i + 1, j),
                    "[0] * (1 + [1] * x) * (1 + TMath::Erf(x / [2])) / 2",
                    0, 4096);
            nonlin_norm_fun[i][j]->SetParameters(
                    nonlin_norm_fun_p0_[i](j),
                    nonlin_norm_fun_p1_[i](j),
                    nonlin_norm_fun_p2_[i](j));
        }
    }
}

