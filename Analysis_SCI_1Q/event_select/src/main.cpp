#include <iostream>
#include <algorithm>
#include "OptionsManager.hpp"
#include "POLEvent.hpp"

using namespace std;

int main(int argc, char** argv) {
    OptionsManager options_mgr;
    if (!options_mgr.parse(argc, argv)) {
        if (options_mgr.get_version_flag()) {
            options_mgr.print_version();
        } else {
            options_mgr.print_help();
        }
        return 2;
    }

    // read event data
    TFile* pol_event_file = new TFile(options_mgr.pol_event_filename.Data(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << options_mgr.pol_event_filename.Data() << endl;
        return 1;
    }
    // TNamed* m_level_num = static_cast<TNamed*>(pol_event_file->Get("m_level_num"));
    // if (m_level_num != NULL) {
    //     if (TString(m_level_num->GetTitle()).Atoi() != 0) {
    //         cout << "m_level_num is not 0." << endl;
    //         return 1;
    //     }
    // }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);

    // open event type file
    TFile* event_type_file = new TFile(options_mgr.event_type_filename.Data(), "read");
    if (event_type_file->IsZombie()) {
        cout << "event_type_file open failed: " << options_mgr.event_type_filename.Data() << endl;
        return 1;
    }
    TTree* event_type_tree = static_cast<TTree*>(event_type_file->Get("t_event_type"));
    if (event_type_tree == NULL) {
        cout << "Cannot find TTree t_event_type. " << endl;
        return 1;
    }
    if (event_type_tree->GetEntries() != t_pol_event_tree->GetEntries()) {
        cout << "use a wrong event_type_file" << endl;
        return 1;
    }
    UShort_t event_type;
    event_type_tree->SetBranchAddress("event_type", &event_type);

    // open output file
    TFile* pol_event_file_good = new TFile(options_mgr.output_good_filename.Data(), "recreate");
    if (pol_event_file_good->IsZombie()) {
        cout << "pol_event_file_sel open failed: " << options_mgr.output_good_filename.Data() << endl;
        return 1;
    }
    pol_event_file_good->cd();
    TTree* t_pol_event_tree_good = t_pol_event_tree->CloneTree(0);

    TFile* pol_event_file_bad = new TFile(options_mgr.output_bad_filename.Data(), "recreate");
    if (pol_event_file_bad->IsZombie()) {
        cout << "pol_event_file_sel open failed: " << options_mgr.output_bad_filename.Data() << endl;
        return 1;
    }
    pol_event_file_bad->cd();
    TTree* t_pol_event_tree_bad = t_pol_event_tree->CloneTree(0);

    // do event selecting
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Doing event selecting ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);
        event_type_tree->GetEntry(q);

        if (event_type > 0) {
            t_pol_event_tree_bad->Fill();
        } else {
            t_pol_event_tree_good->Fill();
        }

    }
    cout << " DONE ]" << endl;

    cout << "writing meta information ... " << endl;
    TKey* key = NULL;
    TNamed* meta = NULL;

    //// good
    // write TTree
    pol_event_file_good->cd();
    t_pol_event_tree_good->Write();
    // write meta
    TIter fileIter_1(pol_event_file->GetListOfKeys());
    while ((key = static_cast<TKey*>(fileIter_1.Next())) != NULL) {
        if (string(key->GetClassName()) != "TNamed") continue;
        if (string(key->GetName()) == "m_energy_unit") {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        } else if (string(key->GetName()) == "m_level_num") {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        } else {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        }
    }
    // close output file
    pol_event_file_good->Close();
    delete pol_event_file_good;
    pol_event_file_good = NULL;

    //// bad
    // write TTree
    pol_event_file_bad->cd();
    t_pol_event_tree_bad->Write();
    // write meta
    TIter fileIter_2(pol_event_file->GetListOfKeys());
    while ((key = static_cast<TKey*>(fileIter_2.Next())) != NULL) {
        if (string(key->GetClassName()) != "TNamed") continue;
        if (string(key->GetName()) == "m_energy_unit") {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        } else if (string(key->GetName()) == "m_level_num") {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        } else {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        }
    }
    // close output file
    pol_event_file_bad->Close();
    delete pol_event_file_bad;
    pol_event_file_bad = NULL;

    pol_event_file->Close();
    event_type_file->Close();

    cout << "[ DONE ]" << endl;

    return 0;
}
