#include <iostream>
#include "RootInc.hpp"
#include "CooConv.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <event_data.root>" << endl;
        return 2;
    }
    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);
    // open event data
    TFile* t_event_file = new TFile(argv[1], "read");
    if (t_event_file->IsZombie()) {
        cout << "event root file open failed." << endl;
        return 1;
    }
    TTree* t_event_tree = static_cast<TTree*>(t_event_file->Get("t_event"));
    struct {
        Int_t    type;
        Double_t ct_time_second;
        Bool_t   time_aligned[25];
        Bool_t   trigger_bit[25][64];
        Float_t  energy_value[25][64];
    } t_event;
    // t_event_tree->SetBranchAddress("type",             &t_event.type             );
    t_event_tree->SetBranchAddress("ct_time_second",   &t_event.ct_time_second   );
    t_event_tree->SetBranchAddress("time_aligned",      t_event.time_aligned     );
    t_event_tree->SetBranchAddress("trigger_bit",       t_event.trigger_bit      );
    // t_event_tree->SetBranchAddress("energy_value",      t_event.energy_value     );

    t_event_tree->SetBranchStatus("*", false);
    // t_event_tree->SetBranchStatus("type", true);
    t_event_tree->SetBranchStatus("ct_time_second", true);
    t_event_tree->SetBranchStatus("time_aligned", true);
    t_event_tree->SetBranchStatus("trigger_bit", true);
    // t_event_tree->SetBranchStatus("energy_value", true);

    t_event_tree->GetEntry(0);
    double ct_time_start = t_event.ct_time_second;
    t_event_tree->GetEntry(t_event_tree->GetEntries() - 1);
    double ct_time_stop = t_event.ct_time_second;

    int count_mat[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            count_mat[i][j] = 0;
        }
    }
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
            for (int j = 0; j < 64; j++) {
                if (t_event.trigger_bit[i][j]) {
                    count_mat[i][j]++;
                }
            }
        }
    }
    cout << " DONE ]" << endl;

    TH2F* hit_map_hist = new TH2F("hit_map_hist", "mean rate of 1600 channel", 40, 0, 40, 40, 0, 40);
    hit_map_hist->SetDirectory(NULL);
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            hit_map_hist->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, count_mat[i][j]);
        }
    }
    hit_map_hist->Scale(1.0 / (ct_time_stop - ct_time_start));

    gStyle->SetOptStat(0);
    new TCanvas();
    hit_map_hist->Draw("colz");

    rootapp->Run();

    return 0;
}
