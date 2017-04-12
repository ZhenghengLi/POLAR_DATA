#include <iostream>
#include "RootInc.hpp"
#include "CooConv.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << " <event_data.root> <rate_data.root> <output.root>" << endl;
        return 2;
    }
    string event_data_fn = argv[1];
    string rate_data_fn = argv[2];
    string output_fn = argv[3];

    // open event data file
    TFile* t_file_in = new TFile(event_data_fn.c_str(), "read");
    if (t_file_in->IsZombie()) {
        cout << "root file open failed" << endl;
        return 1;
    }
    TTree* t_event_tree = static_cast<TTree*>(t_file_in->Get("t_event"));
    if (t_event_tree == NULL) {
        cout << "cannot find TTree t_event." << endl;
        return 1;
    }
    struct {
        Double_t   ct_time_second;
        Bool_t     time_aligned[25];
        Bool_t     trigger_bit[25][64];
        Float_t    energy_value[25][64];
        Int_t      multiplicity[25];
    } t_event;
    t_event_tree->SetBranchAddress("ct_time_second",        &t_event.ct_time_second     );
    t_event_tree->SetBranchAddress("time_aligned",           t_event.time_aligned       );
    t_event_tree->SetBranchAddress("trigger_bit",            t_event.trigger_bit        );
    t_event_tree->SetBranchAddress("energy_value",           t_event.energy_value       );
    t_event_tree->SetBranchAddress("multiplicity",           t_event.multiplicity       );

    t_event_tree->SetBranchStatus("*", false);
    t_event_tree->SetBranchStatus("ct_time_second", true);
    t_event_tree->SetBranchStatus("time_aligned", true);
    t_event_tree->SetBranchStatus("trigger_bit", true);
    t_event_tree->SetBranchStatus("energy_value", true);
    t_event_tree->SetBranchStatus("multiplicity", true);

    // read time range;
    TFile* t_file_rate = new TFile(rate_data_fn.c_str(), "read");
    if (t_file_rate->IsZombie()) {
        cout << "root file open failed" << endl;
        return 1;
    }
    TMatrixF begin_time_mat(25, 64);
    TMatrixF end_time_mat(25, 64);
    TMatrixF* tmp_mat;
    tmp_mat = static_cast<TMatrixF*>(t_file_rate->Get("begin_time_mat"));
    if (tmp_mat == NULL) {
        cout << "cannot file TMatrixF begin_time_mat" << endl;
        return 1;
    } else {
        begin_time_mat = *tmp_mat;
    }
    tmp_mat = static_cast<TMatrixF*>(t_file_rate->Get("end_time_mat"));
    if (tmp_mat == NULL) {
        cout << "cannot file TMatrixF end_time_mat" << endl;
        return 1;
    } else {
        end_time_mat = *tmp_mat;
    }
    t_file_rate->Close();
    delete t_file_rate;
    t_file_rate = NULL;

    // open output file
    TFile* t_file_out = new TFile(output_fn.c_str(), "recreate");
    if (t_file_out->IsZombie()) {
        cout << "root file open failed." << endl;
        return 1;
    }

    TH1F* hist_spec[25][64];
    TF1*  func_hist[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            hist_spec[i][j] = new TH1F(Form("hist_spec_%02d_%02d", i + 1, j + 1), Form("hist_spec_%02d_%02d", i + 1, j + 1), 128, 0, 4096);
            func_hist[i][j] = new TF1(Form("func_hist_%d_%d", i + 1, j + 1), "TMath::Exp([0] + [1] * x) + gaus(2) + [5]", 300, 4096);
            func_hist[i][j]->SetParameters(7, -0.006, 50, 1500, 500, 5);
            func_hist[i][j]->SetParLimits(1, -0.1, -0.0001);
            func_hist[i][j]->SetParLimits(2, 1, 10000);
            func_hist[i][j]->SetParLimits(3, 20, 3500);
            func_hist[i][j]->SetParLimits(4, 100, 2000);
            func_hist[i][j]->SetParLimits(5, 0, 1000);
       }
    }
    // reading data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_event_tree->GetEntry(q);
        for (int i = 0; i < 25; i++) {
            if (!t_event.time_aligned[i]) continue;
            // if (t_event.multiplicity[i] != 2) continue;
            for (int j = 0; j < 64; j++) {
                if (t_event.trigger_bit[i][j] && t_event.ct_time_second > begin_time_mat(i, j) && t_event.ct_time_second < end_time_mat(i, j)) {
                    hist_spec[i][j]->Fill(t_event.energy_value[i][j]);
                }
            }
        }
    }
    cout << " DONE ]" << endl;
    t_file_in->Close();

    // saving hist
    cout << "saving histogram ..." << endl;
    for (int i = 0; i < 25; i++) {
        t_file_out->cd();
        t_file_out->mkdir(Form("spec_CT_%02d", i + 1))->cd();
        for (int j = 0; j < 64; j++) {
            //hist_spec[i][j]->Fit(func_hist[i][j], "RQ");
            hist_spec[i][j]->Write();
        }
    }
    t_file_out->Close();

    return 0;
}
