#include "Gain.hpp"

using namespace std;

Gain::Gain() {
    gain_vs_hv_p0_mat_.ResizeTo(25, 64);
    gain_vs_hv_p1_mat_.ResizeTo(25, 64);
    bad_calib_mat.ResizeTo(25, 64);
    for (int i = 0; i < 25; i++) {
        adc_per_kev_CT_[i].ResizeTo(64);
        gain_vec_CT[i].ResizeTo(64);
    }
}

Gain::~Gain() {

}

bool Gain::read_gain_vec(const char* gain_vec_filename) {
    TFile* gain_vec_file = new TFile(gain_vec_filename, "read");
    if (gain_vec_file->IsZombie()) {
        cout << "gain_vec_file open failed: " << gain_vec_filename << endl;
        return false;
    }
    TMatrixF* tmp_mat;
    tmp_mat = static_cast<TMatrixF*>(gain_vec_file->Get("bad_calib_mat"));
    if (tmp_mat == NULL) {
        for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 64; j++) {
                bad_calib_mat(i, j) = 0;
            }
        }
    } else {
        bad_calib_mat = *tmp_mat;
    }
    TVectorF* tmp_vec;
    for (int i = 0; i < 25; i++) {
        tmp_vec = static_cast<TVectorF*>(gain_vec_file->Get(Form("adc_per_kev_ct_%02d", i + 1)));
        if (tmp_vec == NULL) { // for old format
            tmp_vec = static_cast<TVectorF*>(gain_vec_file->Get(Form("adc_per_kev_vec_ct_%02d", i + 1)));
        }
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("adc_per_kev_ct_%02d", i + 1) << endl;
            return false;
        } else {
            adc_per_kev_CT_[i] = *tmp_vec;
        }
    }
    gain_vec_file->Close();
    delete gain_vec_file;
    gain_vec_file = NULL;

    return true;
}

bool Gain::read_gain_vs_hv(const char* gain_vs_hv_filename) {
    TFile* gain_vs_hv_file = new TFile(gain_vs_hv_filename, "read");
    if (gain_vs_hv_file->IsZombie()) {
        cout << "gain_vs_hv_file open failed: " << gain_vs_hv_filename << endl;
        return false;
    }
    TMatrixF* tmp_mat;
    tmp_mat = static_cast<TMatrixF*>(gain_vs_hv_file->Get("bad_calib_mat"));
    if (tmp_mat == NULL) {
        cout << "cannot find TMatrixF bad_calib_mat." << endl;
        return false;
    } else {
        bad_calib_mat = *tmp_mat;
    }
    tmp_mat = static_cast<TMatrixF*>(gain_vs_hv_file->Get("gain_vs_hv_p0_mat"));
    if (tmp_mat == NULL) {
        cout << "cannot find TMatrixF gain_vs_hv_p0_mat." << endl;
        return false;
    } else {
        gain_vs_hv_p0_mat_ = *tmp_mat;
    }
    tmp_mat = static_cast<TMatrixF*>(gain_vs_hv_file->Get("gain_vs_hv_p1_mat"));
    if (tmp_mat == NULL) {
        cout << "cannot find TMatrixF gain_vs_hv_p1_mat." << endl;
        return false;
    } else {
        gain_vs_hv_p1_mat_ = *tmp_mat;
    }
    gain_vs_hv_file->Close();
    delete gain_vs_hv_file;
    gain_vs_hv_file = NULL;

    return true;
}

void Gain::gen_gain() {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            gain_vec_CT[i](j) = adc_per_kev_CT_[i](j);
        }
    }
}

void Gain::gen_gain(int ct_num, double hv) {
    int ct_idx = ct_num - 1;
    for (int j = 0; j < 64; j++) {
        gain_vec_CT[ct_idx](j) = TMath::Exp(gain_vs_hv_p0_mat_(ct_idx, j) + gain_vs_hv_p1_mat_(ct_idx, j) * TMath::Log(hv));
    }
}

