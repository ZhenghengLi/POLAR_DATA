#include <iostream>
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "CooConv.hpp"

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

    double vthr_mean_0 = 150.0;
    double vthr_sigma_0 = 15.0;
    double vthr_max = 1024;
    double vthr_min = -128.0;

    if (options_mgr.energy_flag) {
        vthr_mean_0 = 10.0;
        vthr_sigma_0 = 5.0;
        vthr_max = 60.0;
        vthr_min = -15.0;
    }

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

    // open output file
    TFile* output_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (output_file->IsZombie()) {
        cout << "output file open failed: " << options_mgr.output_filename.Data() << endl;
        return 1;
    }

    TH1F* tri_spec[25][64];
    TH1F* all_spec[25][64];
    TF1*  fun_spec[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            tri_spec[i][j] = new TH1F(Form("tri_spec_%02d_%02d", i + 1, j), Form("tri_spec_%02d_%02d", i + 1, j), 256, vthr_min, vthr_max);
            // tri_spec[i][j]->SetDirectory(NULL);
            all_spec[i][j] = new TH1F(Form("all_spec_%02d_%02d", i + 1, j), Form("all_spec_%02d_%02d", i + 1, j), 256, vthr_min, vthr_max);
            all_spec[i][j]->SetDirectory(NULL);
            fun_spec[i][j] = new TF1(Form("fun_spec_%02d_%02d", i + 1, j), "(TMath::Erf((x - [0]) / TMath::Sqrt(2) / [1]) + 1.0) / 2.0", vthr_min, vthr_max);
            fun_spec[i][j]->SetParameters(vthr_mean_0, vthr_sigma_0);
        }
    }
    TH2F* max_ADC_spec[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j <64; j++) {
            max_ADC_spec[i][j] = new TH2F(Form("max_ADC_spec_%02d_%02d", i + 1, j), Form("max_ADC_spec_%02d_%02d", i + 1, j),
                    192, 0, 768, 1024, 0, 4096);
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

        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) continue;
            // calculate max ADC
            double cur_max_ADC = 0;
            int cur_max_j = 0;
            for (int j = 0; j < 64; j++) {
                if (cur_max_ADC < t_pol_event.energy_value[i][j]) {
                    cur_max_ADC = t_pol_event.energy_value[i][j];
                    cur_max_j = j;
                }
            }
            if (!t_pol_event.trigger_bit[i][cur_max_j]) continue;
            if (cur_max_ADC < 1000) continue;
            for (int j = 0; j < 64; j++) {
                if (t_pol_event.channel_status[i][j] > 0 && t_pol_event.channel_status[i][j] != 0x4) continue;
                if (t_pol_event.multiplicity[i] - t_pol_event.trigger_bit[i][j] < 2) continue;
                all_spec[i][j]->Fill(t_pol_event.energy_value[i][j] + t_pol_event.common_noise[i]);
                if (t_pol_event.trigger_bit[i][j]) {
                    tri_spec[i][j]->Fill(t_pol_event.energy_value[i][j] + t_pol_event.common_noise[i]);
                    max_ADC_spec[i][j]->Fill(t_pol_event.energy_value[i][j], cur_max_ADC);
                }
            }
        }

    }
    cout << " DONE ]" << endl;
    pol_event_file->Close();
    delete pol_event_file;
    pol_event_file = NULL;

    TH1F* tri_eff[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            tri_eff[i][j] = static_cast<TH1F*>(tri_spec[i][j]->Clone(Form("tri_eff_%02d_%02d", i + 1, j)));
            tri_eff[i][j]->SetTitle(Form("tri_eff_%02d_%02d", i + 1, j));
            for (int k = 1; k < tri_eff[i][j]->GetNbinsX(); k++) {
                if (tri_eff[i][j]->GetBinCenter(k) < 0) {
                    tri_eff[i][j]->SetBinContent(k, 0);
                } else {
                    break;
                }
            }
        }
    }

    // write spec
    gROOT->SetBatch(kTRUE);
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);
    TCanvas* canvas_spec[25];
    TVectorF vthr_adc_value[25];
    TVectorF vthr_adc_sigma[25];
    TVectorF vthr_adc_value_err[25];
    TVectorF vthr_adc_sigma_err[25];
    TCanvas* canvas_vthr_pos[25];
    TLine* vthr_line_center[25][64];
    TLine* vthr_line_left[25][64];
    TLine* vthr_line_right[25][64];
    TCanvas* canvas_max_ADC_spec[25];
    for (int i = 0; i < 25; i++) {
        canvas_spec[i] = new TCanvas(Form("canvas_spec_CT_%02d", i + 1), Form("canvas_spec_CT_%02d", i + 1), 2000, 1600);
        canvas_spec[i]->Divide(8, 8);
        canvas_spec[i]->SetFillColor(kYellow);
        vthr_adc_value[i].ResizeTo(64);
        vthr_adc_sigma[i].ResizeTo(64);
        vthr_adc_value_err[i].ResizeTo(64);
        vthr_adc_sigma_err[i].ResizeTo(64);
        output_file->mkdir(Form("trigger_ratio_CT_%02d", i + 1))->cd();
        for (int j = 0; j < 64; j++) {
            canvas_spec[i]->cd(jtoc(j));
            canvas_spec[i]->GetPad(jtoc(j))->SetFillColor(kWhite);
            tri_eff[i][j]->Divide(all_spec[i][j]);
            for (int k = 1; k < tri_eff[i][j]->GetNbinsX(); k++) {
                if (tri_eff[i][j]->GetBinContent(k) > 0.5) {
                    fun_spec[i][j]->SetParameter(0, tri_eff[i][j]->GetBinCenter(k));
                    break;
                }
            }
            tri_eff[i][j]->Fit(fun_spec[i][j], "RQ");
            vthr_adc_value[i](j) = fun_spec[i][j]->GetParameter(0);
            vthr_adc_value_err[i](j) = fun_spec[i][j]->GetParError(0);
            vthr_adc_sigma[i](j) = fun_spec[i][j]->GetParameter(1);
            vthr_adc_sigma_err[i](j) = fun_spec[i][j]->GetParError(1);
            tri_eff[i][j]->Write();
            tri_spec[i][j]->Write();
            all_spec[i][j]->Write();
            max_ADC_spec[i][j]->Write();
        }
        canvas_vthr_pos[i] = new TCanvas(Form("canvas_vthr_pos_CT_%02d", i + 1), Form("canvas_vthr_pos_CT_%02d", i + 1), 2000, 1600);
        canvas_vthr_pos[i]->Divide(8, 8);
        canvas_vthr_pos[i]->SetFillColor(kYellow);
        for (int j = 0; j < 64; j++) {
            canvas_vthr_pos[i]->cd(jtoc(j));
            canvas_vthr_pos[i]->GetPad(jtoc(j))->SetFillColor(kWhite);
            tri_spec[i][j]->Draw();
            double y_max = tri_spec[i][j]->GetMaximum();
            vthr_line_center[i][j] = new TLine(vthr_adc_value[i](j), 0, vthr_adc_value[i](j), y_max);
            vthr_line_left[i][j] = new TLine(vthr_adc_value[i](j) - vthr_adc_sigma[i](j), 0, vthr_adc_value[i](j) - vthr_adc_sigma[i](j), y_max);
            vthr_line_right[i][j] = new TLine(vthr_adc_value[i](j) + vthr_adc_sigma[i](j), 0, vthr_adc_value[i](j) + vthr_adc_sigma[i](j), y_max);
            vthr_line_center[i][j]->SetLineColor(kRed);
            vthr_line_left[i][j]->SetLineColor(kGreen);
            vthr_line_right[i][j]->SetLineColor(kGreen);
            vthr_line_center[i][j]->Draw("same");
            vthr_line_left[i][j]->Draw("same");
            vthr_line_right[i][j]->Draw("same");
        }
        canvas_max_ADC_spec[i] = new TCanvas(Form("canvas_max_ADC_spec_CT_%02d", i + 1), Form("canvas_max_ADC_spec_CT_%02d", i + 1), 2000, 1600);
        canvas_max_ADC_spec[i]->Divide(8, 8);
        canvas_max_ADC_spec[i]->SetFillColor(kYellow);
        for (int j = 0; j < 64; j++) {
            canvas_max_ADC_spec[i]->cd(jtoc(j));
            canvas_max_ADC_spec[i]->GetPad(jtoc(j))->SetFillColor(kWhite);
            max_ADC_spec[i][j]->Draw("colz");
        }
        output_file->cd();
        canvas_spec[i]->Write();
        canvas_vthr_pos[i]->Write();
        canvas_max_ADC_spec[i]->Write();
        vthr_adc_value[i].Write(Form("vthr_adc_value_CT_%02d", i + 1));
        vthr_adc_value_err[i].Write(Form("vthr_adc_value_err_CT_%02d", i + 1));
        vthr_adc_sigma[i].Write(Form("vthr_adc_sigma_CT_%02d", i + 1));
        vthr_adc_sigma_err[i].Write(Form("vthr_adc_sigma_err_CT_%02d", i + 1));
    }
    output_file->Close();
    delete output_file;
    output_file = NULL;

    return 0;
}
