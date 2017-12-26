#include <iostream>
#include "RootInc.hpp"
#include "CooConv.hpp"

using namespace std;

bool read_ped_temp(const char* filename,
        int& ct_num,
        TVectorF& ped_mean_vec,
        TVectorF& ped_mean_err,
        TVectorF& ped_sigma_vec,
        TVectorF& ped_sigma_err,
        TVectorF& ped_shift_sigma_vec,
        TVectorF& ped_shift_sigma_err,
        double& common_noise,
        double& common_noise_err,
        double& temp_mean) {

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

    // ped_mean_err
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("ped_mean_err"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF ped_mean_err" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        ped_mean_err = *tmp_vec;
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

    // ped_sigma_err
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("ped_sigma_err"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF ped_sigma_err" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        ped_sigma_err = *tmp_vec;
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

    // ped_shift_sigma_err
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("ped_shift_sigma_err"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF ped_shift_sigma_err" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        ped_shift_sigma_err = *tmp_vec;
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

    // common_noise_err
    tmp_tnamed = static_cast<TNamed*>(ped_res_file->Get("common_noise_err"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed common_noise_err" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        common_noise_err = TString(tmp_tnamed->GetTitle()).Atof();
    }

    // temp_mean
    double low_temp(0), high_temp(0);
    tmp_tnamed = static_cast<TNamed*>(ped_res_file->Get("low_temp"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed low_temp" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        low_temp = TString(tmp_tnamed->GetTitle()).Atof();
    }
    tmp_tnamed = static_cast<TNamed*>(ped_res_file->Get("high_temp"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed high_temp" << endl;
        ped_res_file->Close();
        delete ped_res_file;
        ped_res_file = NULL;
        return false;
    } else {
        high_temp = TString(tmp_tnamed->GetTitle()).Atof();
    }
    temp_mean = (low_temp + high_temp) / 2.0;

    ped_res_file->Close();
    delete ped_res_file;
    ped_res_file = NULL;
    return true;
}

double get_k(double x1, double y1, double x2, double y2) {
    return (y2 - y1) / (x2 - x1);
}

double get_b(double x1, double y1, double x2, double y2) {
    double k = (y2 - y1) / (x2 - x1);
    return y1 - k * x1;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <ped_temp.root> <ped_res_CT_01.root> <ped_res_CT_02.root> ..." << endl;
        return 2;
    }

    TGraphErrors* ped_temp_gr[64];
    TGraphErrors* total_noise_temp_gr[64];
    TGraphErrors* intrinsic_noise_temp_gr[64];
    TGraphErrors* common_noise_temp_gr;
    TF1*    ped_temp_ln[64];
    TF1*    total_noise_temp_ln[64];
    TF1*    intrinsic_noise_temp_ln[64];
    TF1*    common_noise_temp_ln;
    for (int j = 0; j < 64; j++) {
        ped_temp_gr[j] = new TGraphErrors();
        ped_temp_gr[j]->SetName(Form("ped_temp_ch_%02d", j));
        ped_temp_gr[j]->SetTitle(Form("ped_temp_ch_%02d", j));
        ped_temp_ln[j] = new TF1(Form("ped_temp_lin_%02d", j), "[0] + [1] * x", 0, 100);
        ped_temp_ln[j]->SetParameters(200, 1);
        total_noise_temp_gr[j] = new TGraphErrors();
        total_noise_temp_gr[j]->SetName(Form("total_noise_temp_ch_%02d", j));
        total_noise_temp_gr[j]->SetTitle(Form("total_noise_temp_ch_%02d", j));
        total_noise_temp_ln[j] = new TF1(Form("total_noise_temp_lin_%02d", j), "[0] + [1] * x", 0, 100);
        total_noise_temp_ln[j]->SetParameters(50, -1);
        intrinsic_noise_temp_gr[j] = new TGraphErrors();
        intrinsic_noise_temp_gr[j]->SetName(Form("intrinsic_noise_temp_ch_%02d", j));
        intrinsic_noise_temp_gr[j]->SetTitle(Form("intrinsic_noise_temp_ch_%02d", j));
        intrinsic_noise_temp_ln[j] = new TF1(Form("intrinsic_noise_temp_lin_%02d", j), "[0] + [1] * x", 0, 100);
        intrinsic_noise_temp_ln[j]->SetParameters(5, -0.01);
    }
    common_noise_temp_gr = new TGraphErrors();
    common_noise_temp_gr->SetName("common_noise_temp");
    common_noise_temp_gr->SetTitle("common_noise_temp");
    common_noise_temp_ln = new TF1("common_noise_temp_lin", "[0] + [1] * x", 0, 100);
    common_noise_temp_ln->SetParameters(30, 0.01);


    int ct_num;
    TVectorF ped_mean_vec(64);
    TVectorF ped_mean_err(64);
    TVectorF ped_sigma_vec(64);
    TVectorF ped_sigma_err(64);
    TVectorF ped_shift_sigma_vec(64);
    TVectorF ped_shift_sigma_err(64);
    double common_noise;
    double common_noise_err;
    double temp_mean;

    int g_ct_num = -1;
    float min_temp = 100;
    float max_temp = 0;

    for (int i = 2; i < argc; i++) {
        if (read_ped_temp(argv[i],
                    ct_num,
                    ped_mean_vec,
                    ped_mean_err,
                    ped_sigma_vec,
                    ped_sigma_err,
                    ped_shift_sigma_vec,
                    ped_shift_sigma_err,
                    common_noise,
                    common_noise_err,
                    temp_mean)) {
            cout << Form("ped_result of CT_%02d is read from file: ", ct_num) << argv[i] << endl;
            if (g_ct_num < 0) {
                g_ct_num = ct_num;
            } else {
                if (ct_num != g_ct_num) {
                    cout << "different ct_num." << endl;
                    return 1;
                }
            }
            if (min_temp > temp_mean) {
                min_temp = temp_mean;
            }
            if (max_temp < temp_mean) {
                max_temp = temp_mean;
            }
            for (int j = 0; j < 64; j++) {
                ped_temp_gr[j]->SetPoint(i - 2, temp_mean, ped_mean_vec(j));
                ped_temp_gr[j]->SetPointError(i - 2, 0.5, ped_mean_err(j));
                total_noise_temp_gr[j]->SetPoint(i - 2, temp_mean, ped_sigma_vec(j));
                total_noise_temp_gr[j]->SetPointError(i - 2, 0.5, ped_sigma_err(j));
                intrinsic_noise_temp_gr[j]->SetPoint(i - 2, temp_mean, ped_shift_sigma_vec(j));
                intrinsic_noise_temp_gr[j]->SetPointError(i - 2, 0.5, ped_shift_sigma_err(j));
            }
            common_noise_temp_gr->SetPoint(i - 2, temp_mean, common_noise);
            common_noise_temp_gr->SetPointError(i - 2, 0.5, common_noise_err);

        } else {
            return 1;
        }
    }

    gROOT->SetBatch(kTRUE);
    gErrorIgnoreLevel = kWarning;

    cout << "All ped_result are read, now do fitting and writting the result into file: " << argv[1] << " ..." << endl;
    // wrtie ped_vec
    TFile* ped_temp_file = new TFile(argv[1], "recreate");
    if (ped_temp_file->IsZombie()) {
        cout << "ped_temp_file to write open failed: " << argv[1] << endl;
        return 1;
    }
    ped_temp_file->cd();
    TNamed("ct_num", Form("%d", g_ct_num)).Write();

    // set init par and fit
    TVectorF ped_const(64);
    TVectorF ped_slope(64);
    TVectorF total_noise_const(64);
    TVectorF total_noise_slope(64);
    TVectorF intrinsic_noise_const(64);
    TVectorF intrinsic_noise_slope(64);
    double   common_noise_const;
    double   common_noise_slope;
    for (int j = 0; j < 64; j++) {
        ped_temp_ln[j]->SetRange(min_temp - 0.5, max_temp + 0.5);
        double min_ped = ped_temp_gr[j]->Eval(min_temp);
        double max_ped = ped_temp_gr[j]->Eval(max_temp);
        ped_temp_ln[j]->SetParameter(0, get_b(min_temp, min_ped, max_temp, max_ped));
        ped_temp_ln[j]->SetParameter(1, get_k(min_temp, min_ped, max_temp, max_ped));
        ped_temp_gr[j]->Fit(ped_temp_ln[j], "RQ");
        ped_const(j) = ped_temp_ln[j]->GetParameter(0);
        ped_slope(j) = ped_temp_ln[j]->GetParameter(1);

        total_noise_temp_ln[j]->SetRange(min_temp - 0.5, max_temp + 0.5);
        double min_total_noise = total_noise_temp_gr[j]->Eval(min_temp);
        double max_total_noise = total_noise_temp_gr[j]->Eval(max_temp);
        total_noise_temp_ln[j]->SetParameter(0, get_b(min_temp, min_total_noise, max_temp, max_total_noise));
        total_noise_temp_ln[j]->SetParameter(1, get_k(min_temp, min_total_noise, max_temp, max_total_noise));
        total_noise_temp_gr[j]->Fit(total_noise_temp_ln[j], "RQ");
        total_noise_const(j) = total_noise_temp_ln[j]->GetParameter(0);
        total_noise_slope(j) = total_noise_temp_ln[j]->GetParameter(1);

        intrinsic_noise_temp_ln[j]->SetRange(min_temp - 0.5, max_temp + 0.5);
        double min_intrinsic_noise = intrinsic_noise_temp_gr[j]->Eval(min_temp);
        double max_intrinsic_noise = intrinsic_noise_temp_gr[j]->Eval(max_temp);
        intrinsic_noise_temp_ln[j]->SetParameter(0, get_b(min_temp, min_intrinsic_noise, max_temp, max_intrinsic_noise));
        intrinsic_noise_temp_ln[j]->SetParameter(1, get_k(min_temp, min_intrinsic_noise, max_temp, max_intrinsic_noise));
        intrinsic_noise_temp_gr[j]->Fit(intrinsic_noise_temp_ln[j], "RQ");
        intrinsic_noise_const(j) = intrinsic_noise_temp_ln[j]->GetParameter(0);
        intrinsic_noise_slope(j) = intrinsic_noise_temp_ln[j]->GetParameter(1);

    }
    common_noise_temp_ln->SetRange(min_temp - 0.5, max_temp + 0.5);
    double min_common_noise = common_noise_temp_gr->Eval(min_temp);
    double max_common_noise = common_noise_temp_gr->Eval(max_temp);
    common_noise_temp_ln->SetParameter(0, get_b(min_temp, min_common_noise, max_temp, max_common_noise));
    common_noise_temp_ln->SetParameter(1, get_k(min_temp, min_common_noise, max_temp, max_common_noise));
    common_noise_temp_gr->Fit(common_noise_temp_ln, "RQ");
    common_noise_const = common_noise_temp_ln->GetParameter(0);
    common_noise_slope = common_noise_temp_ln->GetParameter(1);

    TCanvas* canvas_ped_temp = new TCanvas("canvas_ped_temp", "Pedestal vs Temperature", 900, 900);
    canvas_ped_temp->Divide(8, 8);
    TCanvas* canvas_total_noise_temp = new TCanvas("canvas_total_noise_temp", "Total Noise vs Temperature", 900, 900);
    canvas_total_noise_temp->Divide(8, 8);
    TCanvas* canvas_intrinsic_noise_temp = new TCanvas("canvas_intrinsic_noise_temp", "Total Noise vs Temperature", 900, 900);
    canvas_intrinsic_noise_temp->Divide(8, 8);
    for (int j = 0; j < 64; j++) {
        canvas_ped_temp->cd(jtoc(j));
        ped_temp_gr[j]->Draw("AP");
        canvas_total_noise_temp->cd(jtoc(j));
        total_noise_temp_gr[j]->Draw("AP");
        canvas_intrinsic_noise_temp->cd(jtoc(j));
        intrinsic_noise_temp_gr[j]->Draw("AP");
    }
    ped_temp_file->mkdir("ped_temp_graph")->cd();
    for (int j = 0; j < 64; j++) {
        ped_temp_gr[j]->Write();
    }
    ped_temp_file->mkdir("total_noise_temp_graph")->cd();
    for (int j = 0; j < 64; j++) {
        total_noise_temp_gr[j]->Write();
    }
    ped_temp_file->mkdir("intrinsic_noise_temp_graph")->cd();
    for (int j = 0; j < 64; j++) {
        intrinsic_noise_temp_gr[j]->Write();
    }
    ped_temp_file->cd();
    canvas_ped_temp->Write();
    canvas_total_noise_temp->Write();
    canvas_intrinsic_noise_temp->Write();
    common_noise_temp_gr->Write();
    ped_const.Write("ped_const");
    ped_slope.Write("ped_slope");
    total_noise_const.Write("total_noise_const");
    total_noise_slope.Write("total_noise_slope");
    intrinsic_noise_const.Write("intrinsic_noise_const");
    intrinsic_noise_slope.Write("intrinsic_noise_slope");
    TNamed("common_noise_const", Form("%f", common_noise_const)).Write();
    TNamed("common_noise_slope", Form("%f", common_noise_slope)).Write();

    return 0;
}
