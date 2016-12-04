#include <iostream>
#include "TFile.h"
#include "TTree.h"

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
    } t_event;

    // set address
    t_event_tree->SetBranchAddress("ct_time_second",      &t_event.ct_time_second          );
    t_event_tree->SetBranchAddress("time_aligned",         t_event.time_aligned            );
    t_event_tree->SetBranchAddress("trigger_bit",          t_event.trigger_bit             );

    // select branch
    t_event_tree->SetBranchStatus("*", false);
    t_event_tree->SetBranchStatus("ct_time_second", true);
    t_event_tree->SetBranchStatus("time_aligned", true);
    t_event_tree->SetBranchStatus("trigger_bit", true);

    // get the first and last time
    t_event_tree->GetEntry(0);

    cout << "reading data ..." << endl;
    for (Long64_t i = 0; i < t_event_tree->GetEntries(); i++) {
        t_event_tree->GetEntry(i);

    }
    cout << "done." << endl;

    t_file_in->Close();

    return 0;
}
