#include <iostream>
#include <cmath>
#include "RootInc.hpp"
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

    // open pol_event_file
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
    if (TString(m_level_num->GetTitle()).Atoi() != 0) {
        cout << "m_level_num is not 0." << endl;
        return 1;
    }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);
    t_pol_event.deactive_all(t_pol_event_tree);
    Long64_t begin_entry = 0;
    if (options_mgr.begin_met_time > 0) {
        begin_entry = t_pol_event.find_entry(t_pol_event_tree, options_mgr.begin_met_time);
        if (begin_entry < 0) {
            cout << "cannot find begin_entry." << endl;
            return 1;
        }
    }
    Long64_t end_entry = t_pol_event_tree->GetEntries();
    if (options_mgr.end_met_time > 0) {
        end_entry = t_pol_event.find_entry(t_pol_event_tree, options_mgr.end_met_time);
        if (end_entry < 0) {
            cout << "cannot find end_entry." << endl;
            return 1;
        }
    }
    t_pol_event.active(t_pol_event_tree, "event_time");
    t_pol_event.active(t_pol_event_tree, "trigger_bit");
    t_pol_event.active(t_pol_event_tree, "energy_value");
    t_pol_event.active(t_pol_event_tree, "aux_interval");
    t_pol_event.active(t_pol_event_tree, "fe_temp");
    t_pol_event.active(t_pol_event_tree, "is_ped");
    t_pol_event.active(t_pol_event_tree, "time_aligned");
    t_pol_event_tree->GetEntry(begin_entry);
    int begin_time = static_cast<int>(t_pol_event.event_time);
    t_pol_event_tree->GetEntry(end_entry - 1);
    int end_time = static_cast<int>(t_pol_event.event_time);
    cout << options_mgr.pol_event_filename.Data()
         << " { " << begin_time << "[" << begin_entry << "] => "
         << end_time << "[" << end_entry - 1 << "] }" << endl;

    // open ped_data_file
    struct {
        Double_t event_time;
        Bool_t   trigger_bit[64];
        Float_t  ped_adc[64];
        Float_t  fe_temp;
        Float_t  aux_interval;
    } t_ped_data[25];
    TTree* t_ped_data_tree[25];
    TFile* ped_data_file = new TFile(options_mgr.ped_data_filename.Data(), "update");
    if (ped_data_file->IsZombie()) {
        cout << "ped_data_file open failed." << endl;
        return 1;
    }
    for (int i = 0; i < 25; i++) {
        t_ped_data_tree[i] = static_cast<TTree*>(ped_data_file->Get(Form("t_ped_data_ct_%02d", i + 1)));
        if (t_ped_data_tree[i] == NULL) {
            t_ped_data_tree[i] = new TTree(
                    Form("t_ped_data_ct_%02d", i + 1),
                    Form("pedestal data of ct %02d", i + 1));
            t_ped_data_tree[i]->Branch("event_time",     &t_ped_data[i].event_time,      "event_time/D"        );
            t_ped_data_tree[i]->Branch("trigger_bit",     t_ped_data[i].trigger_bit,     "trigger_bit[64]/O"   );
            t_ped_data_tree[i]->Branch("ped_adc",         t_ped_data[i].ped_adc,         "ped_adc[64]/F"       );
            t_ped_data_tree[i]->Branch("fe_temp",        &t_ped_data[i].fe_temp,         "fe_temp/F"           );
            t_ped_data_tree[i]->Branch("aux_interval",   &t_ped_data[i].aux_interval,    "aux_interval/F"      );
        } else {
            t_ped_data_tree[i]->SetBranchAddress("event_time",     &t_ped_data[i].event_time     );
            t_ped_data_tree[i]->SetBranchAddress("trigger_bit",     t_ped_data[i].trigger_bit    );
            t_ped_data_tree[i]->SetBranchAddress("ped_adc",         t_ped_data[i].ped_adc        );
            t_ped_data_tree[i]->SetBranchAddress("fe_temp",        &t_ped_data[i].fe_temp        );
            t_ped_data_tree[i]->SetBranchAddress("aux_interval",   &t_ped_data[i].aux_interval   );
        }
    }

    // collecting pedestal event data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "colecting pedestal event data ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = begin_entry; q < end_entry; q++) {
        cur_percent = static_cast<int>(q * 100.0 / (end_entry - begin_entry));
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);
        if (!t_pol_event.is_ped) continue;
        for (int i = 0; i < 25; i++) {
            if (t_pol_event.time_aligned[i]) {
                t_ped_data[i].event_time = t_pol_event.event_time;
                if (i == 9) {
                    double temp_sum = 0;
                    for (int k = 0; k < 25; k++) {
                        if (k == 9) continue;
                        temp_sum += t_pol_event.fe_temp[k];
                    }
                    t_ped_data[i].fe_temp = round(temp_sum / 24.0);
                } else {
                    t_ped_data[i].fe_temp = t_pol_event.fe_temp[i];
                }
                t_ped_data[i].aux_interval = t_pol_event.aux_interval;
                for (int j = 0; j < 64; j++) {
                    t_ped_data[i].trigger_bit[j] = t_pol_event.trigger_bit[i][j];
                    t_ped_data[i].ped_adc[j] = t_pol_event.energy_value[i][j];
                }
                t_ped_data_tree[i]->Fill();
            }
        }
    }
    for (int i = 0; i < 25; i++) {
        t_ped_data_tree[i]->Write("", TObject::kOverwrite);
    }
    cout << " DONE ]" << endl;

    ped_data_file->Close();

    return 0;
}
