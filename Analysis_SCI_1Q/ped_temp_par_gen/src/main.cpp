#include <iostream>
#include "RootInc.hpp"

using namespace std;

bool read_ped_temp_par(const char* filename,
        int& ct_num,
        TVectorF& ped_const,
        TVectorF& ped_slope,
        TVectorF& total_noise_const,
        TVectorF& total_noise_slope,
        TVectorF& intrinsic_noise_const,
        TVectorF& intrinsic_noise_slope,
        double& common_noise_const,
        double& common_noise_slope) {

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

    // common_noise_const
    tmp_tnamed = static_cast<TNamed*>(ped_res_file->Get("common_noise_const"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed common_noise_const" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        common_noise_const = TString(tmp_tnamed->GetTitle()).Atof();
    }

    // common_noise_slope
    tmp_tnamed = static_cast<TNamed*>(ped_res_file->Get("common_noise_slope"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed common_noise_slope" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        common_noise_slope = TString(tmp_tnamed->GetTitle()).Atof();
    }

    // ped_const
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("ped_const"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF ped_const" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        ped_const = *tmp_vec;
    }

    // ped_slope
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("ped_slope"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF ped_slope" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        ped_slope = *tmp_vec;
    }

    // total_noise_const
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("total_noise_const"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF total_noise_const" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        total_noise_const = *tmp_vec;
    }

    // total_noise_slope
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("total_noise_slope"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF total_noise_slope" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        total_noise_slope = *tmp_vec;
    }

    // intrinsic_noise_const
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("intrinsic_noise_const"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF intrinsic_noise_const" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        intrinsic_noise_const = *tmp_vec;
    }

    // intrinsic_noise_slope
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("intrinsic_noise_slope"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF intrinsic_noise_slope" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        intrinsic_noise_slope = *tmp_vec;
    }

    ped_res_file->Close();
    delete ped_res_file;
    ped_res_file = NULL;
    return true;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <ped_temp_par.root> <ped_res_CT_01.root> <ped_res_CT_02.root> ..." << endl;
        return 2;
    }
    TVectorF ped_const_vec_ct[25];
    TVectorF ped_slope_vec_ct[25];
    TVectorF total_noise_const_vec_ct[25];
    TVectorF total_noise_slope_vec_ct[25];
    TVectorF intrinsic_noise_const_vec_ct[25];
    TVectorF intrinsic_noise_slope_vec_ct[25];
    TVectorF common_noise_const_vec(25);
    TVectorF common_noise_slope_vec(25);
    bool read_flag[25];
    for (int i = 0; i < 25; i++) {
        ped_const_vec_ct[i].ResizeTo(64);
        ped_slope_vec_ct[i].ResizeTo(64);
        total_noise_const_vec_ct[i].ResizeTo(64);
        total_noise_slope_vec_ct[i].ResizeTo(64);
        intrinsic_noise_const_vec_ct[i].ResizeTo(64);
        intrinsic_noise_slope_vec_ct[i].ResizeTo(64);
        read_flag[i] = false;
    }
    int ct_num;
    TVectorF ped_const_vec(64);
    TVectorF ped_slope_vec(64);
    TVectorF total_noise_const_vec(64);
    TVectorF total_noise_slope_vec(64);
    TVectorF intrinsic_noise_const_vec(64);
    TVectorF intrinsic_noise_slope_vec(64);
    double common_noise_const;
    double common_noise_slope;
    for (int i = 2; i < argc; i++) {
        if (read_ped_temp_par(argv[i],
                    ct_num,
                    ped_const_vec,
                    ped_slope_vec,
                    total_noise_const_vec,
                    total_noise_slope_vec,
                    intrinsic_noise_const_vec,
                    intrinsic_noise_slope_vec,
                    common_noise_const,
                    common_noise_slope)) {
            cout << Form("ped_temp_par of CT_%02d is read from file: ", ct_num) << argv[i] << endl;
            int idx = ct_num - 1;
            ped_const_vec_ct[idx] = ped_const_vec;
            ped_slope_vec_ct[idx] = ped_slope_vec;
            total_noise_const_vec_ct[idx] = total_noise_const_vec;
            total_noise_slope_vec_ct[idx] = total_noise_slope_vec;
            intrinsic_noise_const_vec_ct[idx] = intrinsic_noise_const_vec;
            intrinsic_noise_slope_vec_ct[idx] = intrinsic_noise_slope_vec;
            common_noise_const_vec[idx] = common_noise_const;
            common_noise_slope_vec[idx] = common_noise_slope;
            read_flag[idx] = true;
        } else {
            return 1;
        }
    }
    for (int i = 0; i < 25; i++) {
        if (!read_flag[i]) {
            cout << Form("ped_temp_par of CT_%02d is not read", i + 1) << endl;
            return 1;
        }
    }

    cout << "All ped_temp_par are read, now writting them into file: " << argv[1] << " ..." << endl;
    // wrtie ped_vec
    TFile* ped_temp_par_file = new TFile(argv[1], "recreate");
    if (ped_temp_par_file->IsZombie()) {
        cout << "ped_temp_par_file to write open failed: " << argv[1] << endl;
        return 1;
    }
    ped_temp_par_file->cd();
    for (int i = 0; i < 25; i++) {
        ped_const_vec_ct[i].Write(Form("ped_const_vec_ct_%02d", i + 1));
        ped_slope_vec_ct[i].Write(Form("ped_slope_vec_ct_%02d", i + 1));
        total_noise_const_vec_ct[i].Write(Form("total_noise_const_vec_ct_%02d", i + 1));
        total_noise_slope_vec_ct[i].Write(Form("total_noise_slope_vec_ct_%02d", i + 1));
        intrinsic_noise_const_vec_ct[i].Write(Form("intrinsic_noise_const_vec_ct_%02d", i + 1));
        intrinsic_noise_slope_vec_ct[i].Write(Form("intrinsic_noise_slope_vec_ct_%02d", i + 1));
    }
    common_noise_const_vec.Write("common_noise_const_vec");
    common_noise_slope_vec.Write("common_noise_slope_vec");
    ped_temp_par_file->Close();
    delete ped_temp_par_file;
    ped_temp_par_file = NULL;

    return 0;
}
