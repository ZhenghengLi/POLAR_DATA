#include <iostream>
#include <algorithm>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "POLEvent.hpp"
#include "Gain.hpp"

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

    // prepare gain
    Gain gain_obj;
    if (options_mgr.gain_hv_flag) {
        if (!gain_obj.read_gain_vs_hv(options_mgr.gain_vs_hv_filename.Data())) {
            cout << "gain_vs_hv read failed: " << options_mgr.gain_vs_hv_filename.Data() << endl;
            return 1;
        }
    } else {
        if (!gain_obj.read_gain_vec(options_mgr.gain_vec_filename.Data())) {
            cout << "gain_vec read failed: " << options_mgr.gain_vec_filename.Data() << endl;
            return 1;
        } else {
            gain_obj.gen_gain();
        }
    }

    // read event data
    TFile* pol_event_file = new TFile(options_mgr.pol_event_filename.Data(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << options_mgr.pol_event_filename.Data() << endl;
        return 1;
    }
    TNamed* m_level_num = static_cast<TNamed*>(pol_event_file->Get("m_level_num"));
    if (m_level_num != NULL) {
        if (TString(m_level_num->GetTitle()).Atoi() != 2) {
            cout << "m_level_num is not 2, need to do pedestal & common noise subtraction and crosstalk correction." << endl;
            return 1;
        }
    }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);

    // open output file
    TFile* pol_event_file_energy = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (pol_event_file_energy->IsZombie()) {
        cout << "pol_event_file_energy open failed: " << options_mgr.output_filename.Data() << endl;
        return 1;
    }
    TTree* t_pol_event_tree_new = t_pol_event_tree->CloneTree(0);

    TVectorF energy_value_vector;
    energy_value_vector.ResizeTo(64);

    // do energy reconstruction
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Doing energy reconstructing ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);

        // (1) convert ADC to keV
        for (int i = 0; i < 25; i++) {
            if (t_pol_event.time_aligned[i]) {
                if (options_mgr.gain_hv_flag) {
                    gain_obj.gen_gain(i + 1, t_pol_event.fe_hv[i]);
                }
                for (int j = 0; j < 64; j++) {
                    if (gain_obj.bad_calib_mat(i, j)) {
                        t_pol_event.channel_status[i][j] += POLEvent::BAD_CALIB;
                    }
                    t_pol_event.energy_value[i][j] /= gain_obj.gain_vec_CT[i](j);
                }
            }
        }
        // (2) store energy reconstructed data
        t_pol_event_tree_new->Fill();
    }
    cout << " DONE ]" << endl;

    // write TTree
    pol_event_file_energy->cd();
    t_pol_event_tree_new->Write();

    TTree* t_raw_energy_tree = static_cast<TTree*>(pol_event_file->Get("t_raw_energy"));
    if (t_raw_energy_tree != NULL) {
        cout << "found TTree t_raw_energy, then copying it ..." << endl;
        t_raw_energy_tree->CloneTree()->Write();
    }

    cout << "writing meta information ... " << endl;
    // write meta
    TIter fileIter(pol_event_file->GetListOfKeys());
    TKey* key = NULL;
    TNamed* meta = NULL;
    while ((key = static_cast<TKey*>(fileIter.Next())) != NULL) {
        if (string(key->GetClassName()) != "TNamed") continue;
        if (string(key->GetName()) == "m_energy_unit") {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->SetTitle("keV");
            meta->Write();
        } else if (string(key->GetName()) == "m_level_num") {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->SetTitle("3");
            meta->Write();
        } else {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        }
    }

    // close output file
    pol_event_file_energy->Close();
    delete pol_event_file_energy;
    pol_event_file_energy = NULL;

    cout << "[ DONE ]" << endl;

    return 0;
}
