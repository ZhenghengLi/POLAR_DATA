#include <iostream>
#include "RootInc.hpp"
#include "CooConv.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << "<data_1M_file.root> <ped_vec.root> <tcanvas_output.root>" << endl;
        return 2;
    }
    string data_1M_fn = argv[1];
    string ped_vec_fn = argv[2];
    string tcanvas_fn = argv[3];

//    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);

    // open data_1M_file
    TFile* t_file_in = new TFile(data_1M_fn.c_str(), "read");
    if (t_file_in->IsZombie()) {
        cout << "root file of data_1M open failed." << endl;
        return 1;
    }
    TTree* t_modules_tree = static_cast<TTree*>(t_file_in->Get("t_modules"));
    if (t_modules_tree == NULL) {
        cout << "read t_modules failed." << endl;
        return 1;
    }

    struct {
        Int_t   ct_num;
        Int_t   is_bad;
        Int_t   compress;
        Bool_t  trigger_bit[64];
        Float_t energy_adc[64];
        Float_t common_noise;
    } t_modules;
    t_modules_tree->SetBranchAddress("ct_num",         &t_modules.ct_num             );
    t_modules_tree->SetBranchAddress("is_bad",         &t_modules.is_bad             );
    t_modules_tree->SetBranchAddress("compress",       &t_modules.compress           );
    t_modules_tree->SetBranchAddress("trigger_bit",     t_modules.trigger_bit        );
    t_modules_tree->SetBranchAddress("energy_adc",      t_modules.energy_adc         );
    t_modules_tree->SetBranchAddress("common_noise",   &t_modules.common_noise       );

    // open ped_vec file
    TFile* t_file_ped_vec;
    t_file_ped_vec = new TFile(ped_vec_fn.c_str(), "read");
    if (t_file_ped_vec->IsZombie()) {
        cout << "root file of ped_vec open failed." << endl;
        return 1;
    }

    TVectorF ped_mean_vec[25];
    for (int i = 0; i < 25; i++) {
        TVectorF* tmp_p = static_cast<TVectorF*>(t_file_ped_vec->Get(Form("ped_mean_vec_ct_%02d", i + 1)));
        if (tmp_p == NULL) {
            cout << "read ped_vec failed." << endl;
            return 1;
        }
        ped_mean_vec[i].ResizeTo(64);
        ped_mean_vec[i] = *tmp_p;
    }
    t_file_ped_vec->Close();

    TFile* t_file_out = new TFile(tcanvas_fn.c_str(), "recreate");
    if (t_file_out->IsZombie()) {
        cout << "root file of output open failed." << endl;
        return 1;
    }

    TH1F* tri_spec[25][64];
    TH1F* all_spec[25][64];
    TF1*  fun_spec[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            tri_spec[i][j] = new TH1F(Form("tri_spec_%02d_%02d", i + 1, j + 1), Form("tri_spec_%02d_%02d", i + 1, j + 1), 256, -128, 1280);
            // tri_spec[i][j]->SetDirectory(NULL);
            all_spec[i][j] = new TH1F(Form("all_spec_%02d_%02d", i + 1, j + 1), Form("all_spec_%02d_%02d", i + 1, j + 1), 256, -128, 1280);
            all_spec[i][j]->SetDirectory(NULL);
            fun_spec[i][j] = new TF1(Form("fun_spec_%02d_%02d", i + 1, j + 1), "(TMath::Erf((x - [0]) / TMath::Sqrt(2) / [1]) + 1.0) / 2.0", -128, 1024);
            fun_spec[i][j]->SetParameters(300.0, 2.0);
        }
    }

    Float_t tmp_energy_adc[64];
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data ..." << endl;
    cout << "[ " << flush;
    for (Long64_t i = 0; i < t_modules_tree->GetEntries(); i++) {
        cur_percent = static_cast<int>(i * 100 / t_modules_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_modules_tree->GetEntry(i);
        if (t_modules.is_bad > 0) continue;
        int idx = t_modules.ct_num - 1;
        // subtract pedestal and common noise
        float cur_common_sum = 0;
        float cur_common_n   = 0;
        float cur_common_noise = 0;
        for (int j = 0; j < 64; j++) {
            if (t_modules.energy_adc[j] < 4096) {
                if (t_modules.compress == 3) {
                    tmp_energy_adc[j] = t_modules.energy_adc[j];
                } else {
                    tmp_energy_adc[j] = t_modules.energy_adc[j] - ped_mean_vec[idx](j);
                    if (!t_modules.trigger_bit[j]) {
                        cur_common_sum += tmp_energy_adc[j];
                        cur_common_n   += 1;
                    }
                }
            }
        }
        if (t_modules.compress == 3) {
            cur_common_noise = t_modules.common_noise;
        } else {
            cur_common_noise = (cur_common_n > 0 ? cur_common_noise / cur_common_n : 0);
        }
        for (int j = 0; j < 64; j++) {
            if (tmp_energy_adc[j] < 4096) {
                tmp_energy_adc[j] -= cur_common_noise;
            } else {
                tmp_energy_adc[j] = gRandom->Uniform(-1, 1);
            }
        }
        for (int j = 0; j < 64; j++) {
            all_spec[idx][j]->Fill(tmp_energy_adc[j]);
            if (t_modules.trigger_bit[j]) {
                tri_spec[idx][j]->Fill(tmp_energy_adc[j]);
            }
        }
    }
    cout << " DONE ]" << endl;
    t_file_in->Close();
    delete t_file_in;
    t_file_in = NULL;

    // write spec
    gROOT->SetBatch(kTRUE);
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);
    TCanvas* canvas_spec[25];
    TVectorF vthr_adc_value[25];
    TVectorF vthr_adc_sigma[25];
    TVectorF vthr_adc_value_err[25];
    TVectorF vthr_adc_sigma_err[25];
    for (int i = 0; i < 25; i++) {
        canvas_spec[i] = new TCanvas(Form("canvas_spec_CT_%02d", i + 1), Form("canvas_spec_CT_%02d", i + 1), 2000, 1600);
        canvas_spec[i]->Divide(8, 8);
        canvas_spec[i]->SetFillColor(kYellow);
        vthr_adc_value[i].ResizeTo(64);
        vthr_adc_sigma[i].ResizeTo(64);
        vthr_adc_value_err[i].ResizeTo(64);
        vthr_adc_sigma_err[i].ResizeTo(64);
        t_file_out->mkdir(Form("trigger_ratio_CT_%02d", i + 1))->cd();
        for (int j = 0; j < 64; j++) {
            canvas_spec[i]->cd(jtoc(j));
            canvas_spec[i]->GetPad(jtoc(j))->SetFillColor(kWhite);
            tri_spec[i][j]->Divide(all_spec[i][j]);
            tri_spec[i][j]->Fit(fun_spec[i][j], "RQ");
            vthr_adc_value[i](j) = fun_spec[i][j]->GetParameter(0);
            vthr_adc_value_err[i](j) = fun_spec[i][j]->GetParError(0);
            vthr_adc_sigma[i](j) = fun_spec[i][j]->GetParameter(1);
            vthr_adc_sigma_err[i](j) = fun_spec[i][j]->GetParError(1);
            tri_spec[i][j]->Write();
        }
        t_file_out->cd();
        canvas_spec[i]->Write();
        vthr_adc_value[i].Write(Form("vthr_adc_value_CT_%02d", i + 1));
        vthr_adc_value_err[i].Write(Form("vthr_adc_value_err_CT_%02d", i + 1));
        vthr_adc_sigma[i].Write(Form("vthr_adc_sigma_CT_%02d", i + 1));
        vthr_adc_sigma_err[i].Write(Form("vthr_adc_sigma_err_CT_%02d", i + 1));
    }
    t_file_out->Close();
    delete t_file_out;
    t_file_out = NULL;

    return 0;
}

