#include <iostream>
#include "RootInc.hpp"

using namespace std;

bool read_ped_vec(const char* filename,
        int& ct_num,
        TVectorF& ped_mean_vec,
        TVectorF& ped_sigma_vec,
        TVectorF& ped_shift_mean_vec,
        TVectorF& ped_shift_sigma_vec,
        float& common_noise) {

    TFile* ped_res_file = new TFile(filename, "read");
    if (ped_res_file->IsZombie()) {
        cout << "root file open failed: " << filename << endl;
        return false;
    }

    TNamed* tmp_tnamed;
    TVectorF* tmp_vec;

    // ct_num
    tmp_tnamed = static_cast<TNamed*>(ped_res_file->Get("ct_num"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed ct_num" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        ct_num = TString(tmp_tnamed->GetTitle()).Atoi();
    }

    // ped_mean_vec
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("ped_mean_vec"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF ped_mean_vec" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        ped_mean_vec = *tmp_vec;
    }

    // ped_sigma_vec
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("ped_sigma_vec"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF ped_sigma_vec" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        ped_sigma_vec = *tmp_vec;
    }

    // ped_shift_mean_vec
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("ped_shift_mean_vec"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF ped_shift_mean_vec" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        ped_shift_mean_vec = *tmp_vec;
    }

    // ped_shift_sigma_vec
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("ped_shift_sigma_vec"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF ped_shift_sigma_vec" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        ped_shift_sigma_vec = *tmp_vec;
    }

    // common_noise
    tmp_tnamed = static_cast<TNamed*>(ped_res_file->Get("common_noise"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed common_noise" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        common_noise = TString(tmp_tnamed->GetTitle()).Atof();
    }

    ped_res_file->Close();
    delete ped_res_file;
    ped_res_file = NULL;
    return true;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <ped_vec.root> <ped_res_CT_01.root> <ped_res_CT_02.root> ..." << endl;
        return 2;
    }
    TVectorF ped_mean_vec_ct[25];
    TVectorF ped_sigma_vec_ct[25];
    TVectorF ped_shift_mean_vec_ct[25];
    TVectorF ped_shift_sigma_vec_ct[25];
    bool read_flag[25];
    for (int i = 0; i < 25; i++) {
        ped_mean_vec_ct[i].ResizeTo(64);
        ped_sigma_vec_ct[i].ResizeTo(64);
        ped_shift_mean_vec_ct[i].ResizeTo(64);
        ped_shift_sigma_vec_ct[i].ResizeTo(64);
        read_flag[i] = false;
    }
    TVectorF common_noise_vec(25);
    int ct_num;
    TVectorF ped_mean_vec(64);
    TVectorF ped_sigma_vec(64);
    TVectorF ped_shift_mean_vec(64);
    TVectorF ped_shift_sigma_vec(64);
    float common_noise;
    for (int i = 2; i < argc; i++) {
        if (read_ped_vec(argv[i],
                    ct_num,
                    ped_mean_vec,
                    ped_sigma_vec,
                    ped_shift_mean_vec,
                    ped_shift_sigma_vec,
                    common_noise)) {
            cout << Form("ped_result of CT_%02d is read from file: ", ct_num) << argv[i] << endl;
            int idx = ct_num - 1;
            ped_mean_vec_ct[idx] = ped_mean_vec;
            ped_sigma_vec_ct[idx] = ped_sigma_vec;
            ped_shift_mean_vec_ct[idx] = ped_shift_mean_vec;
            ped_shift_sigma_vec_ct[idx] = ped_shift_sigma_vec;
            common_noise_vec(idx) = common_noise;
            read_flag[idx] = true;
        } else {
            return 1;
        }
    }
    for (int i = 0; i < 25; i++) {
        if (!read_flag[i]) {
            cout << Form("ped_result of CT_%02d is not read", i + 1) << endl;
            return 1;
        }
    }

    cout << "All ped_result are read, now writting them into file: " << argv[1] << " ..." << endl;
    // wrtie ped_vec
    TFile* ped_vec_file = new TFile(argv[1], "recreate");
    if (ped_vec_file->IsZombie()) {
        cout << "ped_vec_file to write open failed: " << argv[1] << endl;
        return 1;
    }
    ped_vec_file->cd();
    for (int i = 0; i < 25; i++) {
        ped_mean_vec_ct[i].Write(Form("ped_mean_vec_ct_%02d", i + 1));
        ped_sigma_vec_ct[i].Write(Form("ped_sigma_vec_ct_%02d", i + 1));
        ped_shift_mean_vec_ct[i].Write(Form("ped_shift_mean_vec_ct_%02d", i + 1));
        ped_shift_sigma_vec_ct[i].Write(Form("ped_shift_sigma_vec_ct_%02d", i + 1));
    }
    common_noise_vec.Write("common_noise_vec");
    ped_vec_file->Close();
    delete ped_vec_file;
    ped_vec_file = NULL;

    return 0;
}
