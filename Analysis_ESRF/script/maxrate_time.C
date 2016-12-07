#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TNamed.h"
#include "TString.h"

using namespace std;

void maxrate_time() {
    string rate_filename;
    cout << "rate_filename = " << flush;
    cin >> rate_filename;
    string out_filename;
    cout << "out_filename = " << flush;
    cin >> out_filename;
    ofstream out_file;
    out_file.open(out_filename.c_str());
    if (!out_file.is_open()) {
        cout << "out_file open failed." << endl;
        return;
    }
    TFile* t_file_in = new TFile(rate_filename.c_str(), "read");
    if (t_file_in->IsZombie()) {
        cout << "root file open failed" << endl;
        return;
    }
    TH1F* rate_hist[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            rate_hist[i][j] = static_cast<TH1F*>(t_file_in->Get(
                        Form("rate_hist_CT_%02d/rate_hist_CT_%02d_%02d", i + 1, i + 1, j)));
        }
    }
    TNamed* m_ct_time_second_first = static_cast<TNamed*>(t_file_in->Get("m_ct_time_second_first"));
    TNamed* m_ct_time_second_last  = static_cast<TNamed*>(t_file_in->Get("m_ct_time_second_last"));
    TNamed* m_nbins = static_cast<TNamed*>(t_file_in->Get("m_nbins"));
    double ct_time_second_first = TString(m_ct_time_second_first->GetTitle()).Atof();
    double ct_time_second_last  = TString(m_ct_time_second_last->GetTitle()).Atof();
    int nbins = TString(m_nbins->GetTitle()).Atoi();
    TGraph* gr = new TGraph();
    int pre_i(0), pre_j(0);
    double step = 0.1;
    for (int q = 0; q < nbins; q++) {
        double cur_time_second = ct_time_second_first + q * step;
        double max_rate = 0;
        int max_i(0), max_j(0);
        for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 64; j++) {
                if (max_rate < rate_hist[i][j]->Interpolate(cur_time_second)) {
                    max_rate = rate_hist[i][j]->Interpolate(cur_time_second);
                    max_i = i;
                    max_j = j;
                }
            }
        }
        gr->SetPoint(q, cur_time_second, max_rate);
        if ((max_i != pre_i || max_j != pre_j) && max_rate > 1000) {
            out_file << Form("%02d %02d %15.5f %15.5f", max_i + 1, max_j + 1, max_rate, cur_time_second) << endl;
            pre_i = max_i;
            pre_j = max_j;
        }
    }
    gr->Draw();
    out_file.close();
    t_file_in->Close();
}
