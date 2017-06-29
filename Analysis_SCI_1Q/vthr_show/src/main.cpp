#include <iostream>
#include "RootInc.hpp"
#include "CommonCanvas.hpp"
#include "CooConv.hpp"

// for ADC
#define VTHR_VALUE_MAX 500
#define VTHR_SIGMA_MAX 60

// for energy
//#define VTHR_VALUE_MAX 32
//#define VTHR_SIGMA_MAX 16

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <vthr.root>" << endl;
        return 2;
    }

    TVectorF vthr_adc_value_ct[25];
    TVectorF vthr_adc_sigma_ct[25];
    for (int i = 0; i < 25; i++) {
        vthr_adc_value_ct[i].ResizeTo(64);
        vthr_adc_sigma_ct[i].ResizeTo(64);
    }

    // open vthr_file
    TFile* vthr_file = new TFile(argv[1], "read");
    if (vthr_file->IsZombie()) {
        cout << "vthr_file open failed: " << argv[1] << endl;
        return 1;
    }
    TVectorF* tmp_vec;
    // read vthr
    for (int i = 0; i < 25; i++) {
        tmp_vec = static_cast<TVectorF*>(vthr_file->Get(Form("vthr_adc_value_CT_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("vthr_adc_value_CT_%02d", i + 1) << endl;
            return 1;
        } else {
            vthr_adc_value_ct[i] = *tmp_vec;
        }
        tmp_vec = static_cast<TVectorF*>(vthr_file->Get(Form("vthr_adc_sigma_CT_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("vthr_adc_sigma_CT_%02d", i + 1) << endl;
            return 1;
        } else {
            vthr_adc_sigma_ct[i] = *tmp_vec;
        }
    }
    vthr_file->Close();
    delete vthr_file;
    vthr_file = NULL;

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);
    CommonCanvas canvas;
    gStyle->SetOptStat(0);
    TLine* line_h[4];
    TLine* line_v[4];
    for (int i = 0; i < 4; i++) {
        line_h[i] = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
        line_h[i]->SetLineColor(kWhite);
        line_v[i] = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
        line_v[i]->SetLineColor(kWhite);
    }
    // pedestal mean
    TH2D* vthr_value_map = new TH2D("vthr_value_map", "Threshold Value Map of 1600 Channels (view from BOTTOM)", 40, 0, 40, 40, 0, 40);
    vthr_value_map->SetDirectory(NULL);
    vthr_value_map->GetXaxis()->SetNdivisions(40);
    vthr_value_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            vthr_value_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            vthr_value_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            vthr_value_map->SetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1, vthr_adc_value_ct[i](j));
        }
    }
    canvas.cd(1);
    canvas.get_canvas()->GetPad(1)->SetGrid();
    vthr_value_map->DrawCopy("COLZ");
    for (int i = 0; i < 4; i++) {
        line_h[i]->Draw();
        line_v[i]->Draw();
    }
    // ped_shit
    TH2D* vthr_sigma_map = new TH2D("vthr_sigma_map", "Threshold Width Map of 1600 Channels (view from BOTTOM)", 40, 0, 40, 40, 0, 40);
    vthr_sigma_map->SetDirectory(NULL);
    vthr_sigma_map->GetXaxis()->SetNdivisions(40);
    vthr_sigma_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            vthr_sigma_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            vthr_sigma_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            vthr_sigma_map->SetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1, vthr_adc_sigma_ct[i](j));
        }
    }
    canvas.cd(2);
    canvas.get_canvas()->GetPad(2)->SetGrid();
    vthr_sigma_map->DrawCopy("COLZ");
    for (int i = 0; i < 4; i++) {
        line_h[i]->Draw();
        line_v[i]->Draw();
    }

    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);
    TH1D* vthr_value_dist = new TH1D("vthr_value_dist", "Distribution of Threshold Value", 64, 0, VTHR_VALUE_MAX);
    vthr_value_dist->SetDirectory(NULL);
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            vthr_value_dist->Fill(vthr_adc_value_ct[i](j));
        }
    }
    TF1* vthr_value_func = new TF1("vthr_value_func", "gaus(0)", 0, VTHR_VALUE_MAX);
    vthr_value_func->SetParameter(0, 1);
    vthr_value_func->SetParameter(1, vthr_value_dist->GetMean());
    vthr_value_func->SetParameter(2, vthr_value_dist->GetRMS() / 2);
    canvas.cd(3);
    vthr_value_dist->Fit(vthr_value_func, "RQ");

    TH1D* vthr_sigma_dist = new TH1D("vthr_sigma_dist", "Distribution of Threshold Sigma", 64, 0, VTHR_SIGMA_MAX);
    vthr_sigma_dist->SetDirectory(NULL);
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            vthr_sigma_dist->Fill(vthr_adc_sigma_ct[i](j));
        }
    }
    TF1* vthr_sigma_func = new TF1("vthr_sigma_func", "gaus(0)", 0, VTHR_SIGMA_MAX);
    vthr_sigma_func->SetParameter(0, 1);
    vthr_sigma_func->SetParameter(1, vthr_sigma_dist->GetMean());
    vthr_sigma_func->SetParameter(2, vthr_sigma_dist->GetRMS() / 2);
    canvas.cd(4);
    vthr_sigma_dist->Fit(vthr_sigma_func, "RQ");

    rootapp->Run();
    return 0;
}
