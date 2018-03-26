#include <iostream>
#include <algorithm>
#include "OptionsManager.hpp"
#include "Pedestal.hpp"
#include "POLEvent.hpp"

using namespace std;

bool neigh_to_trigger(bool trigger_bit[64], int j) {
    // first neighbour
    if (j - 1 >= 0  && trigger_bit[j - 1]) return true;
    if (j + 1 <= 63 && trigger_bit[j + 1]) return true;
    if (j - 8 >= 0  && trigger_bit[j - 8]) return true;
    if (j + 8 <= 63 && trigger_bit[j + 8]) return true;
    if (j - 7 >= 0  && trigger_bit[j - 7]) return true;
    if (j + 7 <= 63 && trigger_bit[j + 7]) return true;
    if (j - 9 >= 0  && trigger_bit[j - 9]) return true;
    if (j + 9 <= 63 && trigger_bit[j + 9]) return true;
    // second neighbour
    // if (j - 2 >= 0  && trigger_bit[j - 2]) return true;
    // if (j + 2 <= 63 && trigger_bit[j + 2]) return true;
    // if (j - 6 >= 0  && trigger_bit[j - 6]) return true;
    // if (j + 6 <= 63 && trigger_bit[j + 6]) return true;
    // if (j - 10 >= 0  && trigger_bit[j - 10]) return true;
    // if (j + 10 <= 63 && trigger_bit[j + 10]) return true;
    // if (j - 14 >= 0  && trigger_bit[j - 14]) return true;
    // if (j + 14 <= 63 && trigger_bit[j + 14]) return true;
    // if (j - 15 >= 0  && trigger_bit[j - 15]) return true;
    // if (j + 15 <= 63 && trigger_bit[j + 15]) return true;
    // if (j - 16 >= 0  && trigger_bit[j - 16]) return true;
    // if (j + 16 <= 63 && trigger_bit[j + 16]) return true;
    // if (j - 17 >= 0  && trigger_bit[j - 17]) return true;
    // if (j + 17 <= 63 && trigger_bit[j + 17]) return true;
    // if (j - 18 >= 0  && trigger_bit[j - 18]) return true;
    // if (j + 18 <= 63 && trigger_bit[j + 18]) return true;

    return false;
}

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
    if (options_mgr.ped_temp_flag) {
        if (!ped_obj.read_ped_temp_par(options_mgr.ped_temp_par_filename.Data())) {
            cout << "ped_temp_par vector read failed: " << options_mgr.ped_temp_par_filename.Data() << endl;
            return 1;
        }
    } else {
        if (!ped_obj.read_pedestal(options_mgr.ped_vector_filename.Data())) {
            cout << "pedestal vector read failed: " << options_mgr.ped_vector_filename.Data() << endl;
            return 1;
        }
        ped_obj.gen_pedestal();
    }

    // read event data
    TFile* pol_event_file = new TFile(options_mgr.pol_event_filename.Data(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << options_mgr.pol_event_filename.Data() << endl;
        return 1;
    }
    TNamed* m_level_num = static_cast<TNamed*>(pol_event_file->Get("m_level_num"));
    if (m_level_num != NULL) {
        if (TString(m_level_num->GetTitle()).Atoi() != 0) {
            cout << "m_level_num is not 0." << endl;
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
            if (options_mgr.ped_temp_flag) {
                if (i == 9) {
                    double temp_sum = 0;
                    for (int k = 0; k < 25; k++) {
                        if (k == 9) continue;
                        temp_sum += t_pol_event.fe_temp[k];
                    }
                    ped_obj.gen_pedestal(i + 1, round(temp_sum / 24.0));
                } else {
                    ped_obj.gen_pedestal(i + 1, t_pol_event.fe_temp[i]);
                }
            }
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
            for (int j = 0; j < 64; j++) {
                if (t_pol_event.channel_status[i][j] & POLEvent::ADC_NOT_READOUT) {
                    t_pol_event.energy_value[i][j] -= t_pol_event.common_noise[i];
                }
            }
            if (t_pol_event.compress[i] == 3) {
                double sum_non_trigger = t_pol_event.common_noise[i] * 2.0 * 64;
                // correction for common_noise calculated in-orbit
                int neigh_count = 0;
                int trigg_count = 0;
                for (int j = 0; j < 64; j++) {
                    if (t_pol_event.trigger_bit[i][j]) {
                        trigg_count++;
                        continue;
                    }
                    if (t_pol_event.channel_status[i][j] & POLEvent::ADC_NOT_READOUT) continue;
                    if (neigh_to_trigger(t_pol_event.trigger_bit[i], j)) {
                        sum_non_trigger -= t_pol_event.energy_value[i][j];
                        neigh_count++;
                    }
                }
                cur_common_noise = (trigg_count + neigh_count < 64 ? sum_non_trigger / (64 - trigg_count - neigh_count) : 0);

            } else if (t_pol_event.compress[i] == 1) {
                cur_common_noise = 0.0;
            } else {
                cur_common_sum = 0.0;
                cur_common_n   = 0;
                for (int j = 0; j < 64; j++) {
                    if (t_pol_event.trigger_bit[i][j]) continue;
                    if (neigh_to_trigger(t_pol_event.trigger_bit[i], j)) continue;
                    cur_common_sum += t_pol_event.energy_value[i][j];
                    cur_common_n   += 1;
                }
                cur_common_noise = (cur_common_n > 0 ? cur_common_sum / cur_common_n : 0.0);
            }
            if (t_pol_event.compress[i] != 1) {
                t_pol_event.common_noise[i] = cur_common_noise;
            }
            for (int j = 0; j < 64; j++) {
                if (t_pol_event.channel_status[i][j] & POLEvent::ADC_NOT_READOUT) continue;
                t_pol_event.energy_value[i][j] -= cur_common_noise;
            }
        }

        // (4) store pedestal and common noise subtracted data
        t_pol_event_tree_new->Fill();
    }
    cout << " DONE ]" << endl;

    // write TTree
    pol_event_file_subped->cd();
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
