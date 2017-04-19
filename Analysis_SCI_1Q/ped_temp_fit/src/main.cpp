#include <iostream>
#include "RootInc.hpp"
#include "CooConv.hpp"

using namespace std;

bool read_ped_temp(const char* filename,
        int& ct_num,
        TVectorF& ped_mean_vec,
        float& temp_mean) {

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

    // temp_mean
    float low_temp(0), high_temp(0);
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

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <ped_temp.root> <ped_res_CT_01.root> <ped_res_CT_02.root> ..." << endl;
        return 2;
    }

    TGraph* ped_temp_gr[64];
    TF1*    ped_temp_ln[64];
    for (int j = 0; j < 64; j++) {
        ped_temp_gr[j] = new TGraph();
        ped_temp_gr[j]->SetName(Form("ped_temp_bar_%02d", j));
        ped_temp_gr[j]->SetTitle(Form("ped_temp_bar_%02d", j));
        ped_temp_ln[j] = new TF1(Form("ped_temp_lin_%02d", j), "[0] + [1] * x", 0, 100);
        ped_temp_ln[j]->SetParameters(200, 1);
    }

    int ct_num;
    TVectorF ped_mean_vec(64);
    float temp_mean;

    int g_ct_num = -1;
    float min_temp = 100;
    float max_temp = 0;

    for (int i = 2; i < argc; i++) {
        if (read_ped_temp(argv[i],
                    ct_num,
                    ped_mean_vec,
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
            }
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

    TVectorF ped_const(64);
    TVectorF ped_slope(64);
    TCanvas* canvas_ped_temp = new TCanvas("ped_temp_canvas", "Pedestal vs Temperature", 900, 900);
    canvas_ped_temp->Divide(8, 8);
    for (int j = 0; j < 64; j++) {
        canvas_ped_temp->cd(jtoc(j));
        ped_temp_ln[j]->SetRange(min_temp - 0.5, max_temp + 0.5);
        ped_temp_gr[j]->Fit(ped_temp_ln[j], "QR");
        ped_temp_gr[j]->Draw("AC*");
        ped_const(j) = ped_temp_ln[j]->GetParameter(0);
        ped_slope(j) = ped_temp_ln[j]->GetParameter(1);
    }
    ped_temp_file->mkdir("ped_temp_graph")->cd();
    for (int j = 0; j < 64; j++) {
        ped_temp_gr[j]->Write();
    }
    ped_temp_file->cd();
    canvas_ped_temp->Write();
    ped_const.Write("ped_const");
    ped_slope.Write("ped_slope");

    return 0;
}
