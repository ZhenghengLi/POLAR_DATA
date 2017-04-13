#include <iostream>
#include <algorithm>
#include "OptionsManager.hpp"
#include "Crosstalk.hpp"
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

    // prepare Crosstalk
    Crosstalk xtalk_obj;
    if (!xtalk_obj.read_crosstalk(options_mgr.xtalk_matrix_filename.Data())) {
        cout << "Crosstalk matrix read failed: " << options_mgr.xtalk_matrix_filename.Data() << endl;
        return 1;
    }
    xtalk_obj.gen_crosstalk();

    // read event data
    TFile* pol_event_file = new TFile(options_mgr.pol_event_filename.Data(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << options_mgr.pol_event_filename.Data() << endl;
        return 1;
    }
    TNamed* m_level_num = static_cast<TNamed*>(pol_event_file->Get("m_level_num"));
    if (m_level_num == NULL) {
        cout << "cannot find TNamed m_level_num." << endl;
        return 1;
    }
    if (TString(m_level_num->GetTitle()).Atoi() != 1) {
        cout << "m_level_num is not 1, need to do pedestal and common noise subtraction first." << endl;
        return 1;
    }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);

    // open output file
    TFile* pol_event_file_xtalkcorr = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (pol_event_file_xtalkcorr->IsZombie()) {
        cout << "pol_event_file_xtalkcorr open failed: " << options_mgr.output_filename.Data() << endl;
        return 1;
    }
    TTree* t_pol_event_tree_new = t_pol_event_tree->CloneTree(0);

    TVectorF energy_value_vector;
    energy_value_vector.ResizeTo(64);

    // do crosstalk correction
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Doing crosstalk correcting ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);

        // (1) do crosstalk correction
        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) continue;
            copy(t_pol_event.energy_value[i], t_pol_event.energy_value[i] + 64, energy_value_vector.GetMatrixArray());
            energy_value_vector = xtalk_obj.xtalk_matrix_inv_CT[i] * energy_value_vector;
            copy(energy_value_vector.GetMatrixArray(), energy_value_vector.GetMatrixArray() + 64, t_pol_event.energy_value[i]);
        }

        // (2) store crosstalk corrected data
        t_pol_event_tree_new->Fill();
    }
    cout << " DONE ]" << endl;

    // write TTree
    pol_event_file_xtalkcorr->cd();
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
            meta->SetTitle("2");
            meta->Write();
        } else {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        }
    }

    // close output file
    pol_event_file_xtalkcorr->Close();
    delete pol_event_file_xtalkcorr;
    pol_event_file_xtalkcorr = NULL;

    cout << "[ DONE ]" << endl;

    return 0;
}
