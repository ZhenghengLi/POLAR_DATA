#include <iostream>
#include "RootInc.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << " <rate_deadtime.root> <dist_inten_prof.root> <outfile.root>" << endl;
        return 2;
    }
    string rate_deadtime_filename = argv[1];
    string dist_inten_prof_filename = argv[2];
    string outfile_fn = argv[3];

    // read rate_deadtime_file
    TFile* rate_deadtime_file = new TFile(rate_deadtime_filename.c_str(), "read");
    if (rate_deadtime_file->IsZombie()) {
        cout << "read_deadtime_file open failed." << endl;
        return 1;
    }
    TH1F* rate_hist[25][64];
    TH1F* dead_ratio_hist[25];
    for (int i = 0; i < 25; i++) {
        dead_ratio_hist[i] = static_cast<TH1F*>(rate_deadtime_file->Get(Form("rate_hist_CT_%02d/dead_ratio_hist_CT_%02d", i + 1, i + 1)));
        if (dead_ratio_hist[i] == NULL) {
            cout << "cannot find TH1F " << Form("dead_ratio_hist_CT_%02d", i + 1) << endl;
        }
        for (int j = 0; j < 64; j++) {
            rate_hist[i][j] = static_cast<TH1F*>(rate_deadtime_file->Get(Form("rate_hist_CT_%02d/rate_hist_CT_%02d_%02d", i + 1, i + 1, j)));
            if (rate_hist[i][j] == NULL) {
                cout << "cannot find TH1F " << Form("rate_hist_CT_%02d_%02d", i + 1, j) << endl;
                return 1;
            }
            // dead time correction
            for (int k = 1; k <= rate_hist[i][j]->GetNbinsX(); k++) {
                rate_hist[i][j]->SetBinContent(k, rate_hist[i][j]->GetBinContent(k) / (1 - dead_ratio_hist[i]->GetBinContent(k)));
            }
        }
    }

    // read dist_inten_prof
    TFile* dist_inten_prof_file = new TFile(dist_inten_prof_filename.c_str(), "read");
    if (dist_inten_prof_file->IsZombie()) {
        cout << "dist_inten_prof_file open failed." << endl;
        return 1;
    }
    TProfile* intensity_prof;
    TProfile* distance_prof[25][64];
    intensity_prof = static_cast<TProfile*>(dist_inten_prof_file->Get("intensity_prof"));
    if (intensity_prof == NULL) {
        cout << "cannot find TProfile intensity_prof" << endl;
        return 1;
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            distance_prof[i][j] = static_cast<TProfile*>(dist_inten_prof_file->Get(Form("CT_%02d/distance_prof_CT_%02d_%02d", i + 1, i + 1, j)));
            if (distance_prof[i][j] == NULL) {
                cout << "cannot find TProfile " << Form("distance_prof_CT_%02d_%02d", i + 1, j) << endl;
                return 1;
            }
        }
    }

    cout << "open output file" << endl;
    // open output file
    TFile* output_file = new TFile(outfile_fn.c_str(), "recreate");
    output_file->cd();
    TProfile* rate_vs_dist[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            rate_vs_dist[i][j] = new TProfile(Form("dist_vs_rate_CT_%02d_%02d", i + 1, j),
                    Form("distance_prof_CT_%02d_%02d", i + 1, j), 100,
                    0, 50, 0, 4000);
            rate_vs_dist[i][j]->SetDirectory(NULL);
        }
    }

    cout << "fill rate_vs_dist" << endl;
    // fill rate_vs_dist
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 1; k <= rate_hist[i][j]->GetNbinsX(); k++) {
                if (rate_hist[i][j]->GetNbinsX() != distance_prof[i][j]->GetNbinsX()) {
                    cout << "problem" << i << " " << j << endl;
                }
                rate_vs_dist[i][j]->Fill(distance_prof[i][j]->GetBinContent(k), rate_hist[i][j]->GetBinContent(k));
            }
        }
    }

    cout << "save rate_vs_dist" << endl;
    // save rate_vs_dist
    for (int i = 0; i < 25; i++) {
        output_file->mkdir(Form("CT_%02d", i + 1))->cd();
        for (int j = 0; j < 64; j++) {
            rate_vs_dist[i][j]->Write();
        }
    }
    output_file->Close();

    return 0;
}
