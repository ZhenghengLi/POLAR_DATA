#include <iostream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "SciIterator.hpp"
#include "CommonCanvas.hpp"
#include "CooConv.hpp"

#define PED_BINS 256
#define PED_MAX  1024

using namespace std;

int main(int argc, char** argv) {
    OptionsManager options_mgr;
    if (!options_mgr.parse(argc, argv)) {
        if (options_mgr.get_version_flag()) {
            options_mgr.print_version();
        } else {
            options_mgr.print_help();
        }
        return 1;
    }

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);
    // open file
    SciIterator sciIter;
    if (!sciIter.open(options_mgr.decoded_data_filename.Data(),
                options_mgr.begin_gps.Data(), options_mgr.end_gps.Data())) {
        cout << "sci root file open failed." << endl;
        return false;
    }
    sciIter.print_file_info();
    // open output file
    TFile* t_file_out = NULL;
    if (!options_mgr.output_filename.IsNull()) {
        t_file_out = new TFile(options_mgr.output_filename.Data(), "recreate");
        if (t_file_out->IsZombie()) {
            cout << "output root file open failed." << endl;
            sciIter.close();
            return false;
        }
    }
    // prepare histogram
    TH1D* ped_hist[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ped_hist[i][j] = new TH1D(Form("ped_hist_%02d_%02d", i + 1, j + 1),
                    Form("Pedestal of CH %02d_%02d", i + 1, j + 1),
                    PED_BINS, 0, PED_MAX);
            ped_hist[i][j]->SetDirectory(t_file_out);
        }
    }
    // read ped_data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Filling pedestal data of all modules ..." << endl;
    cout << "[ " << flush;
    sciIter.ped_modules_set_start();
    while (sciIter.ped_modules_next()) {
        cur_percent = static_cast<int>(100 * sciIter.ped_modules_get_cur_entry() / sciIter.ped_modules_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        int idx = sciIter.t_ped_modules.ct_num - 1;
        for (int j = 0; j < 64; j++) {
            ped_hist[idx][j]->Fill(sciIter.t_ped_modules.energy_adc[j]);
        }
    }
    cout << " DONE ]" << endl;

    gROOT->SetBatch(kTRUE);
    gErrorIgnoreLevel = kWarning;
    // fit pedestal
    cout << "Fitting pedestal ..." << endl;
    double ped_mean_0[25][64], ped_mean[25][64];
    double ped_sigma_0[25][64], ped_sigma[25][64];
    TF1* ped_gaus[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ped_gaus[i][j] = new TF1(Form("ped_gaus_%02d_%02d", i + 1, j + 1), "gaus(0)", 0, PED_MAX);
            ped_gaus[i][j]->SetParameters(1, 400, 25);
            ped_gaus[i][j]->SetParName(0, "Amplitude");
            ped_gaus[i][j]->SetParName(1, "Mean");
            ped_gaus[i][j]->SetParName(2, "Sigma");
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ped_mean_0[i][j]  = ped_hist[i][j]->GetMean();
            ped_sigma_0[i][j] = ped_hist[i][j]->GetRMS();
            if (ped_hist[i][j]->GetEntries() < 20) {
                cout << "- WARNING: entries of CH " << i + 1 << "_" << j + 1
                     << "is too small, use the arithmetic mean and sigma instead." << endl;
                ped_mean[i][j] = ped_mean_0[i][j];
                ped_sigma[i][j] = ped_sigma_0[i][j];
                continue;
            }
            Float_t min_adc = ped_mean_0[i][j] - 200 > 0 ? ped_mean_0[i][j] - 200 : 0;
            Float_t max_adc = ped_mean_0[i][j] + 200 < PED_MAX ? ped_mean_0[i][j] + 200 : PED_MAX;
            ped_gaus[i][j]->SetParameter(1, ped_mean_0[i][j]);
            ped_gaus[i][j]->SetParameter(2, ped_sigma_0[i][j] - 1);
            ped_gaus[i][j]->SetRange(min_adc, max_adc);
            ped_hist[i][j]->Fit(ped_gaus[i][j], "RQ");
            ped_hist[i][j]->Fit(ped_gaus[i][j], "RQ");
            ped_mean[i][j]  = ped_gaus[i][j]->GetParameter(1);
            ped_sigma[i][j] = abs(ped_gaus[i][j]->GetParameter(2));
            if (ped_mean[i][j] < 0 || ped_mean[i][j] > PED_MAX) {
                cout << "- WARNING: ped_mean of CH " << i + 1 << "_" << j + 1
                     << " is out of range, use the arithmetic mean instead." << endl;
                ped_mean[i][j]  = ped_mean_0[i][j];
                ped_sigma[i][j] = ped_sigma_0[i][j];
            } else if (ped_sigma[i][j] / ped_sigma_0[i][j] > 2.0) {
                cout << "- WARNING: ped_sigma of CH " << i + 1 << "_" << j + 1
                     << " is too large, use the arithmetic mean instead." << endl;
                ped_mean[i][j]  = ped_mean_0[i][j];
                ped_sigma[i][j] = ped_sigma_0[i][j];
            }
        }
    }
    // subtract common noise and refit
    // prepare histogram
    TH1D* common_noise_hist[25];
    for (int i = 0; i < 25; i++) {
        common_noise_hist[i] = new TH1D(Form("common_noise_%02d", i + 1),
                Form("Common noise of Module CT_%02d", i + 1), 256, -128, 128);
        common_noise_hist[i]->SetDirectory(t_file_out);
    }
    TH1D* ped_shift_hist[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ped_shift_hist[i][j] = new TH1D(Form("ped_shift_hist_%02d_%02d", i + 1, j + 1),
                    Form("Pedestal shift of CH %02d_%02d", i + 1, j + 1),
                    256, -128, 128);
            ped_shift_hist[i][j]->SetDirectory(t_file_out);
        }
    }
    // read ped_data
    Float_t tmp_energy_adc[64];
    double  common_noise_sum;
    int     common_noise_n;
    double  cur_common_noise;
    pre_percent = 0;
    cur_percent = 0;
    cout << "Filling pedestal data of all modules and subtract common noise ..." << endl;
    cout << "[ " << flush;
    sciIter.ped_modules_set_start();
    while (sciIter.ped_modules_next()) {
        cur_percent = static_cast<int>(100 * sciIter.ped_modules_get_cur_entry() / sciIter.ped_modules_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        int idx = sciIter.t_ped_modules.ct_num - 1;
        copy(sciIter.t_ped_modules.energy_adc, sciIter.t_ped_modules.energy_adc + 64, tmp_energy_adc);
        // subtract pedestal, calculate common noise
        common_noise_sum = 0;
        common_noise_n   = 0;
        for (int j = 0; j < 64; j++) {
            tmp_energy_adc[j] -= ped_mean[idx][j];
            if (!sciIter.t_ped_modules.trigger_bit[j]) {
                common_noise_sum += tmp_energy_adc[j];
                common_noise_n++;
            }
        }
        cur_common_noise = (common_noise_n > 0 ? common_noise_sum / common_noise_n : 0);
        if (common_noise_n > 0)
            common_noise_hist[idx]->Fill(cur_common_noise);
        // subtract common noise and fill
        for (int j = 0; j < 64; j++) {
            tmp_energy_adc[j] -= cur_common_noise;
            ped_shift_hist[idx][j]->Fill(tmp_energy_adc[j]);
        }
    }
    cout << " DONE ]" << endl;
    // refit
    cout << "Refitting after common noise subtracted ..." << endl;
    double ped_shift_mean_0[25][64], ped_shift_mean[25][64];
    double ped_shift_sigma_0[25][64], ped_shift_sigma[25][64];
    TF1* ped_shift_gaus[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ped_shift_gaus[i][j] = new TF1(Form("ped_shift_gaus_%02d_%02d", i + 1, j + 1), "gaus(0)", -32, 32);
            ped_shift_gaus[i][j]->SetParameters(1, 0, 25);
            ped_shift_gaus[i][j]->SetParName(0, "Amplitude");
            ped_shift_gaus[i][j]->SetParName(1, "Mean");
            ped_shift_gaus[i][j]->SetParName(2, "Sigma");
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ped_shift_mean_0[i][j]  = ped_shift_hist[i][j]->GetMean();
            ped_shift_sigma_0[i][j] = ped_shift_hist[i][j]->GetRMS();
            if (ped_shift_hist[i][j]->GetEntries() < 20) {
                cout << "- WARNING: entries of CH " << i + 1 << "_" << j + 1
                     << "is too small, use the arithmetic mean and ped_sigma instead." << endl;
                ped_shift_mean[i][j] = ped_shift_mean_0[i][j];
                ped_shift_sigma[i][j] = ped_shift_sigma_0[i][j];
                continue;
            }
            ped_shift_gaus[i][j]->SetParameter(1, ped_shift_mean_0[i][j]);
            ped_shift_gaus[i][j]->SetParameter(2, ped_shift_sigma_0[i][j] - 1);
            ped_shift_hist[i][j]->Fit(ped_shift_gaus[i][j], "RQ");
            ped_shift_hist[i][j]->Fit(ped_shift_gaus[i][j], "RQ");
            ped_shift_mean[i][j]  = ped_shift_gaus[i][j]->GetParameter(1);
            ped_shift_sigma[i][j] = abs(ped_shift_gaus[i][j]->GetParameter(2));
            if (ped_shift_mean[i][j] < -128 || ped_shift_mean[i][j] > 128) {
                cout << "- WARNING: ped_shift_mean of CH " << i + 1 << "_" << j + 1
                     << " is out of range, use the arithmetic mean instead." << endl;
                ped_shift_mean[i][j]  = ped_shift_mean_0[i][j];
                ped_shift_sigma[i][j] = ped_shift_sigma_0[i][j];
            } else if (ped_shift_sigma[i][j] / ped_shift_sigma_0[i][j] > 2.0) {
                cout << "- WARNING: ped_shift_sigma of CH " << i + 1 << "_" << j + 1
                     << " is too large, use the arithmetic mean instead." << endl;
                ped_shift_mean[i][j]  = ped_shift_mean_0[i][j];
                ped_shift_sigma[i][j] = ped_shift_sigma_0[i][j];
            }
        }
    }
    // draw
    gROOT->SetBatch(kFALSE);
    gStyle->SetOptStat(0);
    CommonCanvas canvas_noise;
    TLine* line_h[4];
    TLine* line_v[4];
    for (int i = 0; i < 4; i++) {
        line_h[i] = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
        line_h[i]->SetLineColor(kWhite);
        line_v[i] = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
        line_v[i]->SetLineColor(kWhite);
    }
    // pedestal mean
    TH2D* ped_map = new TH2D("ped_map", "Pedestal Map of 1600 Channels", 40, 0, 40, 40, 0, 40);
    ped_map->SetDirectory(NULL);
    ped_map->GetXaxis()->SetNdivisions(40);
    ped_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            ped_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            ped_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ped_map->SetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1, ped_mean[i][j]);
        }
    }
    canvas_noise.cd(1);
    canvas_noise.get_canvas()->GetPad(1)->SetGrid();
    ped_map->DrawCopy("COLZ");
    for (int i = 0; i < 4; i++) {
        line_h[i]->Draw();
        line_v[i]->Draw();
    }
    // common noise
    canvas_noise.cd(2);
    TPad* common_noise_pad = static_cast<TPad*>(canvas_noise.get_canvas()->GetPad(2));
    common_noise_pad->Divide(5, 5);
    for (int i = 0; i < 25; i++) {
        common_noise_pad->cd(itocb(i));
        common_noise_hist[i]->DrawCopy("eh");
    }
    // ped_shit
    TH2D* ped_shift_map = new TH2D("ped_shift_map", "Pedestal Shift Map of 1600 Channels After Common Noise Subtracted", 40, 0, 40, 40, 0, 40);
    ped_shift_map->SetDirectory(NULL);
    ped_shift_map->GetXaxis()->SetNdivisions(40);
    ped_shift_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            ped_shift_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            ped_shift_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ped_shift_map->SetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1, ped_shift_mean[i][j]);
        }
    }
    canvas_noise.cd(3);
    canvas_noise.get_canvas()->GetPad(3)->SetGrid();
    ped_shift_map->DrawCopy("COLZ");
    for (int i = 0; i < 4; i++) {
        line_h[i]->Draw();
        line_v[i]->Draw();
    }
    // intrinsic noise
    TH2D* intrinsic_noise_map = new TH2D("intrinsic_noise_map", "Intrinsic Noise Map of 1600 Channels", 40, 0, 40, 40, 0, 40);
    intrinsic_noise_map->SetDirectory(NULL);
    intrinsic_noise_map->GetXaxis()->SetNdivisions(40);
    intrinsic_noise_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            intrinsic_noise_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            intrinsic_noise_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            intrinsic_noise_map->SetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1, ped_shift_sigma[i][j]);
        }
    }
    canvas_noise.cd(4);
    canvas_noise.get_canvas()->GetPad(4)->SetGrid();
    intrinsic_noise_map->DrawCopy("COLZ");
    for (int i = 0; i < 4; i++) {
        line_h[i]->Draw();
        line_v[i]->Draw();
    }

    // save data
    if (t_file_out != NULL) {
        t_file_out->cd();
        t_file_out->mkdir("hist_pedestal_raw");
        for (int i = 0; i < 25; i++) {
            t_file_out->cd("hist_pedestal_raw");
            gDirectory->mkdir(Form("CT_%02d", i + 1))->cd();
            for (int j = 0; j < 64; j++) {
                ped_hist[i][j]->Write();
            }
        }
        t_file_out->mkdir("hist_pedestal_shift");
        for (int i = 0; i < 25; i++) {
            t_file_out->cd("hist_pedestal_shift");
            gDirectory->mkdir(Form("CT_%02d", i + 1))->cd();
            for (int j = 0; j < 64; j++) {
                ped_shift_hist[i][j]->Write();
            }
        }
        t_file_out->mkdir("hist_common_noise")->cd();
        for (int i = 0; i < 25; i++) {
            common_noise_hist[i]->Write();
        }
        TMatrixF ped_mean_mat(25, 64);
        TMatrixF ped_sigma_mat(25, 64);
        TMatrixF ped_shift_mean_mat(25, 64);
        TMatrixF ped_shift_sigma_mat(25, 64);
        for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 64; j++) {
                ped_mean_mat(i, j) = ped_mean[i][j];
                ped_sigma_mat(i, j) = ped_sigma[i][j];
                ped_shift_mean_mat(i, j) = ped_shift_mean[i][j];
                ped_shift_sigma_mat(i, j) = ped_shift_sigma[i][j];
            }
        }
        t_file_out->cd();
        ped_mean_mat.Write("ped_mean_mat");
        ped_sigma_mat.Write("ped_sigma_mat");
        ped_shift_mean_mat.Write("ped_shift_mean_mat");
        ped_shift_sigma_mat.Write("ped_shift_sigma_mat");
        canvas_noise.get_canvas()->Write();
        t_file_out->Close();
        cout << "all data writed successfully." << endl;

    }

    rootapp->Run();

    return 0;
}
