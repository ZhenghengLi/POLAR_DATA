#include <iostream>
#include <algorithm>
#include "OptionsManager.hpp"
#include "Pedestal.hpp"
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

    // prepare pedestal
    Pedestal ped_obj;
    if (!ped_obj.read_pedestal(options_mgr.ped_vector_filename.Data())) {
        cout << "pedestal vector read failed: " << options_mgr.ped_vector_filename.Data() << endl;
        return 1;
    }
    ped_obj.gen_pedestal();

    // read event data
    TFile* pol_event_file = new TFile(options_mgr.pol_event_filename.Data(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << options_mgr.pol_event_filename.Data() << endl;
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
    TFile* pol_event_file_subped = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (pol_event_file_subped->IsZombie()) {
        cout << "pol_event_file_subped open failed: " << options_mgr.output_filename.Data() << endl;
        return 1;
    }
    TTree* t_pol_event_tree_new = t_pol_event_tree->CloneTree(0);

    TVectorF energy_value_vector;
    energy_value_vector.ResizeTo(64);

    // do pedestal and common noise subtraction
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "subtracting pedestal and common noise ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);

        // (1) store overflow flag
        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) continue;
            if (t_pol_event.compress[i] < 3) {
                for (int j = 0; j < 64; j++) {
                    if (t_pol_event.energy_value[i][j] > 4094.0) {
                        t_pol_event.channel_status[i][j] += POLEvent::ADC_OVERFLOW;
                    }
                }
            } else {
                for (int j = 0; j < 64; j++) {
                    if (t_pol_event.energy_value[i][j] + ped_obj.ped_vec_CT[i](j) > 4000.0) {
                        t_pol_event.channel_status[i][j] += POLEvent::ADC_OVERFLOW;
                    }
                }
            }
        }

        // (2) subtract pedestal
        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) continue;
            if (t_pol_event.compress[i] == 3) continue;
            for (int j = 0; j < 64; j++) {
                if (!(t_pol_event.channel_status[i][j] & POLEvent::ADC_NOT_READOUT)) {
                    t_pol_event.energy_value[i][j] -= ped_obj.ped_vec_CT[i](j);
                }
            }
        }

        // (3) subtract common noise
        float cur_common_sum   = 0;
        int   cur_common_n     = 0;
        float cur_common_noise = 0;
        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) continue;
            if (t_pol_event.compress[i] == 3) {
                cur_common_noise = t_pol_event.common_noise[i] * 2.0;
            } else if (t_pol_event.compress[i] == 1) {
                cur_common_noise = 0.0;
            } else {
                cur_common_sum = 0.0;
                cur_common_n   = 0;
                for (int j = 0; j < 64; j++) {
                    if (!t_pol_event.trigger_bit[i][j]) {
                        cur_common_sum += t_pol_event.energy_value[i][j];
                        cur_common_n   += 1;
                    }
                }
                cur_common_noise = (cur_common_n > 0 ? cur_common_sum / cur_common_n : 0.0);
            }
            if (t_pol_event.compress[i] != 1) {
                t_pol_event.common_noise[i] = cur_common_noise;
            }
            for (int j = 0; j < 64; j++) {
                if (t_pol_event.channel_status[i][j] & POLEvent::ADC_NOT_READOUT) {
                    t_pol_event.energy_value[i][j] -= cur_common_noise / 2.0;
                } else {
                    t_pol_event.energy_value[i][j] -= cur_common_noise;
                }
            }
        }

        // (4) store pedestal and common noise subtracted data
        t_pol_event_tree_new->Fill();
    }
    cout << " DONE ]" << endl;

    // write TTree
    pol_event_file_subped->cd();
    t_pol_event_tree_new->Write();

    cout << "writing meta information ... " << flush;
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

    cout << " DONE ]" << endl;

    return 0;
}
