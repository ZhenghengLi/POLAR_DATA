#include <iostream>
#include "RootInc.hpp"
#include "CooConv.hpp"

// for energy
// #define VTHR_MEAN 10.0
// #define VTHR_SIGMA 5.0
// #define VTHR_MAX 60.0
// #define VTHR_MIN -15.0

// for adc
#define VTHR_MEAN 100.0
#define VTHR_SIGMA 50.0
#define VTHR_MAX 768.0
#define VTHR_MIN -128.0

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << "<data_1Q_file.root> <tcanvas_output.root>" << endl;
        return 2;
    }
    string data_1Q_fn = argv[1];
    string tcanvas_fn = argv[2];

//    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);

    // open data_1M_file
    TFile* t_file_in = new TFile(data_1Q_fn.c_str(), "read");
    if (t_file_in->IsZombie()) {
        cout << "root file of data_1Q open failed." << endl;
        return 1;
    }
    TTree* t_event_tree = static_cast<TTree*>(t_file_in->Get("t_event"));
    if (t_event_tree == NULL) {
        cout << "read t_event failed." << endl;
        return 1;
    }

    struct {
        Int_t   type;
        Int_t   trigger_n;
        Int_t   multiplicity[25];
        Bool_t  time_aligned[25];
        Int_t   pkt_count;
        Int_t   lost_count;
        Bool_t  trigger_bit[25][64];
        Float_t energy_value[25][64];
    } t_event;
    t_event_tree->SetBranchAddress("type",             &t_event.type                 );
    t_event_tree->SetBranchAddress("trigger_n",        &t_event.trigger_n            );
    t_event_tree->SetBranchAddress("multiplicity",      t_event.multiplicity         );
    t_event_tree->SetBranchAddress("time_aligned",      t_event.time_aligned         );
    t_event_tree->SetBranchAddress("pkt_count",        &t_event.pkt_count            );
    t_event_tree->SetBranchAddress("lost_count",       &t_event.lost_count           );
    t_event_tree->SetBranchAddress("trigger_bit",       t_event.trigger_bit          );
    t_event_tree->SetBranchAddress("energy_value",      t_event.energy_value         );

    t_event_tree->SetBranchStatus("*", false);
    t_event_tree->SetBranchStatus("type", true);
    t_event_tree->SetBranchStatus("trigger_n", true);
    t_event_tree->SetBranchStatus("multiplicity", true);
    t_event_tree->SetBranchStatus("time_aligned", true);
    t_event_tree->SetBranchStatus("pkt_count", true);
    t_event_tree->SetBranchStatus("lost_count", true);
    t_event_tree->SetBranchStatus("trigger_bit", true);
    t_event_tree->SetBranchStatus("energy_value", true);

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
            tri_spec[i][j] = new TH1F(Form("tri_spec_%02d_%02d", i + 1, j + 1), Form("tri_spec_%02d_%02d", i + 1, j + 1), 128, VTHR_MIN, VTHR_MAX);
            // tri_spec[i][j]->SetDirectory(NULL);
            all_spec[i][j] = new TH1F(Form("all_spec_%02d_%02d", i + 1, j + 1), Form("all_spec_%02d_%02d", i + 1, j + 1), 128, VTHR_MIN, VTHR_MAX);
            all_spec[i][j]->SetDirectory(NULL);
            fun_spec[i][j] = new TF1(Form("fun_spec_%02d_%02d", i + 1, j + 1), "(TMath::Erf((x - [0]) / TMath::Sqrt(2) / [1]) + 1.0) / 2.0", VTHR_MIN, VTHR_MAX);
            fun_spec[i][j]->SetParameters(VTHR_MEAN, VTHR_SIGMA);
        }
    }

    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data ..." << endl;
    cout << "[ " << flush;
    for (Long64_t i = 0; i < t_event_tree->GetEntries(); i++) {
        cur_percent = static_cast<int>(i * 100 / t_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_event_tree->GetEntry(i);

        if (t_event.type != 0x00FF) continue;

        for (int i = 0; i < 25; i++) {
            if (!t_event.time_aligned[i]) continue;
            for (int j = 0; j < 64; j++) {
                if (t_event.multiplicity[i] - t_event.trigger_bit[i][j] < 2) continue;
                if (t_event.multiplicity[i] > 4) continue;
                all_spec[i][j]->Fill(t_event.energy_value[i][j]);
                if (t_event.trigger_bit[i][j]) {
                    tri_spec[i][j]->Fill(t_event.energy_value[i][j]);
                }
            }
        }

    }
    cout << " DONE ]" << endl;
    t_file_in->Close();
    delete t_file_in;
    t_file_in = NULL;

    TH1F* tri_eff[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            tri_eff[i][j] = static_cast<TH1F*>(tri_spec[i][j]->Clone(Form("tri_eff_%02d_%02d", i + 1, j + 1)));
            tri_eff[i][j]->SetTitle(Form("tri_eff_%02d_%02d", i + 1, j + 1));
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

