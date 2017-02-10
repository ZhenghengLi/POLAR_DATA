#include <iostream>
#include "RootInc.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <event_data.root> <rate_data.root>" << endl;
        return 1;
    }
    string event_data_filename = argv[1];
    string rate_data_filename = argv[2];

    TFile* t_file_in = new TFile(event_data_filename.c_str(), "read");
    if (t_file_in->IsZombie()) {
        cout << "root file open failed" << endl;
        return 1;
    }
    TTree* t_event_tree = static_cast<TTree*>(t_file_in->Get("t_event"));
    if (t_event_tree == NULL) {
        cout << "cannot find TTree t_event" << endl;
        return 1;
    }

    struct {
        Double_t ct_time_second;
        Bool_t   time_aligned[25];
        Bool_t   trigger_bit[25][64];
        Int_t    raw_rate[25];
    } t_event;

    // set address
    t_event_tree->SetBranchAddress("ct_time_second",      &t_event.ct_time_second          );
    t_event_tree->SetBranchAddress("time_aligned",         t_event.time_aligned            );
    t_event_tree->SetBranchAddress("trigger_bit",          t_event.trigger_bit             );
    t_event_tree->SetBranchAddress("raw_rate",             t_event.raw_rate                );

    // select branch
    t_event_tree->SetBranchStatus("*", false);
    t_event_tree->SetBranchStatus("ct_time_second", true);
    t_event_tree->SetBranchStatus("time_aligned", true);
    t_event_tree->SetBranchStatus("trigger_bit", true);
    t_event_tree->SetBranchStatus("raw_rate", true);

    // get the first and last time
    t_event_tree->GetEntry(0);
    double ct_time_second_first = t_event.ct_time_second;
    t_event_tree->GetEntry(t_event_tree->GetEntries() - 1);
    double ct_time_second_last  = t_event.ct_time_second;

    // prepare histogram
    double binw = 0.1;
    int nbins = static_cast<int>((ct_time_second_last - ct_time_second_first) / binw);
    TH1F* rate_hist[25][64];
    TH1F* dead_ratio_hist[25];
    for (int i = 0; i < 25; i++) {
        dead_ratio_hist[i] = new TH1F(Form("dead_ratio_hist_CT_%02d", i + 1),
                Form("dead_ratio_hist_CT_%02d", i + 1), nbins,
                ct_time_second_first, ct_time_second_last);
        dead_ratio_hist[i]->SetDirectory(NULL);
        for (int j = 0; j < 64; j++) {
            rate_hist[i][j] = new TH1F(Form("rate_hist_CT_%02d_%02d", i + 1, j),
                    Form("rate_hist_CT_%02d_%02d", i + 1, j), nbins,
                    ct_time_second_first, ct_time_second_last);
            rate_hist[i][j]->SetDirectory(NULL);
        }
    }

    cout << "reading data ..." << endl;
    for (Long64_t q = 0; q < t_event_tree->GetEntries(); q++) {
        t_event_tree->GetEntry(q);
        for (int i = 0; i < 25; i++) {
            if (!t_event.time_aligned[i]) continue;
            dead_ratio_hist[i]->Fill(t_event.ct_time_second, (t_event.raw_rate[i] + 1) * 68.82E-6);
            for (int j = 0; j < 64; j++) {
                if (t_event.trigger_bit[i][j]) {
                    rate_hist[i][j]->Fill(t_event.ct_time_second);
                }
            }
        }
    }
    cout << "done." << endl;
    t_file_in->Close();

    // calc bar beam time window
    TMatrixF begin_time_mat(25, 64);
    TMatrixF end_time_mat(25, 64);
    TMatrixF max_rate_mat(25, 64);
    TMatrixF max_time_mat(25, 64);
    float ratio = 0.35;
    for (int i = 0; i < 25; i++) {
        dead_ratio_hist[i]->Scale(1, "width");
        for (int j = 0; j < 64; j++) {
            rate_hist[i][j]->Scale(1, "width");
            int maximum_bin = rate_hist[i][j]->GetMaximumBin();
            max_time_mat[i][j] = rate_hist[i][j]->GetBinCenter(maximum_bin);
            max_rate_mat[i][j] = rate_hist[i][j]->GetBinContent(maximum_bin);
            int begin_bin = maximum_bin;
            while (begin_bin > 0 && begin_bin > (maximum_bin - 1.5 / binw)) {
                begin_bin--;
                if (rate_hist[i][j]->GetBinContent(begin_bin) < ratio * max_rate_mat[i][j]) {
                    break;
                }
            }
            begin_time_mat[i][j] = rate_hist[i][j]->GetBinCenter(begin_bin);
            int end_bin   = maximum_bin;
            while (end_bin < nbins && end_bin < (maximum_bin + 1.5 / binw)) {
                end_bin++;
                if (rate_hist[i][j]->GetBinContent(end_bin) < ratio * max_rate_mat[i][j]) {
                    break;
                }
            }
            end_time_mat[i][j] = rate_hist[i][j]->GetBinCenter(end_bin);
        }
    }

    cout << "wrtie rate vs. time ..." << endl;
    TFile* t_file_out = new TFile(rate_data_filename.c_str(), "recreate");
    if (t_file_out->IsZombie()) {
        cout << "rate data file open failed." << endl;
        return 1;
    }
    for (int i = 0; i < 25; i++) {
        t_file_out->mkdir(Form("rate_hist_CT_%02d", i + 1))->cd();
        dead_ratio_hist[i]->Write();
        for (int j = 0; j < 64; j++) {
            rate_hist[i][j]->Write();
        }
    }
    t_file_out->cd();
    begin_time_mat.Write("begin_time_mat");
    end_time_mat.Write("end_time_mat");
    max_rate_mat.Write("max_rate_mat");
    max_time_mat.Write("max_time_mat");
    TNamed("m_eventfile", TSystem().BaseName(event_data_filename.c_str())).Write();
    TNamed("m_ct_time_second_first", Form("%f", ct_time_second_first)).Write();
    TNamed("m_ct_time_second_last",  Form("%f", ct_time_second_last)).Write();
    TNamed("m_nbins",Form("%d", nbins)).Write();
    t_file_out->Close();
    return 0;
}
