#include <iostream>
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
    if (TString(m_level_num->GetTitle()).Atoi() != 2) {
        cout << "m_level_num is not 2, need to do pedestal & noise subtraction and crosstalk correction." << endl;
        return 1;
    }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }

    // open na22_flag_file
    TFile* na22_flag_file = new TFile(options_mgr.na22_flag_filename.Data(), "read");
    if (na22_flag_file->IsZombie()) {
        cout << "na22_flag_file open failed" << endl;
        return 1;
    }
    TTree* t_na22_flag_tree = static_cast<TTree*>(na22_flag_file->Get("t_na22_flag"));
    if (t_na22_flag_tree == NULL) {
        cout << "cannot find TTree t_na22_flag" << endl;
        return 1;
    }
    if (t_na22_flag_tree->GetEntries() != t_pol_event_tree->GetEntries()) {
        cout << "na22_flag_file does not match" << endl;
        return 1;
    }
    struct {
        Bool_t is_na22;
        Int_t  first_ij[2];
        Int_t  second_ij[2];
    } t_na22_flag;
    t_na22_flag_tree->SetBranchAddress("is_na22",    &t_na22_flag.is_na22    );
    t_na22_flag_tree->SetBranchAddress("first_ij",    t_na22_flag.first_ij   );
    t_na22_flag_tree->SetBranchAddress("second_ij",   t_na22_flag.second_ij  );

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
    t_pol_event.active(t_pol_event_tree, "time_aligned");
    t_pol_event.active(t_pol_event_tree, "trigger_bit");
    t_pol_event.active(t_pol_event_tree, "trigger_n");
    t_pol_event.active(t_pol_event_tree, "multiplicity");
    t_pol_event.active(t_pol_event_tree, "energy_value");
    t_pol_event.active(t_pol_event_tree, "channel_status");
    t_pol_event.active(t_pol_event_tree, "aux_interval");
    t_pol_event.active(t_pol_event_tree, "fe_temp");
    t_pol_event.active(t_pol_event_tree, "fe_hv");
    t_pol_event.active(t_pol_event_tree, "is_ped");
    t_pol_event.active(t_pol_event_tree, "pkt_count");
    t_pol_event.active(t_pol_event_tree, "lost_count");

    t_pol_event_tree->GetEntry(begin_entry);
    int begin_time = static_cast<int>(t_pol_event.event_time);
    t_pol_event_tree->GetEntry(end_entry - 1);
    int end_time = static_cast<int>(t_pol_event.event_time);
    cout << options_mgr.pol_event_filename.Data()
         << " { " << begin_time << "[" << begin_entry << "] => "
         << end_time << "[" << end_entry - 1 << "] }" << endl;

    // open na22_data_file
    struct {
        Double_t event_time;
        Bool_t   time_aligned[25];
        Bool_t   trigger_bit[25][64];
        Int_t    trigger_n;
        Int_t    multiplicity[25];
        Float_t  energy_value[25][64];
        UShort_t channel_status[25][64];
        Int_t    first_ij[2];
        Int_t    second_ij[2];
        Float_t  fe_temp[25];
        Float_t  fe_hv[25];
        Float_t  aux_interval;
    } t_na22_data;
    TTree* t_na22_data_tree;
    TFile* na22_data_file = new TFile(options_mgr.na22_data_filename.Data(), "update");
    if (na22_data_file->IsZombie()) {
        cout << "na22_data_file open failed." << endl;
        return 1;
    }
    t_na22_data_tree = static_cast<TTree*>(na22_data_file->Get("t_na22_data"));
    if (t_na22_data_tree == NULL) {
        t_na22_data_tree = new TTree("t_na22_data", "Na22 event data");
        t_na22_data_tree->Branch("event_time",     &t_na22_data.event_time,      "event_time/D"              );
        t_na22_data_tree->Branch("time_aligned",    t_na22_data.time_aligned,    "time_aligned[25]/O"        );
        t_na22_data_tree->Branch("trigger_bit",     t_na22_data.trigger_bit,     "trigger_bit[25][64]/O"     );
        t_na22_data_tree->Branch("trigger_n",      &t_na22_data.trigger_n,       "trigger_n/I"               );
        t_na22_data_tree->Branch("multiplicity",    t_na22_data.multiplicity,    "multiplicity[25]/I"        );
        t_na22_data_tree->Branch("energy_value",    t_na22_data.energy_value,    "energy_value[25][64]/F"    );
        t_na22_data_tree->Branch("channel_status",  t_na22_data.channel_status,  "channel_status[25][64]/s"  );
        t_na22_data_tree->Branch("first_ij",        t_na22_data.first_ij,        "first_ij[2]/I"             );
        t_na22_data_tree->Branch("second_ij",       t_na22_data.second_ij,       "second_ij[2]/I"            );
        t_na22_data_tree->Branch("fe_temp",         t_na22_data.fe_temp,         "fe_temp[25]/F"             );
        t_na22_data_tree->Branch("fe_hv",           t_na22_data.fe_hv,           "fe_hv[25]/F"               );
        t_na22_data_tree->Branch("aux_interval",   &t_na22_data.aux_interval,    "aux_interval/F"            );
    } else {
        t_na22_data_tree->SetBranchAddress("event_time",     &t_na22_data.event_time      );
        t_na22_data_tree->SetBranchAddress("time_aligned",    t_na22_data.time_aligned    );
        t_na22_data_tree->SetBranchAddress("trigger_bit",     t_na22_data.trigger_bit     );
        t_na22_data_tree->SetBranchAddress("trigger_n",      &t_na22_data.trigger_n       );
        t_na22_data_tree->SetBranchAddress("multiplicity",    t_na22_data.multiplicity    );
        t_na22_data_tree->SetBranchAddress("energy_value",    t_na22_data.energy_value    );
        t_na22_data_tree->SetBranchAddress("channel_status",  t_na22_data.channel_status  );
        t_na22_data_tree->SetBranchAddress("first_ij",        t_na22_data.first_ij        );
        t_na22_data_tree->SetBranchAddress("second_ij",       t_na22_data.second_ij       );
        t_na22_data_tree->SetBranchAddress("fe_temp",         t_na22_data.fe_temp         );
        t_na22_data_tree->SetBranchAddress("fe_hv",           t_na22_data.fe_hv           );
        t_na22_data_tree->SetBranchAddress("aux_interval",   &t_na22_data.aux_interval    );
    }

    // collecting Na22 event data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "selecting Na22 event data ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = begin_entry; q < end_entry; q++) {
        cur_percent = static_cast<int>(q * 100.0 / (end_entry - begin_entry));
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);
        t_na22_flag_tree->GetEntry(q);

        if (t_pol_event.is_ped) continue;
        if (t_pol_event.pkt_count > 25) continue;
        if (t_pol_event.lost_count > 0) continue;
        // check Na22 event
        if (!t_na22_flag.is_na22) continue;
        // save Na22 event
        t_na22_data.event_time = t_pol_event.event_time;
        t_na22_data.trigger_n  = t_pol_event.trigger_n;
        t_na22_data.aux_interval   = t_pol_event.aux_interval;
        for (int i = 0; i < 25; i++) {
            t_na22_data.time_aligned[i] = t_pol_event.time_aligned[i];
            t_na22_data.multiplicity[i] = t_pol_event.multiplicity[i];
            if (i == 9) {
                double temp_sum = 0;
                for (int k = 0; k < 25; k++) {
                    if (k == 9) continue;
                    temp_sum += t_pol_event.fe_temp[k];
                }
                t_na22_data.fe_temp[i] = round(temp_sum / 24.0);
            } else {
                t_na22_data.fe_temp[i] = t_pol_event.fe_temp[i];
            }
            t_na22_data.fe_hv[i]        = t_pol_event.fe_hv[i];
            for (int j = 0; j < 64; j++) {
                t_na22_data.trigger_bit[i][j] = t_pol_event.trigger_bit[i][j];
                t_na22_data.energy_value[i][j] = t_pol_event.energy_value[i][j];
                t_na22_data.channel_status[i][j] = t_pol_event.channel_status[i][j];
            }
        }
        for (int k = 0; k < 2; k++) {
            t_na22_data.first_ij[k] = t_na22_flag.first_ij[k];
            t_na22_data.second_ij[k] = t_na22_flag.second_ij[k];
        }
        t_na22_data_tree->Fill();

    }
    t_na22_data_tree->Write("", TObject::kOverwrite);
    cout << " DONE ]" << endl;

    na22_data_file->Close();

    return 0;
}
