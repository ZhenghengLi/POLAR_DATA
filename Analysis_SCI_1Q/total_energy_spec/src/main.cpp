#include <iostream>
#include "RootInc.hpp"
#include "CommonCanvas.hpp"
#include "CooConv.hpp"

// for ADC
#define VALUE_MAX 2000

// for energy
//#define VALUE_MAX 400

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <event.root>" << endl;
        return 2;
    }

    string pol_event_filename = argv[1];

    TFile* pol_event_file = new TFile(pol_event_filename.c_str(), "read");
    TTree* pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (pol_event_tree == NULL) {
        pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_event"));
    }
    if (pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event or t_event." << endl;
        return 1;
    }
    struct {
        Bool_t time_aligned[25];
        Int_t  lost_count;
        Bool_t trigger_bit[25][64];
        Float_t energy_value[25][64];
    } pol_event;
    pol_event_tree->SetBranchAddress("time_aligned", pol_event.time_aligned);
    pol_event_tree->SetBranchAddress("lost_count", &pol_event.lost_count);
    pol_event_tree->SetBranchAddress("trigger_bit", pol_event.trigger_bit);
    pol_event_tree->SetBranchAddress("energy_value", pol_event.energy_value);
    pol_event_tree->SetBranchStatus("*", false);
    pol_event_tree->SetBranchStatus("time_aligned", true);
    pol_event_tree->SetBranchStatus("lost_count", true);
    pol_event_tree->SetBranchStatus("trigger_bit", true);
    pol_event_tree->SetBranchStatus("energy_value", true);

    TH1F* total_energy_spec = new TH1F("total_energy_spec", "total_energy_spec", 500, 0, 2000);
    total_energy_spec->SetDirectory(NULL);

    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        pol_event_tree->GetEntry(q);
        if (pol_event.lost_count > 0) continue;
        double total_energy = 0;
        for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 64; j++) {
                if (pol_event.trigger_bit[i][j]) {
                    total_energy += pol_event.energy_value[i][j];
                }
            }
        }
        total_energy_spec->Fill(total_energy);
    }
    cout << " DONE ]" << endl;
    pol_event_file->Close();
    delete pol_event_file;
    pol_event_file = NULL;


    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);
    CommonCanvas canvas;
    canvas.cd();
    total_energy_spec->Draw("H");

    rootapp->Run();
    return 0;
}
