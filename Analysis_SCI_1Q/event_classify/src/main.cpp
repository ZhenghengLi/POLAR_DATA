#include <iostream>
#include <algorithm>
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "EventFilter.hpp"
#include "Na22Check.hpp"

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

    // read pedestal vs temperature
    TVectorF ped_const_vec[25];
    TVectorF ped_slope_vec[25];
    TFile* ped_temp_file = new TFile(options_mgr.ped_temp_filename.Data(), "read");
    if (ped_temp_file->IsZombie()) {
        cout << "ped_temp_file open failed: " << options_mgr.ped_temp_filename.Data() << endl;
        return 1;
    }
    TVectorF* tmp_vec;
    for (int i = 0; i < 25; i++) {
        tmp_vec = static_cast<TVectorF*>(ped_temp_file->Get(Form("ped_const_vec_ct_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("ped_const_vec_ct_%02d", i + 1) << endl;
            return 1;
        } else {
            ped_const_vec[i].ResizeTo(64);
            ped_const_vec[i] = *tmp_vec;
        }
        tmp_vec = static_cast<TVectorF*>(ped_temp_file->Get(Form("ped_slope_vec_ct_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("ped_slope_vec_ct_%02d", i + 1) << endl;
            return 1;
        } else {
            ped_slope_vec[i].ResizeTo(64);
            ped_slope_vec[i] = *tmp_vec;
        }
    }

    // open output file
    TFile* output_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (output_file->IsZombie()) {
        cout << "output_file open failed: " << options_mgr.output_filename.Data() << endl;
        return 1;
    }
    struct {
        Long64_t event_id_c;
        Double_t event_time_c;
        UShort_t event_type;
        Bool_t   is_na22;
        Float_t  mod_adc_diff[25];
        Bool_t   time_aligned[25];
    } t_event_type;
    TTree* t_event_type_tree = new TTree("t_event_type", "Event Type");
    t_event_type_tree->Branch("event_id_c",         &t_event_type.event_id_c,       "event_id_c/L"         );
    t_event_type_tree->Branch("event_time_c",       &t_event_type.event_time_c,     "event_time_c/D"       );
    t_event_type_tree->Branch("event_type",         &t_event_type.event_type,       "event_type/s"         );
    t_event_type_tree->Branch("is_na22",            &t_event_type.is_na22,          "is_na22/O"            );
    t_event_type_tree->Branch("mod_adc_diff",        t_event_type.mod_adc_diff,     "mod_adc_diff[25]/F"   );
    t_event_type_tree->Branch("time_aligned",        t_event_type.time_aligned,     "time_aligned[25]/O"   );

    EventFilter event_filter;
    Na22Check   na22_checker;

    int pre_percent = 0;
    int cur_percent = 0;
    cout << "classifying events ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);

        // subtract pedestal
        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) continue;
            if (t_pol_event.compress[i] == 3) continue;
            if (i == 9) {
               double temp_sum = 0;
               for (int k = 0; k < 25; k++) {
                   if (k == 9) continue;
                   temp_sum += t_pol_event.fe_temp[k];
               }
               t_pol_event.fe_temp[9] = temp_sum / 24.0;
            }
            for (int j = 0; j < 64; j++) {
                if (t_pol_event.channel_status[i] == 0) {
                    t_pol_event.energy_value[i][j] -= ped_const_vec[i](j) + ped_slope_vec[i](j) * t_pol_event.fe_temp[i];
                }
            }
        }

        t_event_type.event_id_c   = t_pol_event.event_id;
        t_event_type.event_time_c = t_pol_event.event_time;
        t_event_type.event_type   = event_filter.classify(t_pol_event);

        for (int i = 0; i < 25; i++) {
            t_event_type.time_aligned[i] = event_filter.cur_time_aligned[i];
            t_event_type.mod_adc_diff[i] = event_filter.cur_mod_adc_diff[i];
        }

        t_event_type.is_na22 = false;
        if (t_event_type.event_type < 1 && na22_checker.check_na22_event(t_pol_event)) {
            t_event_type.is_na22 = true;
        }

        t_event_type_tree->Fill();

    }

    pol_event_file->Close();
    delete pol_event_file;
    pol_event_file = NULL;

    cout << " DONE ]" << endl;

    // write TTree
    output_file->cd();
    t_event_type_tree->Write();


    output_file->Close();
    delete output_file;

    return 0;
}
