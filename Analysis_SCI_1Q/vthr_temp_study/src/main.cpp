#include <iostream>
#include <cmath>
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "CooConv.hpp"

using namespace std;

double get_k(double x1, double y1, double x2, double y2) {
    return (y2 - y1) / (x2 - x1);
}

double get_b(double x1, double y1, double x2, double y2) {
    double k = (y2 - y1) / (x2 - x1);
    return y1 - k * x1;
}

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

    double vthr_max = 1024;
    double vthr_min = -128.0;

    // open pol_event file
    TFile* pol_event_file = new TFile(options_mgr.pol_event_filename.Data(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << options_mgr.pol_event_filename.Data() << endl;
        return 1;
    }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);
    t_pol_event.deactive_all(t_pol_event_tree);
    t_pol_event.active(t_pol_event_tree, "time_aligned");
    t_pol_event.active(t_pol_event_tree, "is_ped");
    t_pol_event.active(t_pol_event_tree, "type");
    t_pol_event.active(t_pol_event_tree, "multiplicity");
    t_pol_event.active(t_pol_event_tree, "trigger_bit");
    t_pol_event.active(t_pol_event_tree, "energy_value");
    t_pol_event.active(t_pol_event_tree, "channel_status");
    t_pol_event.active(t_pol_event_tree, "dy12_too_high");
    t_pol_event.active(t_pol_event_tree, "common_noise");
    t_pol_event.active(t_pol_event_tree, "fe_temp");

    // open output file
    TFile* output_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (output_file->IsZombie()) {
        cout << "output file open failed: " << options_mgr.output_filename.Data() << endl;
        return 1;
    }

    // ======== prepare histogram ========
    int temp_min_ct[25] = {26, 34, 29, 25, 26,
                           36, 30, 35, 32, 0,
                           29, 30, 29, 32, 29,
                           27, 30, 33, 28, 28,
                           25, 28, 26, 27, 25};

    int temp_max_ct[25] = {30, 37, 34, 30, 32,
                           39, 33, 38, 36, 0,
                           31, 33, 32, 35, 35,
                           29, 33, 36, 32, 33,
                           27, 30, 29, 31, 31};

    // correct zero temperature
    double temp_min_sum = 0;
    int    temp_min_n = 0;
    double temp_max_sum = 0;
    int    temp_max_n = 0;
    for (int i = 0; i < 25; i++) {
        if (temp_min_ct[i] > 0) {
            temp_min_sum += temp_min_ct[i];
            temp_min_n++;
        }
        if (temp_max_ct[i] > 0) {
            temp_max_sum += temp_max_ct[i];
            temp_max_n++;
        }
    }
    int temp_min_mean = temp_min_sum / temp_min_n + 0.5;
    int temp_max_mean = temp_max_sum / temp_max_n + 0.5;
    for (int i = 0; i < 25; i++) {
        if (temp_min_ct[i] < 1) {
            temp_min_ct[i] = temp_min_mean;
        }
        if (temp_max_ct[i] < 1) {
            temp_max_ct[i] = temp_max_mean;
        }
    }

    int num_of_temp[25];
    for (int i = 0; i < 25; i++) {
        num_of_temp[i] = temp_max_ct[i] - temp_min_ct[i] + 1;
    }

    TH1F* tri_spec[25][64][20];
    TH1F* all_spec[25][64][20];
    TH1F* tri_eff[25][64][20];
    TF1*  fun_spec[25][64][20];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < num_of_temp[i]; k++) {
                tri_spec[i][j][k] = new TH1F(Form("tri_spec_%02d_%02d_%02d", i + 1, j, temp_min_ct[i] + k),
                        Form("tri_spec_%02d_%02d_%02d", i + 1, j, temp_min_ct[i] + k), 256, vthr_min, vthr_max);
                tri_spec[i][j][k]->SetDirectory(NULL);
                all_spec[i][j][k] = new TH1F(Form("all_spec_%02d_%02d_%02d", i + 1, j, temp_min_ct[i] + k),
                        Form("all_spec_%02d_%02d_%02d", i + 1, j, temp_min_ct[i] + k), 256, vthr_min, vthr_max);
                all_spec[i][j][k]->SetDirectory(NULL);
                tri_eff[i][j][k] = new TH1F(Form("tri_eff_%02d_%02d_%02d", i + 1, j, temp_min_ct[i] + k),
                        Form("tri_eff_%02d_%02d_%02d", i + 1, j, temp_min_ct[i] + k), 256, vthr_min, vthr_max);
                tri_eff[i][j][k]->SetDirectory(NULL);
                fun_spec[i][j][k] = new TF1(Form("fun_spec_%02d_%02d_%02d", i + 1, j, temp_min_ct[i] + k),
                        "(TMath::Erf((x - [0]) / TMath::Sqrt(2) / [1]) + 1.0) / 2.0", vthr_min, vthr_max);
            }
        }
    }

    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);

        // skip pedestal event
        if (t_pol_event.is_ped) continue;
        // skip cosmic events
        if (t_pol_event.type == 0xFF00) continue;

        // correct temp
        double fe_temp_sum(0);
        int fe_temp_n(0);
        for (int i = 0; i < 25; i++) {
            if (t_pol_event.fe_temp[i] > 5) {
                fe_temp_sum += t_pol_event.fe_temp[i];
                fe_temp_n++;
            }
        }
        int fe_temp_mean = fe_temp_sum / fe_temp_n + 0.5;
        for (int i = 0; i < 25; i++) {
            if (t_pol_event.fe_temp[i] <= 5) {
                t_pol_event.fe_temp[i] = fe_temp_mean;
            }
        }

        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) continue;
            int k = t_pol_event.fe_temp[i] - temp_min_ct[i];
            if (k < 0 || k >= num_of_temp[i]) continue;

            for (int j = 0; j < 64; j++) {
                if (t_pol_event.channel_status[i][j] > 0 && t_pol_event.channel_status[i][j] != 0x4) continue;
                if (t_pol_event.multiplicity[i] - t_pol_event.trigger_bit[i][j] < 2) continue;

                all_spec[i][j][k]->Fill(t_pol_event.energy_value[i][j] + t_pol_event.common_noise[i]);
                if (t_pol_event.trigger_bit[i][j]) {
                    tri_spec[i][j][k]->Fill(t_pol_event.energy_value[i][j] + t_pol_event.common_noise[i]);
                }
            }
        }

    }
    cout << " DONE ]" << endl;
    pol_event_file->Close();
    delete pol_event_file;
    pol_event_file = NULL;

    // vthr calc
    gROOT->SetBatch(kTRUE);
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);

    TGraphErrors* vthr_temp_gr[25][64];
    TF1* vthr_temp_fun[25][64];
    TVectorF vthr_temp_p0[25];
    TVectorF vthr_temp_p1[25];

    for (int i = 0; i < 25; i++) {
        vthr_temp_p0[i].ResizeTo(64);
        vthr_temp_p1[i].ResizeTo(64);
        for (int j = 0; j < 64; j++) {
            vthr_temp_gr[i][j] = new TGraphErrors();
            vthr_temp_gr[i][j]->SetName(Form("vthr_temp_gr_%02d_%02d", i + 1, j));
            vthr_temp_gr[i][j]->SetTitle(Form("vthr_temp_gr_%02d_%02d", i + 1, j));
            vthr_temp_fun[i][j] = new TF1(Form("vthr_temp_fun_%02d_%02d", i + 1, j), "[0] + [1] * x", vthr_min, vthr_max);
            vthr_temp_fun[i][j]->SetParameters(100.0, 1.0);

            for (int k = 0; k < num_of_temp[i]; k++) {
                if (tri_spec[i][j][k]->GetEntries() < 50) {
                    cout << "WARNING: entries of " << Form("CT_%02d_%02d_%02d", i + 1, j, temp_min_ct[i] + k) << " is less than 50." << endl;
                    continue;
                }
                // generate tri_eff
                for (int m = 1; m <= tri_eff[i][j][k]->GetNbinsX(); m++) {
                    double tri_val = tri_spec[i][j][k]->GetBinContent(m);
                    double all_val = all_spec[i][j][k]->GetBinContent(m);
                    double bin_content = tri_val / all_val;
                    double err_pow2 = 1.0 / tri_val + 1.0 / all_val - 2.0 / TMath::Sqrt(tri_val * all_val);
                    double bin_error = bin_content * TMath::Sqrt(err_pow2);
                    if (tri_eff[i][j][k]->GetBinCenter(m) > 0 && bin_content > 0) {
                        tri_eff[i][j][k]->SetBinContent(m, bin_content);
                        tri_eff[i][j][k]->SetBinError(m, bin_error);
                    }
                }

                for (int m = 1; m < tri_eff[i][j][k]->GetNbinsX() * 0.6; m++) {
                    if (tri_eff[i][j][k]->GetBinContent(m) > 0.5) {
                        fun_spec[i][j][k]->SetParameter(0, tri_eff[i][j][k]->GetBinCenter(m));
                        fun_spec[i][j][k]->SetParameter(1, 20);
                        break;
                    }
                }
                tri_eff[i][j][k]->Fit(fun_spec[i][j][k], "RQ");
                double cur_vthr = fun_spec[i][j][k]->GetParameter(0);
                double cur_vthr_err = fun_spec[i][j][k]->GetParError(0);
                vthr_temp_gr[i][j]->SetPoint(k, temp_min_ct[i] + k, cur_vthr);
                vthr_temp_gr[i][j]->SetPointError(k, 0.5, cur_vthr_err);
            }
            double min_temp = temp_min_ct[i] + 0.5;
            double max_temp = temp_max_ct[i] - 0.5;
            double min_vthr = vthr_temp_gr[i][j]->Eval(min_temp);
            double max_vthr = vthr_temp_gr[i][j]->Eval(max_temp);
            vthr_temp_fun[i][j]->SetParameters(get_b(min_temp, min_vthr, max_temp, max_vthr), get_k(min_temp, min_vthr, max_temp, max_vthr));
            vthr_temp_gr[i][j]->Fit(vthr_temp_fun[i][j], "RQ");
            vthr_temp_p0[i](j) = vthr_temp_fun[i][j]->GetParameter(0);
            vthr_temp_p1[i](j) = vthr_temp_fun[i][j]->GetParameter(1);
        }
    }

    TCanvas* canvas_vthr_temp[25];
    for (int i = 0; i < 25; i++) {
        canvas_vthr_temp[i] = new TCanvas(Form("canvas_vthr_temp_%02d", i + 1), Form("canvas_vthr_temp_%02d", i + 1), 1200, 900);
        canvas_vthr_temp[i]->Divide(8, 8);
        canvas_vthr_temp[i]->SetFillColor(kYellow);
        for (int j = 0; j < 64; j++) {
            canvas_vthr_temp[i]->cd(jtoc(j));
            canvas_vthr_temp[i]->GetPad(jtoc(j))->SetFillColor(kWhite);
            vthr_temp_gr[i][j]->Draw("AP");
        }
        output_file->cd();
        output_file->mkdir(Form("dir_vthr_temp_%02d", i + 1))->cd();;
        for (int j = 0; j < 64; j++) {
            vthr_temp_gr[i][j]->Write();
            for (int k = 0; k < num_of_temp[i]; k++) {
                tri_eff[i][j][k]->Write();
            }
        }
        output_file->cd();
        canvas_vthr_temp[i]->Write();
        vthr_temp_p0[i].Write(Form("vthr_temp_p0_%02d", i + 1));
        vthr_temp_p1[i].Write(Form("vthr_temp_p1_%02d", i + 1));
    }

    output_file->Close();
    delete output_file;
    output_file = NULL;

    return 0;
}
