#include <iostream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
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
        return 2;
    }

    // open ped_data_file
    TFile* ped_data_file = new TFile(options_mgr.ped_data_filename.Data(), "read");
    if (ped_data_file->IsZombie()) {
        cout << "ped_data_file open failed." << endl;
        return 1;
    }
    TTree* t_ped_data_tree = static_cast<TTree*>(ped_data_file->Get(Form("t_ped_data_ct_%02d", options_mgr.ct_num)));
    if (t_ped_data_tree == NULL) {
        cout << "cannot find TTree " << Form("t_ped_data_ct_%02d", options_mgr.ct_num) << endl;
        return 1;
    }
    struct {
        Double_t event_time;
        Bool_t   trigger_bit[64];
        Float_t  ped_adc[64];
        Float_t  fe_temp;
        Float_t  aux_interval;
    } t_ped_data;
    t_ped_data_tree->SetBranchAddress("event_time",     &t_ped_data.event_time     );
    t_ped_data_tree->SetBranchAddress("trigger_bit",     t_ped_data.trigger_bit    );
    t_ped_data_tree->SetBranchAddress("ped_adc",         t_ped_data.ped_adc        );
    t_ped_data_tree->SetBranchAddress("fe_temp",        &t_ped_data.fe_temp        );
    t_ped_data_tree->SetBranchAddress("aux_interval",   &t_ped_data.aux_interval   );
    cout << options_mgr.ped_data_filename.Data() << " { " << "ct_num = " << options_mgr.ct_num << " }" << endl;

    // open output file
    TFile* ped_result_file = new TFile(options_mgr.ped_result_filename.Data(), "recreate");
    if (ped_result_file->IsZombie()) {
        cout << "ped_result_file open failed." << endl;
        return 1;
    }

    // prepare histogram
    TH1D* ped_hist[64];
    for (int j = 0; j < 64; j++) {
        ped_hist[j] = new TH1D(Form("ped_hist_%02d_%02d", options_mgr.ct_num, j),
                Form("Pedestal of CH %02d_%02d", options_mgr.ct_num, j),
                PED_BINS, 0, PED_MAX);
        ped_hist[j]->SetDirectory(ped_result_file);
    }
    TH1D* common_noise_hist;
    common_noise_hist = new TH1D(Form("common_noise_%02d", options_mgr.ct_num),
            Form("Common noise of Module CT_%02d", options_mgr.ct_num), 256, -128, 128);
    common_noise_hist->SetDirectory(ped_result_file);
    TH1D* ped_shift_hist[64];
    for (int j = 0; j < 64; j++) {
        ped_shift_hist[j] = new TH1D(Form("ped_shift_hist_%02d_%02d", options_mgr.ct_num, j),
                Form("Pedestal shift of CH %02d_%02d", options_mgr.ct_num, j),
                256, -128, 128);
        ped_shift_hist[j]->SetDirectory(ped_result_file);
    }
    TVectorF ped_mean_vec(64);
    TVectorF ped_mean_err(64);
    TVectorF ped_sigma_vec(64);
    TVectorF ped_sigma_err(64);
    TVectorF ped_shift_mean_vec(64);
    TVectorF ped_shift_mean_err(64);
    TVectorF ped_shift_sigma_vec(64);
    TVectorF ped_shift_sigma_err(64);

    // reading and selecting data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading and selecting data ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_ped_data_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_ped_data_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_ped_data_tree->GetEntry(q);

        // filter start
        if (t_ped_data.aux_interval > 20) continue;
        if (t_ped_data.fe_temp < options_mgr.low_temp) continue;
        if (t_ped_data.fe_temp > options_mgr.high_temp) continue;
        // filter stop

        for (int j = 0; j < 64; j++) {
            ped_hist[j]->Fill(t_ped_data.ped_adc[j]);
        }
    }
    cout << " DONE ]" << endl;

    gROOT->SetBatch(kTRUE);
    gErrorIgnoreLevel = kWarning;
    gStyle->SetOptStat(0);

    cout << "Fitting pedestal ..." << endl;
    double ped_mean_0[64], ped_mean[64], ped_mean_e[64];
    double ped_sigma_0[64], ped_sigma[64], ped_sigma_e[64];
    TF1* ped_gaus[64];
    for (int j = 0; j < 64; j++) {
        ped_gaus[j] = new TF1(Form("ped_gaus_%02d_%02d", options_mgr.ct_num, j + 1), "gaus(0)", 0, PED_MAX);
        ped_gaus[j]->SetParameters(1, 400, 25);
        ped_gaus[j]->SetParName(0, "Amplitude");
        ped_gaus[j]->SetParName(1, "Mean");
        ped_gaus[j]->SetParName(2, "Sigma");
    }
    for (int j = 0; j < 64; j++) {
        ped_mean_0[j]  = ped_hist[j]->GetMean();
        ped_sigma_0[j] = ped_hist[j]->GetRMS();
        if (ped_hist[j]->GetEntries() < 20) {
            cout << "- WARNING: entries of CH " << options_mgr.ct_num << "_" << j + 1
                 << "is too small, use the arithmetic mean and sigma instead." << endl;
            ped_mean[j] = ped_mean_0[j];
            ped_sigma[j] = ped_sigma_0[j];
            continue;
        }
        Float_t min_adc = ped_mean_0[j] - 200 > 0 ? ped_mean_0[j] - 200 : 0;
        Float_t max_adc = ped_mean_0[j] + 200 < PED_MAX ? ped_mean_0[j] + 200 : PED_MAX;
        ped_gaus[j]->SetParameter(1, ped_mean_0[j]);
        ped_gaus[j]->SetParameter(2, ped_sigma_0[j] / 2);
        ped_gaus[j]->SetRange(min_adc, max_adc);
        ped_hist[j]->Fit(ped_gaus[j], "RQ");
        ped_hist[j]->Fit(ped_gaus[j], "RQ");
        ped_mean[j]  = ped_gaus[j]->GetParameter(1);
        ped_mean_e[j] = ped_gaus[j]->GetParError(1);
        ped_sigma[j] = abs(ped_gaus[j]->GetParameter(2));
        ped_sigma_e[j] = abs(ped_gaus[j]->GetParError(2));
        if (ped_mean[j] < 0 || ped_mean[j] > PED_MAX) {
            cout << "- WARNING: ped_mean of CH " << options_mgr.ct_num << "_" << j + 1
                 << " is out of range, use the arithmetic mean instead." << endl;
            ped_mean[j]  = ped_mean_0[j];
            ped_mean_e[j] = 0;
            ped_sigma[j] = ped_sigma_0[j];
            ped_sigma_e[j] = 0;
        } else if (ped_sigma[j] / ped_sigma_0[j] > 2.0) {
            cout << "- WARNING: ped_sigma of CH " << options_mgr.ct_num << "_" << j + 1
                 << " is too large, use the arithmetic mean instead." << endl;
            ped_mean[j]  = ped_mean_0[j];
            ped_mean_e[j] = 0;
            ped_sigma[j] = ped_sigma_0[j];
            ped_sigma_e[j] = 0;
        }
    }

    // subtract common noise and refit
    double  common_noise_sum;
    int     common_noise_n;
    double  cur_common_noise;
    pre_percent = 0;
    cur_percent = 0;
    cout << "Filling pedestal data of all modules and subtract common noise ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_ped_data_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_ped_data_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_ped_data_tree->GetEntry(q);

        // filter start
        if (t_ped_data.aux_interval > 20) continue;
        if (t_ped_data.fe_temp < options_mgr.low_temp) continue;
        if (t_ped_data.fe_temp > options_mgr.high_temp) continue;
        // filter stop

        common_noise_sum = 0;
        common_noise_n   = 0;
        for (int j = 0; j < 64; j++) {
            t_ped_data.ped_adc[j] -= ped_mean[j];
            if (!t_ped_data.trigger_bit[j]) {
                common_noise_sum += t_ped_data.ped_adc[j];
                common_noise_n++;
            }
        }
        cur_common_noise = (common_noise_n > 0 ? common_noise_sum / common_noise_n : 0);
        if (common_noise_n > 0)
            common_noise_hist->Fill(cur_common_noise);
        for (int j = 0; j < 64; j++) {
            ped_shift_hist[j]->Fill(t_ped_data.ped_adc[j] - cur_common_noise);
        }
    }
    cout << " DONE ]" << endl;
    // refit
    cout << "Refitting after common noise subtracted ..." << endl;
    double ped_shift_mean_0[64], ped_shift_mean[64], ped_shift_mean_e[64];
    double ped_shift_sigma_0[64], ped_shift_sigma[64], ped_shift_sigma_e[64];
    TF1* ped_shift_gaus[64];
    for (int j = 0; j < 64; j++) {
        ped_shift_gaus[j] = new TF1(Form("ped_shift_gaus_%02d_%02d", options_mgr.ct_num, j + 1), "gaus(0)", -64, 64);
        ped_shift_gaus[j]->SetParameters(1, 0, 25);
        ped_shift_gaus[j]->SetParName(0, "Amplitude");
        ped_shift_gaus[j]->SetParName(1, "Mean");
        ped_shift_gaus[j]->SetParName(2, "Sigma");
    }
    for (int j = 0; j < 64; j++) {
        ped_shift_mean_0[j]  = ped_shift_hist[j]->GetMean();
        ped_shift_sigma_0[j] = ped_shift_hist[j]->GetRMS();
        if (ped_shift_hist[j]->GetEntries() < 20) {
            cout << "- WARNING: entries of CH " << options_mgr.ct_num << "_" << j + 1
                 << "is too small, use the arithmetic mean and sigma instead." << endl;
            ped_shift_mean[j] = ped_shift_mean_0[j];
            ped_shift_sigma[j] = ped_shift_sigma_0[j];
            continue;
        }
        ped_shift_gaus[j]->SetParameter(1, ped_shift_mean_0[j]);
        ped_shift_gaus[j]->SetParameter(2, ped_shift_sigma_0[j] / 2);
        ped_shift_gaus[j]->SetRange(ped_shift_mean_0[j] - 5 * ped_shift_sigma_0[j],
                ped_shift_mean_0[j] + 5 * ped_shift_sigma_0[j]);
        ped_shift_hist[j]->Fit(ped_shift_gaus[j], "RQ");
        ped_shift_hist[j]->Fit(ped_shift_gaus[j], "RQ");
        ped_shift_mean[j]  = ped_shift_gaus[j]->GetParameter(1);
        ped_shift_mean_e[j] = ped_shift_gaus[j]->GetParError(1);
        ped_shift_sigma[j] = abs(ped_shift_gaus[j]->GetParameter(2));
        ped_shift_sigma_e[j] = abs(ped_shift_gaus[j]->GetParError(2));
        if (ped_shift_mean[j] < -128 || ped_shift_mean[j] > 128) {
            cout << "- WARNING: ped_shift_mean of CH " << options_mgr.ct_num << "_" << j + 1
                 << " is out of range, use the arithmetic mean instead." << endl;
            ped_shift_mean[j]  = ped_shift_mean_0[j];
            ped_shift_sigma[j] = ped_shift_sigma_0[j];
        } else if (ped_shift_sigma[j] / ped_shift_sigma_0[j] > 2.0) {
            cout << "- WARNING: ped_shift_sigma of CH " << options_mgr.ct_num << "_" << j + 1
                 << " is too large, use the arithmetic mean instead." << endl;
            ped_shift_mean[j]  = ped_shift_mean_0[j];
            ped_shift_sigma[j] = ped_shift_sigma_0[j];
        }
    }
    // fit common noise
    cout << "Fitting common noise ..." << endl;
    double common_noise_mean_0, common_noise_mean, common_noise_mean_e;
    double common_noise_sigma_0, common_noise_sigma, common_noise_sigma_e;
    TF1* common_noise_gaus;
    common_noise_gaus = new TF1(Form("common_noise_gaus_%02d", options_mgr.ct_num), "gaus(0)", -64, 64);
    common_noise_gaus->SetParameters(1, 0, 40);
    common_noise_gaus->SetParName(0, "Amplitude");
    common_noise_gaus->SetParName(1, "Mean");
    common_noise_gaus->SetParName(2, "Sigma");
    common_noise_mean_0  = common_noise_hist->GetMean();
    common_noise_sigma_0 = common_noise_hist->GetRMS();
    if (common_noise_hist->GetEntries() < 20) {
        cout << "- WARNING: entries of CT " << options_mgr.ct_num
             << "is too small, use the arithmetic mean and sigma instead." << endl;
        common_noise_mean = common_noise_mean_0;
        common_noise_mean_e = 0;
        common_noise_sigma = common_noise_sigma_0;
        common_noise_sigma_e = 0;
    } else {
        common_noise_gaus->SetParameter(1, common_noise_mean_0);
        common_noise_gaus->SetParameter(2, common_noise_sigma_0 / 2);
        common_noise_gaus->SetRange(common_noise_mean_0 - 5 * common_noise_sigma_0,
                common_noise_mean_0 + 5 * common_noise_sigma_0);
        common_noise_hist->Fit(common_noise_gaus, "RQ");
        common_noise_hist->Fit(common_noise_gaus, "RQ");
        common_noise_mean  = common_noise_gaus->GetParameter(1);
        common_noise_mean_e = common_noise_gaus->GetParError(1);
        common_noise_sigma = abs(common_noise_gaus->GetParameter(2));
        common_noise_sigma_e = abs(common_noise_gaus->GetParError(2));
        if (common_noise_mean < -128 || common_noise_mean > 128) {
            cout << "- WARNING: common_noise_mean of CT " << options_mgr.ct_num
                 << " is out of range, use the arithmetic mean instead." << endl;
            common_noise_mean  = common_noise_mean_0;
            common_noise_mean_e = 0;
            common_noise_sigma = common_noise_sigma_0;
            common_noise_sigma_e = 0;
        } else if (common_noise_sigma / common_noise_sigma_0 > 2.0) {
            cout << "- WARNING: common_noise_sigma of CT " << options_mgr.ct_num
                 << " is too large, use the arithmetic mean instead." << endl;
            common_noise_mean  = common_noise_mean_0;
            common_noise_mean_e = 0;
            common_noise_sigma = common_noise_sigma_0;
            common_noise_sigma_e = 0;
        }
    }

    // draw and save result
    ped_result_file->cd();
    TNamed("ct_num", Form("%d", options_mgr.ct_num)).Write();
    TCanvas* canvas_ped_hist = new TCanvas(Form("ped_hist_CT_%02d", options_mgr.ct_num),
            Form("ped_hist_CT_%02d", options_mgr.ct_num), 900, 900);
    canvas_ped_hist->Divide(8, 8);
    for (int j = 0; j < 64; j++) {
        canvas_ped_hist->cd(jtoc(j));
        ped_hist[j]->Draw();
    }
    canvas_ped_hist->Write();
    TCanvas* canvas_ped_shift_hist = new TCanvas(Form("ped_shift_hist_CT_%02d", options_mgr.ct_num));
    canvas_ped_shift_hist->Divide(8, 8);
    for (int j = 0; j < 64; j++) {
        canvas_ped_shift_hist->cd(jtoc(j));
        ped_shift_hist[j]->Draw();
    }
    canvas_ped_shift_hist->Write();
    common_noise_hist->Write();
    for (int j = 0; j < 64; j++) {
        ped_mean_vec(j) = ped_mean[j];
        ped_mean_err(j) = ped_mean_e[j];
        ped_sigma_vec(j) = ped_sigma[j];
        ped_sigma_err(j) = ped_sigma_e[j];
        ped_shift_mean_vec(j) = ped_shift_mean[j];
        ped_shift_mean_err(j) = ped_shift_mean_e[j];
        ped_shift_sigma_vec(j) = ped_shift_sigma[j];
        ped_shift_sigma_err(j) = ped_shift_sigma_e[j];
    }
    ped_mean_vec.Write("ped_mean_vec");
    ped_mean_err.Write("ped_mean_err");
    ped_sigma_vec.Write("ped_sigma_vec");
    ped_sigma_err.Write("ped_sigma_err");
    ped_shift_mean_vec.Write("ped_shift_mean_vec");
    ped_shift_mean_err.Write("ped_shift_mean_err");
    ped_shift_sigma_vec.Write("ped_shift_sigma_vec");
    ped_shift_sigma_err.Write("ped_shift_sigma_err");
    TNamed("common_noise", Form("%f", common_noise_sigma)).Write();
    TNamed("common_noise_err", Form("%f", common_noise_sigma_e)).Write();
    TNamed("common_mean", Form("%f", common_noise_mean)).Write();
    TNamed("common_mean_err", Form("%f", common_noise_mean_e)).Write();
    TNamed("low_temp", Form("%f", options_mgr.low_temp)).Write();
    TNamed("high_temp", Form("%f", options_mgr.high_temp)).Write();

    return 0;
}
