#include <iostream>
#include "RootInc.hpp"

using namespace std;

bool read_xtalk_mat(const char* filename,
        int& ct_num,
        TMatrixF& xtalk_mat,
        TMatrixF& xtalk_mat_err,
        TMatrixF& xtalk_mat_inv) {
    TFile* xtalk_res_file = new TFile(filename, "read");
    if (xtalk_res_file->IsZombie()) {
        cout << "root file open failed: " << filename << endl;
        return false;
    }

    TNamed* tmp_tnamed;
    TMatrixF* tmp_mat;

    // ct_num
    tmp_tnamed = static_cast<TNamed*>(xtalk_res_file->Get("ct_num"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed ct_num" << endl;
        xtalk_res_file->Close();
        delete xtalk_res_file;
        xtalk_res_file = NULL;
        return false;
    } else {
        ct_num = TString(tmp_tnamed->GetTitle()).Atoi();
    }

    // xtalk_matrix
    tmp_mat = static_cast<TMatrixF*>(xtalk_res_file->Get("xtalk_matrix"));
    if (tmp_mat == NULL) {
        cout << "cannot find TMatrixF xtalk_matrix" << endl;
        xtalk_res_file->Close();
        delete xtalk_res_file;
        xtalk_res_file = NULL;
        return false;
    } else {
        xtalk_mat = *tmp_mat;
    }

    // xtalk_matrix_err
    tmp_mat = static_cast<TMatrixF*>(xtalk_res_file->Get("xtalk_matrix_err"));
    if (tmp_mat == NULL) {
        cout << "cannot find TMatrixF xtalk_matrix_err" << endl;
        xtalk_res_file->Close();
        delete xtalk_res_file;
        xtalk_res_file = NULL;
        return false;
    } else {
        xtalk_mat_err = *tmp_mat;
    }

    // xtalk_matrix_inv
    tmp_mat = static_cast<TMatrixF*>(xtalk_res_file->Get("xtalk_matrix_inv"));
    if (tmp_mat == NULL) {
        cout << "cannot find TMatrixF xtalk_matrix_inv" << endl;
        xtalk_res_file->Close();
        delete xtalk_res_file;
        xtalk_res_file = NULL;
        return false;
    } else {
        xtalk_mat_inv = *tmp_mat;
    }

    xtalk_res_file->Close();
    delete xtalk_res_file;
    xtalk_res_file = NULL;
    return true;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <xtalk_mat.root> <xtalk_res_CT_01.root> <xtalk_res_CT_02.root> ..." << endl;
        return 2;
    }
    TMatrixF xtalk_mat_ct[25];
    TMatrixF xtalk_mat_err_ct[25];
    TMatrixF xtalk_mat_inv_ct[25];
    bool read_flag[25];
    for (int i = 0; i < 25; i++) {
        xtalk_mat_ct[i].ResizeTo(64, 64);
        xtalk_mat_err_ct[i].ResizeTo(64, 64);
        xtalk_mat_inv_ct[i].ResizeTo(64, 64);
        read_flag[i] = false;
    }
    TVectorF common_noise_vec(25);
    int ct_num;
    TMatrixF xtalk_mat(64, 64);
    TMatrixF xtalk_mat_err(64, 64);
    TMatrixF xtalk_mat_inv(64, 64);
    for (int i = 2; i < argc; i++) {
        if (read_xtalk_mat(argv[i],
                    ct_num,
                    xtalk_mat,
                    xtalk_mat_err,
                    xtalk_mat_inv)) {
            cout << Form("xtalk_result of CT_%02d is read from file: ", ct_num) << argv[i] << endl;
            int idx = ct_num - 1;
            xtalk_mat_ct[idx] = xtalk_mat;
            xtalk_mat_err_ct[idx] = xtalk_mat_err;
            xtalk_mat_inv_ct[idx] = xtalk_mat_inv;
            read_flag[idx] = true;
        } else {
            return 1;
        }
    }
    for (int i = 0; i < 25; i++) {
        if (!read_flag[i]) {
            cout << Form("xtalk_result of CT_%02d is not read", i + 1) << endl;
            return 1;
        }
    }

    cout << "All xtalk_result are read, now writting them into file: " << argv[1] << " ..." << endl;
    // wrtie ped_vec
    TFile* xtalk_mat_file = new TFile(argv[1], "recreate");
    if (xtalk_mat_file->IsZombie()) {
        cout << "xtalk_mat_file to write open failed: " << argv[1] << endl;
        return 1;
    }
    xtalk_mat_file->cd();
    for (int i = 0; i < 25; i++) {
        xtalk_mat_ct[i].Write(Form("xtalk_mat_ct_%02d", i + 1));
        xtalk_mat_err_ct[i].Write(Form("xtalk_mat_err_ct_%02d", i + 1));
        xtalk_mat_inv_ct[i].Write(Form("xtalk_mat_inv_ct_%02d", i + 1));
    }
    xtalk_mat_file->Close();
    delete xtalk_mat_file;
    xtalk_mat_file = NULL;

    return 0;
}
