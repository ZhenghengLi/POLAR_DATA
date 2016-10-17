#include <iostream>
#include "RootInc.hpp"
#include "EventIterator.hpp"
#include "CooConv.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <beam_event.root> <histogram.pdf>" << endl;
        return 2;
    }
    
    string beam_event_fn = argv[1];
    string histogram_fn = argv[2];

    TFile* t_file_merged_in = new TFile(beam_event_fn.c_str(), "read");
    if (t_file_merged_in->IsZombie()) {
        cout << "root file open failed." << endl;
        return 1;
    }
    
    struct {
        Int_t    type;
        Bool_t   trig_accepted[25];
        Bool_t   time_aligned[25];
        Int_t    pkt_count;
        Int_t    lost_count;
        Bool_t   trigger_bit[1600];
        Int_t    trigger_n;
        Int_t    multiplicity[25];
        Float_t  energy_adc[1600];
        Int_t    compress[25];
        Float_t  common_noise[25];
        Bool_t   bar_beam[1600];
        Double_t time_second[25];
        Float_t  max_rate[1600];
    } t_beam_event;

    TTree* t_beam_event_tree = static_cast<TTree*>(t_file_merged_in->Get("t_beam_event"));
    if (t_beam_event_tree == NULL) {
        cout << "not found ttree." << endl;
        return 1;
    }

    t_beam_event_tree->SetBranchAddress("type",                &t_beam_event.type               );
    t_beam_event_tree->SetBranchAddress("trig_accepted",        t_beam_event.trig_accepted      );
    t_beam_event_tree->SetBranchAddress("time_aligned",         t_beam_event.time_aligned       );
    t_beam_event_tree->SetBranchAddress("pkt_count",           &t_beam_event.pkt_count          );
    t_beam_event_tree->SetBranchAddress("lost_count",          &t_beam_event.lost_count         );
    t_beam_event_tree->SetBranchAddress("trigger_bit",          t_beam_event.trigger_bit        );
    t_beam_event_tree->SetBranchAddress("trigger_n",           &t_beam_event.trigger_n          );
    t_beam_event_tree->SetBranchAddress("multiplicity",         t_beam_event.multiplicity       );
    t_beam_event_tree->SetBranchAddress("energy_adc",           t_beam_event.energy_adc         );
    t_beam_event_tree->SetBranchAddress("compress",             t_beam_event.compress           );
    t_beam_event_tree->SetBranchAddress("common_noise",         t_beam_event.common_noise       );
    t_beam_event_tree->SetBranchAddress("bar_beam",             t_beam_event.bar_beam           );
    t_beam_event_tree->SetBranchAddress("time_second",          t_beam_event.time_second        );
    t_beam_event_tree->SetBranchAddress("max_rate",             t_beam_event.max_rate           );

    // prepare histogram
    TH2F* beam_rate_map = new TH2F("beam_rate_map", "max beam rate of 1600 bars", 40, 0, 40, 40, 0, 40);
    beam_rate_map->SetDirectory(NULL);
    beam_rate_map->GetXaxis()->SetNdivisions(40);
    beam_rate_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            beam_rate_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            beam_rate_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    bool rate_set_flag[25][64];
    TF1*  func_hist[25][64];
    TH1F* spec_hist[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            rate_set_flag[i][j] = true;
            spec_hist[i][j] = new TH1F(Form("spec_%d_%d", i + 1, j + 1), Form("spec_%d_%d", i + 1, j + 1), 128, 0, 4096);
            spec_hist[i][j]->SetDirectory(NULL);
            func_hist[i][j] = new TF1(Form("func_hist_%d_%d", i + 1, j + 1), "TMath::Exp([0] + [1] * x) + gaus(2) + [5]", 500, 4096);
            func_hist[i][j]->SetParameters(8, -0.006, 50, 1500, 500, 5);
            func_hist[i][j]->SetParLimits(1, -0.1, -0.0001);
            func_hist[i][j]->SetParLimits(3, 500, 3500);
            func_hist[i][j]->SetParLimits(4, 100, 2000);
            func_hist[i][j]->SetParLimits(5, 0, 1000);
        }
    }

    // fill histogram
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_beam_event_tree->GetEntries(); q++) {
        t_beam_event_tree->GetEntry(q);
        cur_percent = static_cast<int>(100 * q / t_beam_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        for (int i = 0; i < 25; i++) {
            if (!t_beam_event.time_aligned[i]) {
                continue;
            }
            for (int j = 0; j < 64; j++) {
                int k = i * 64 + j;
                if (t_beam_event.energy_adc[k] > 4096)
                    continue;
                if (t_beam_event.bar_beam[k] && t_beam_event.trigger_bit[k] && t_beam_event.max_rate[k] > 10) {
                    if (rate_set_flag[i][j]) {
                        rate_set_flag[i][j] = false;
                        beam_rate_map->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, t_beam_event.max_rate[k]);
                    }
                    spec_hist[i][j]->Fill(t_beam_event.energy_adc[k]);
                }
            }
        }
    }
    t_file_merged_in->Close();
    cout << " DONE ]" << endl;

    // draw histogram
    cout << "drawing ... " << endl;
    gROOT->SetBatch(kTRUE);
    gErrorIgnoreLevel = kWarning;
    TCanvas* canvas_rate_map = new TCanvas("canvas_rate_map", "canvas_rate_map", 2000, 1600);
    canvas_rate_map->SetGrid();
    canvas_rate_map->cd();
    beam_rate_map->Draw("COLZ");
    for (int i = 0; i < 4; i++) {
        TLine* line_h = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
        line_h->SetLineColor(kWhite);
        line_h->Draw("same");
        TLine* line_v = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
        line_v->SetLineColor(kWhite);
        line_v->Draw("same");
    }
    TCanvas* canvas_spec[25];
    for (int i = 0; i < 25; i++) {
        canvas_spec[i] = new TCanvas(Form("canvas_spec_CT_%d", i + 1), Form("canvas_spec_CT_%d", i + 1), 2000, 1600);
        canvas_spec[i]->Divide(8, 8);
        for (int j = 0; j < 64; j++) {
            canvas_spec[i]->cd(jtoc(j));
//            canvas_spec[i]->GetPad(jtoc(j))->SetLogy();
//            spec_hist[i][j]->Draw();
            if (spec_hist[i][j]->GetEntries() < 10) 
                continue;
            spec_hist[i][j]->Fit(func_hist[i][j], "RQ");
        }
    }
    
    cout << "printing ... " << endl;
    cout << " - page 1" << endl;
    canvas_rate_map->Print((histogram_fn + "(").c_str(), "pdf");
    for (int i = 0; i < 25; i++) {
        cout << " - page " << i + 2 << endl;
        if (i == 24) {
            canvas_spec[i]->Print((histogram_fn + ")").c_str(), "pdf");
        } else {
            canvas_spec[i]->Print(histogram_fn.c_str(), "pdf");
        }
    }
  
    return 0;
}
