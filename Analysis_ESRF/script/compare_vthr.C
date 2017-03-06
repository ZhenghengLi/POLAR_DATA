#include <iostream>
#include "TFile.h"
#include "TVector.h"
#include "TH1.h"
#include "TCanvas.h"
#include "/home/mark/IHEP_Work/POLAR/POLAR_DATA/Analysis_ESRF/lib/include/CooConv.hpp"

using namespace std;

void compare_vthr() {
    string vthr_filename_1;
    cout << "vthr_filename_1 = " << flush;
    cin >> vthr_filename_1;
    TFile* vthr_file_1 = new TFile(vthr_filename_1.c_str(), "read");
    if (vthr_file_1->IsZombie()) {
        cout << "vthr_file_1 open failed." << endl;
        return;
    }
    TVectorF* tmp_vec_1;
    TVectorF vthr_adc_value_1[25];
    for (int i = 0; i < 25; i++) {
        vthr_adc_value_1[i].ResizeTo(64);
        tmp_vec_1 = static_cast<TVectorF*>(vthr_file_1->Get(Form("vthr_adc_value_CT_%02d", i + 1)));
        if (tmp_vec_1 == NULL) {
            cout << "cannot find TVectorF " << Form("vthr_adc_value_CT_%02d", i + 1) << endl;
            return;
        } else {
            vthr_adc_value_1[i] = *tmp_vec_1;
        }
    }
    string vthr_filename_2;
    cout << "vthr_filename_2 = " << flush;
    cin >> vthr_filename_2;
    TFile* vthr_file_2 = new TFile(vthr_filename_2.c_str(), "read");
    if (vthr_file_2->IsZombie()) {
        cout << "vthr_file_2 open failed." << endl;
        return;
    }
    TVectorF* tmp_vec_2;
    TVectorF vthr_adc_value_2[25];
    for (int i = 0; i < 25; i++) {
        vthr_adc_value_2[i].ResizeTo(64);
        tmp_vec_2 = static_cast<TVectorF*>(vthr_file_2->Get(Form("vthr_adc_value_CT_%02d", i + 1)));
        if (tmp_vec_2 == NULL) {
            cout << "cannot find TVectorF " << Form("vthr_adc_value_CT_%02d", i + 1) << endl;
            return;
        } else {
            vthr_adc_value_2[i] = *tmp_vec_2;
        }
    }

    TH1F* h_ratio = new TH1F("h_ratio", "h_ratio", 100, 0.8, 1.2);
    h_ratio->SetDirectory(NULL);
    for (int i = 0; i < 25; i++) {
        if (i == 1 || i == 7 || i == 8 || i == 3) {
            continue;
        }
        for (int j = 0; j < 64; j++) {
            if (vthr_adc_value_1[i][j] > 0 && vthr_adc_value_2[i][j] > 0) {
                h_ratio->Fill(vthr_adc_value_1[i][j] / vthr_adc_value_2[i][j]);
            }
        }
    }
    new TCanvas();
    h_ratio->Draw("H");

}
