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

    const int bin_size_x = 64;
    const int max_adc_x = 4096;
    const int bin_size_y = 4;
    const int max_adc_y = 4096;

    int nbins_x = max_adc_x / bin_size_x;
    int nbins_y = max_adc_y / bin_size_y;

    // open output file
    TFile* output_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (output_file->IsZombie()) {
        cout << "output_file open failed: " << options_mgr.output_filename << endl;
        return 1;
    }
    TH2F* max_ADC_spec_tri[25][64];
    TH2F* max_ADC_spec_all[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j <64; j++) {
            max_ADC_spec_tri[i][j] = new TH2F(Form("max_ADC_spec_tri_%02d_%02d", i + 1, j), Form("max_ADC_spec_tri_%02d_%02d", i + 1, j),
                    nbins_x, 0, max_adc_x, nbins_y, 0, max_adc_y);
            max_ADC_spec_all[i][j] = new TH2F(Form("max_ADC_spec_all_%02d_%02d", i + 1, j), Form("max_ADC_spec_all_%02d_%02d", i + 1, j),
                    nbins_x, 0, max_adc_x, nbins_y, 0, max_adc_y);
        }
    }

    for (size_t i = 0; i < options_mgr.pol_event_filename_list.size(); i++) {
        TFile* pol_event_file = new TFile(options_mgr.pol_event_filename_list[i].Data(), "read");
        if (pol_event_file->IsZombie()) {
            cout << "pol_event_file open failed: " << options_mgr.pol_event_filename_list[i] << endl;
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
        t_pol_event.active(t_pol_event_tree, "common_noise");

        int pre_percent = 0;
        int cur_percent = 0;
        cout << "reading file: " << options_mgr.pol_event_filename_list[i] << " ..." << endl;
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
                for (int j = 0; j < 64; j++) {
                    if (t_pol_event.channel_status[i][j] > 0) continue;
                    if (j == cur_max_j) continue;
                    max_ADC_spec_all[i][j]->Fill(cur_max_ADC, t_pol_event.energy_value[i][j]);
                    if (t_pol_event.trigger_bit[i][j]) {
                        max_ADC_spec_tri[i][j]->Fill(cur_max_ADC, t_pol_event.energy_value[i][j]);
                    }
                }
            }
        }
        cout << " DONE ]" << endl;

        pol_event_file->Close();
        delete pol_event_file;
        pol_event_file = NULL;

    }

    cout << "calculating cutoff and saving result ..." << endl;

    gROOT->SetBatch(kTRUE);
    gErrorIgnoreLevel = kWarning;

    // calculate left cutoff
    TH1F* h_spec_tri = new TH1F("h_spec_tri", "h_spec_tri", max_adc_y / bin_size_y, 0, max_adc_y);
    h_spec_tri->SetDirectory(NULL);
    TH1F* h_spec_all = new TH1F("h_spec_all", "h_spec_all", max_adc_y / bin_size_y, 0, max_adc_y);
    h_spec_all->SetDirectory(NULL);
    int tri_stat = 0;
    TF1* f_ratio = new TF1("f_ratio", "(TMath::Erf((x - [0]) / TMath::Sqrt(2) / [1]) + 1.0) / 2.0", 0, max_adc_y);
    f_ratio->SetParLimits(0, 10, 3000);
    f_ratio->SetParLimits(1, 1, 100);
    TH1F* nonlin_curve[25][64];
    TF1*  nonlin_fun[25][64];
    TVectorF nonlin_fun_p0[25];
    TVectorF nonlin_fun_p1[25];
    TVectorF nonlin_fun_p2[25];
    for (int i = 0; i < 25; i++) {
        nonlin_fun_p0[i].ResizeTo(64);
        nonlin_fun_p1[i].ResizeTo(64);
        nonlin_fun_p2[i].ResizeTo(64);
        for (int j = 0; j < 64; j++) {
            nonlin_curve[i][j] = new TH1F(
                    Form("nonlin_curve_%02d_%02d", i + 1, j),
                    Form("nonlin_curve_%02d_%02d", i + 1, j),
                    nbins_x, 0, 4096);
            nonlin_curve[i][j]->SetLineColor(kBlue);
            nonlin_curve[i][j]->SetLineWidth(2);
            nonlin_curve[i][j]->SetMarkerColor(kRed);
            nonlin_curve[i][j]->SetMarkerStyle(5);
            nonlin_fun[i][j] = new TF1(
                    Form("nonlin_fun_%02d_%02d", i + 1, j),
                    "[0] * (1 + [1] * x) * (1 + TMath::Erf(x / [2])) / 2",
                    50, 4000);
            nonlin_fun[i][j]->SetParameters(300, 2.5E-5, 400);
            nonlin_fun[i][j]->SetParLimits(0, 10, 3000);
            nonlin_fun[i][j]->SetParLimits(1, 0, 0.1);
            nonlin_fun[i][j]->SetParLimits(2, 10, 2000);
            for (int k = 1; k <= nbins_x; k++) {
                tri_stat = 0;
                // read counts
                h_spec_tri->Reset("M");
                h_spec_all->Reset("M");
                for (int n = 1; n <= nbins_y; n++) {
                    int binc_tri = max_ADC_spec_tri[i][j]->GetBinContent(k, n);
                    int binc_all = max_ADC_spec_all[i][j]->GetBinContent(k, n);
                    tri_stat += binc_tri;
                    h_spec_tri->SetBinContent(n, binc_tri);
                    h_spec_all->SetBinContent(n, binc_all);
                }
                if (tri_stat < 30) continue;
                h_spec_tri->Divide(h_spec_all);
                // find first bin with ratio 1
                int first_bin_1 = 0;
                for (int n = 1; n <= nbins_y; n++) {
                    if (h_spec_tri->GetBinContent(n) > 0.90) {
                        first_bin_1 = n;
                        break;
                    }
                }
                // find last bin with ratio 1
                int last_bin_1 = 0;
                for (int n = nbins_y; n >= 1; n--) {
                    if (h_spec_tri->GetBinContent(n) > 0.90) {
                        last_bin_1 = n;
                        break;
                    }
                }
                if (last_bin_1 - first_bin_1 < 5) continue;
                double adc_left = (first_bin_1 - 0.5) * bin_size_y;
                double adc_right = (last_bin_1 - 0.5) * bin_size_y;
                f_ratio->SetRange(adc_left - 40, adc_right);
                f_ratio->SetParameters(adc_left, 15);
                h_spec_tri->Fit(f_ratio, "RQ");
                double cutoff_pos = f_ratio->GetParameter(0);
                double cutoff_err = f_ratio->GetParError(0);
                if (cutoff_err < 1) continue;
                if (cutoff_err > 100) continue;
                nonlin_curve[i][j]->SetBinContent(k, cutoff_pos);
                nonlin_curve[i][j]->SetBinError(k, cutoff_err);
            }
            nonlin_curve[i][j]->Fit(nonlin_fun[i][j], "RQ");
            nonlin_curve[i][j]->Fit(nonlin_fun[i][j], "RQ");
            nonlin_curve[i][j]->Fit(nonlin_fun[i][j], "RQ");
            nonlin_fun_p0[i](j) = nonlin_fun[i][j]->GetParameter(0);
            nonlin_fun_p1[i](j) = nonlin_fun[i][j]->GetParameter(1);
            nonlin_fun_p2[i](j) = nonlin_fun[i][j]->GetParameter(2);
        }
    }


    // save result
    TCanvas* canvas_non_lin[25];
    TCanvas* canvas_non_lin_fitting[25];
    output_file->cd();
    for (int i = 0; i < 25; i++) {
        output_file->cd();
        canvas_non_lin[i] = new TCanvas(Form("canvas_non_lin_%02d", i + 1), Form("canvas_non_lin_%02d", i + 1), 1800, 1500);
        canvas_non_lin[i]->Divide(8, 8);
        canvas_non_lin[i]->SetFillColor(kYellow);
        for (int j = 0; j < 64; j++) {
            canvas_non_lin[i]->cd(jtoc(j));
            canvas_non_lin[i]->GetPad(jtoc(j))->SetFillColor(kWhite);
            // canvas_non_lin[i]->GetPad(jtoc(j))->SetLogz();
            max_ADC_spec_tri[i][j]->Draw("colz");
            nonlin_curve[i][j]->Draw("same HE");
        }
        canvas_non_lin_fitting[i] = new TCanvas(Form("canvas_non_lin_fitting_%02d", i + 1), Form("canvas_non_lin_fitting_%02d", i + 1), 1800, 1500);
        canvas_non_lin_fitting[i]->Divide(8, 8);
        canvas_non_lin_fitting[i]->SetFillColor(kYellow);
        for (int j = 0; j < 64; j++) {
            canvas_non_lin_fitting[i]->cd(jtoc(j));
            canvas_non_lin_fitting[i]->GetPad(jtoc(j))->SetFillColor(kWhite);
            nonlin_curve[i][j]->Draw("HE");
        }
        canvas_non_lin[i]->Write();
        canvas_non_lin_fitting[i]->Write();
        nonlin_fun_p0[i].Write(Form("nonlin_fun_p0_%02d", i + 1));
        nonlin_fun_p1[i].Write(Form("nonlin_fun_p1_%02d", i + 1));
        nonlin_fun_p2[i].Write(Form("nonlin_fun_p2_%02d", i + 1));
        output_file->mkdir(Form("non_linearity_%02d", i + 1))->cd();
        for (int j = 0; j < 64; j++) {
            max_ADC_spec_tri[i][j]->Write();
            nonlin_curve[i][j]->Write();
        }
    }
    output_file->Close();
    delete output_file;
    output_file = NULL;

    return 0;
}
