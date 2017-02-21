#include <iostream>
#include "RootInc.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <event.root> <deadtime.root>" << endl;
        return 2;
    }
    string event_filename = argv[1];
    string deadtime_filename = argv[2];

    // open event file
    TFile* event_file = new TFile(event_filename.c_str(), "read");
    if (event_file->IsZombie()) {
        cout << "event file open failed" << endl;
        return 1;
    }
    TTree* t_event_tree = static_cast<TTree*>(event_file->Get("t_event"));
    if (t_event_tree == NULL) {
        cout << "cannot find TTree t_event" << endl;
        return 1;
    }
    struct {
        Double_t ct_time_second;
        Bool_t   trig_accepted[25];
        Int_t    raw_rate[25];
    } t_event;
    // set address
    t_event_tree->SetBranchAddress("ct_time_second",      &t_event.ct_time_second          );
    t_event_tree->SetBranchAddress("trig_accepted",        t_event.trig_accepted           );
    t_event_tree->SetBranchAddress("raw_rate",             t_event.raw_rate                );
    // select branch
    t_event_tree->SetBranchStatus("*", false);
    t_event_tree->SetBranchStatus("ct_time_second", true);
    t_event_tree->SetBranchStatus("trig_accepted", true);
    t_event_tree->SetBranchStatus("raw_rate", true);
    // get the first and last time
    t_event_tree->GetEntry(0);
    double ct_time_second_first = t_event.ct_time_second;
    t_event_tree->GetEntry(t_event_tree->GetEntries() - 1);
    double ct_time_second_last  = t_event.ct_time_second;

    // prepare histogram
    double binw = 0.1;
    int nbins = static_cast<int>((ct_time_second_last - ct_time_second_first) / binw);
    TH1F* dead_ratio_hist[25];
    for (int i = 0; i < 25; i++) {
        dead_ratio_hist[i] = new TH1F(Form("dead_ratio_hist_CT_%02d", i + 1),
                Form("dead_ratio_hist_CT_%02d", i + 1), nbins,
                ct_time_second_first, ct_time_second_last);
        dead_ratio_hist[i]->SetDirectory(NULL);
    }
    cout << "reading data ..." << endl;
    for (Long64_t q = 0; q < t_event_tree->GetEntries(); q++) {
        t_event_tree->GetEntry(q);
        for (int i = 0; i < 25; i++) {
            if (t_event.trig_accepted[i]) {
                dead_ratio_hist[i]->Fill(t_event.ct_time_second, (1.0 * 68.82E-6));
            }
        }
    }
    cout << "done." << endl;
    for (int i = 0; i < 25; i++) {
        dead_ratio_hist[i]->Scale(1, "width");
    }
    struct {
        Double_t ct_time_second_5;
        Float_t  module_dead_ratio[25];
    } t_dead_ratio;
    TFile* deadtime_file = new TFile(deadtime_filename.c_str(), "recreate");
    TTree* t_dead_ratio_tree = new TTree("t_dead_ratio", "deadtime ratio calculated by a larger bin size");
    t_dead_ratio_tree->Branch("ct_time_second_5",     &t_dead_ratio.ct_time_second_5,    "ct_time_second_5/D"        );
    t_dead_ratio_tree->Branch("module_dead_ratio",     t_dead_ratio.module_dead_ratio,   "module_dead_ratio[25]/F"   );
    cout << "writing deadtime ..." << endl;
    for (Long64_t q = 0; q < t_event_tree->GetEntries(); q++) {
        t_event_tree->GetEntry(q);
        t_dead_ratio.ct_time_second_5 = t_event.ct_time_second;
        for (int i = 0; i < 25; i++) {
            t_dead_ratio.module_dead_ratio[i] = dead_ratio_hist[i]->Interpolate(t_event.ct_time_second);
        }
        t_dead_ratio_tree->Fill();
    }
    cout << "done." << endl;
    deadtime_file->cd();
    t_dead_ratio_tree->Write();
    TNamed("binsize", "0.1 second").Write();
    deadtime_file->Close();
    event_file->Close();

    return 0;
}
