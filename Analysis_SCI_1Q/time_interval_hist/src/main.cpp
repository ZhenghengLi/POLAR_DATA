#include <iostream>
#include "RootInc.hpp"
#include "POLEvent.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <SCI_1Q.root> <output.root>" << endl;
        return 2;
    }
    string pol_event_filename = argv[1];
    string output_filename = argv[2];

    TFile* pol_event_file = new TFile(pol_event_filename.c_str(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << pol_event_filename << endl;
        return 1;
    }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event" << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);
    t_pol_event.deactive_all(t_pol_event_tree);
    t_pol_event.active(t_pol_event_tree, "event_time");

    TFile* output_file = new TFile(output_filename.c_str(), "recreate");
    if (output_file->IsZombie()) {
        cout << "output_file open failed: " << output_filename << endl;
        return 1;
    }
    TH1D* h1_time_interval = new TH1D("h1_time_interval", "h1_time_interval", 240, 0, 120E-6);

    bool is_first = true;
    double pre_event_time;

    int pre_percent = 0;
    int cur_percent = 0;
    cout << "read event_time ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);
        if (is_first) {
            pre_event_time = t_pol_event.event_time;
            is_first = false;
        } else {
            h1_time_interval->Fill(t_pol_event.event_time - pre_event_time);
            pre_event_time = t_pol_event.event_time;
        }
    }
    cout << " DONE ]" << endl;

    output_file->cd();
    h1_time_interval->Write();
    output_file->Close();
    delete output_file;

    pol_event_file->Close();
    delete pol_event_file;

    return 0;
}
