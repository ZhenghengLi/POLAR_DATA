#include <iostream>
#include <algorithm>
#include "POLEvent.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <input.root> <output.root>" << endl;
        return 2;
    }

    string input_filename = argv[1];
    string output_filename = argv[2];

    // read event data
    TFile* pol_event_file = new TFile(input_filename.c_str(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << input_filename << endl;
        return 1;
    }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);

    TTree* t_raw_energy_tree = static_cast<TTree*>(pol_event_file->Get("t_raw_energy"));
    if (t_raw_energy_tree == NULL) {
        cout << "cannot find TTree t_raw_energy." << endl;
        return 1;
    }
    if (t_raw_energy_tree->GetEntries() != t_pol_event_tree->GetEntries()) {
        cout << "wrong raw_energy TTree" << endl;
        return 1;
    }
    Float_t raw_energy[25][64];
    t_raw_energy_tree->SetBranchAddress("raw_energy", raw_energy);

    // open output file
    TFile* pol_event_file_subped = new TFile(output_filename.c_str(), "recreate");
    if (pol_event_file_subped->IsZombie()) {
        cout << "pol_event_file_subped open failed: " << output_filename << endl;
        return 1;
    }
    TTree* t_pol_event_tree_new = t_pol_event_tree->CloneTree(0);

    // do pedestal and common noise subtraction
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "copying raw energy ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);
        t_raw_energy_tree->GetEntry(q);

        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 64; j++) {
                t_pol_event.energy_value[i][j] = raw_energy[i][j];
            }
        }

        t_pol_event_tree_new->Fill();
    }
    cout << " DONE ]" << endl;

    // write TTree
    pol_event_file_subped->cd();
    t_pol_event_tree_new->Write();

    cout << "writing meta information ... " << endl;
    // write meta
    TIter fileIter(pol_event_file->GetListOfKeys());
    TKey* key = NULL;
    TNamed* meta = NULL;
    while ((key = static_cast<TKey*>(fileIter.Next())) != NULL) {
        if (string(key->GetClassName()) != "TNamed") continue;
        if (string(key->GetName()) == "m_energy_unit") {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        } else if (string(key->GetName()) == "m_level_num") {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->SetTitle("1");
            meta->Write();
        } else {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        }
    }

    // close output file
    pol_event_file_subped->Close();
    delete pol_event_file_subped;
    pol_event_file_subped = NULL;

    cout << "[ DONE ]" << endl;

    return 0;
}
