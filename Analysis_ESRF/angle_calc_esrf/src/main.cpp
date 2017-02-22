#include <iostream>
#include <queue>
#include <algorithm>
#include "RootInc.hpp"
#include "Na22Info.hpp"
#include "BarPos.hpp"
#include "ChannelStatus.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 6) {
        cout << "USAGE: " << argv[0] << "<event_data_depenergy.root> <angle_data.root> <deadtime.root> <weight.root> <current.root>" << endl;
        return 0;
    }

    string event_data_depenergy_fn = argv[1];
    string angle_data_fn = argv[2];
    string deadtime_fn = argv[3];
    string weight_fn = argv[4];
    string current_fn = argv[5];

    TFile* t_event_depenergy_file = new TFile(event_data_depenergy_fn.c_str(), "read");
    if (t_event_depenergy_file->IsZombie()) {
        cout << "root file open failed." << endl;
        return 1;
    }

    struct {
        Double_t ct_time_second;
        Float_t  fe_dead_ratio[25];
        Bool_t   time_aligned[25];
        Int_t    lost_count;
        Bool_t   trigger_bit[25][64];
        Float_t  energy_value[25][64];
        UShort_t channel_status[25][64];
        // from friends
        Float_t  module_dead_ratio[25];
        Float_t  ch_weight[25][64];
        Bool_t   weight_is_bad;
        Double_t current;
    } t_event;

    TTree* t_event_tree = static_cast<TTree*>(t_event_depenergy_file->Get("t_event"));
    if (t_event_tree == NULL) {
        cout << "t_event is not found." << endl;
        return 1;
    }

    // read deadtime
    TFile* deadtime_file = new TFile(deadtime_fn.c_str(), "read");
    if (deadtime_file->IsZombie()) {
        cout << "deadtime file open failed." << endl;
        return 1;
    }
    TTree* t_dead_ratio_tree = static_cast<TTree*>(deadtime_file->Get("t_dead_ratio"));
    if (t_dead_ratio_tree == NULL) {
        cout << "cannot find TTree t_dead_ratio." << endl;
        return 1;
    } else {
        if (t_event_tree->GetEntries() == t_dead_ratio_tree->GetEntries()) {
            t_event_tree->AddFriend(t_dead_ratio_tree);
        } else {
            cout << "number of entries is different between t_event and t_dead_ratio." << endl;
            return 1;
        }
    }

    // read weight
    TFile* weight_file = new TFile(weight_fn.c_str(), "read");
    if (weight_file->IsZombie()) {
        cout << "weight_file open failed." << endl;
        return 1;
    }
    TTree* t_weight_tree = static_cast<TTree*>(weight_file->Get("t_weight"));
    if (t_weight_tree == NULL) {
        cout << "cannot find TTree t_weight." << endl;
    } else {
        if (t_event_tree->GetEntries() == t_weight_tree->GetEntries()) {
            t_event_tree->AddFriend(t_weight_tree);
        } else {
            cout << "number of entries is different between t_event and t_weight." << endl;
            return 1;
        }
    }

    // read current
    TFile* current_file = new TFile(current_fn.c_str(), "read");
    if (current_file->IsZombie()) {
        cout << "current_file open failed." << endl;
        return 1;
    }
    TTree* t_beam_intensity_tree = static_cast<TTree*>(current_file->Get("t_beam_intensity"));
    if (t_beam_intensity_tree == NULL) {
        cout << "cannot find TTree t_beam_intensity." << endl;
        return 1;
    } else {
        if (t_event_tree->GetEntries() == t_beam_intensity_tree->GetEntries()) {
            t_event_tree->AddFriend(t_beam_intensity_tree);
        } else {
            cout << "number of entries is different between t_event and t_beam_intensity." << endl;
            return 1;
        }
    }

    t_event_tree->SetBranchAddress("ct_time_second",      &t_event.ct_time_second       );
    t_event_tree->SetBranchAddress("fe_dead_ratio",        t_event.fe_dead_ratio        );
    t_event_tree->SetBranchAddress("time_aligned",         t_event.time_aligned         );
    t_event_tree->SetBranchAddress("lost_count",          &t_event.lost_count           );
    t_event_tree->SetBranchAddress("trigger_bit",          t_event.trigger_bit          );
    t_event_tree->SetBranchAddress("energy_value",         t_event.energy_value         );
    t_event_tree->SetBranchAddress("channel_status",       t_event.channel_status       );
    t_event_tree->SetBranchAddress("module_dead_ratio",    t_event.module_dead_ratio    );
    t_event_tree->SetBranchAddress("ch_weight",            t_event.ch_weight            );
    t_event_tree->SetBranchAddress("weight_is_bad",       &t_event.weight_is_bad        );
    t_event_tree->SetBranchAddress("current",             &t_event.current              );

    t_event_tree->SetBranchStatus("*", false);
    t_event_tree->SetBranchStatus("ct_time_second", true);
    t_event_tree->SetBranchStatus("fe_dead_ratio", true);
    t_event_tree->SetBranchStatus("time_aligned", true);
    t_event_tree->SetBranchStatus("lost_count", true);
    t_event_tree->SetBranchStatus("trigger_bit", true);
    t_event_tree->SetBranchStatus("energy_value", true);
    t_event_tree->SetBranchStatus("channel_status", true);
    t_event_tree->SetBranchStatus("module_dead_ratio", true);
    t_event_tree->SetBranchStatus("ch_weight", true);
    t_event_tree->SetBranchStatus("weight_is_bad", true);
    t_event_tree->SetBranchStatus("current", true);

    // open angle data file
    struct {
        Double_t  ct_time_second;
        Int_t     first_ij[2];
        Int_t     second_ij[2];
        Float_t   rand_angle;
        Float_t   rand_distance;
        Float_t   first_energy;
        Float_t   second_energy;
        Bool_t    is_valid;
        Float_t   deadtime_weight;
        Float_t   efficiency_weight;
        Float_t   current_weight;
    } t_angle;
    TFile* t_angle_file = new TFile(angle_data_fn.c_str(), "recreate");
    if (t_angle_file->IsZombie()) {
        cout << "angle data file open failed." << endl;
        return 1;
    }
    TTree* t_angle_tree = new TTree("t_angle", "angle data");
    t_angle_tree->Branch("ct_time_second",    &t_angle.ct_time_second,     "ct_time_second/D"    );
    t_angle_tree->Branch("first_ij",           t_angle.first_ij,           "first_ij[2]/I"       );
    t_angle_tree->Branch("second_ij",          t_angle.second_ij,          "second_ij[2]/I"      );
    t_angle_tree->Branch("rand_angle",        &t_angle.rand_angle,         "rand_angle/F"        );
    t_angle_tree->Branch("rand_distance",     &t_angle.rand_distance,      "rand_distance/F"     );
    t_angle_tree->Branch("first_energy",      &t_angle.first_energy,       "first_energy/F"      );
    t_angle_tree->Branch("second_energy",     &t_angle.second_energy,      "second_energy/F"     );
    t_angle_tree->Branch("is_valid",          &t_angle.is_valid,           "is_valid/O"          );
    t_angle_tree->Branch("deadtime_weight",   &t_angle.deadtime_weight,    "deadtime_weight/F"   );
    t_angle_tree->Branch("efficiency_weight", &t_angle.efficiency_weight,  "efficiency_weight/F" );
    t_angle_tree->Branch("current_weight",    &t_angle.current_weight,     "current_weight/F"    );

    // calculate angle
    priority_queue<Bar> bar_queue;
    bool is_bad_event = false;
    bool found_not_adjacent = false;
    Bar first_bar;
    Pos first_pos;
    Bar second_bar;
    Pos second_pos;
    for (Long64_t q = 0; q < t_event_tree->GetEntries(); q++) {
        t_event_tree->GetEntry(q);
        if (q % 10000 == 0) {
            cout << q << endl;
        }
        // init entry
        t_angle.ct_time_second  = t_event.ct_time_second;
        for (int i = 0; i < 2; i++) {
            t_angle.first_ij[i] = -1;
            t_angle.second_ij[i] = -1;
        }
        t_angle.rand_angle = -1;
        t_angle.first_energy = -1;
        t_angle.second_energy = -1;
        t_angle.is_valid = false;
        t_angle.deadtime_weight = 0;
        t_angle.efficiency_weight = 0;
        t_angle.current_weight = 0;

        // skip event with lost packets
        if (t_event.lost_count > 0) {
            t_angle_tree->Fill();
            continue;
        }

        // find the first two bars
        while (!bar_queue.empty()) bar_queue.pop();
        found_not_adjacent = false;
        is_bad_event = false;
        for (int i = 0; i < 25; i++) {
            if (!t_event.time_aligned[i]) continue;
            if (i == 1 || i == 7 || i == 8 || i == 3) {
                is_bad_event = true;
                break;
            }
            for (int j = 0; j < 64; j++) {
                if (t_event.trigger_bit[i][j] && t_event.channel_status[i][j] > 0) {
                   is_bad_event = true;
                   break;
                }
                if (t_event.trigger_bit[i][j]) {
                    bar_queue.push(Bar(t_event.energy_value[i][j], i, j));
                }
            }
            if (is_bad_event) break;
        }
        if (is_bad_event) {
            t_angle_tree->Fill();
            continue;
        }
        if (bar_queue.empty()) {
            t_angle_tree->Fill();
            continue;
        }
        first_bar = bar_queue.top();
        bar_queue.pop();
        first_pos.randomize(first_bar.i, first_bar.j);

        while (!bar_queue.empty()) {
            second_bar = bar_queue.top();
            bar_queue.pop();
            second_pos.randomize(second_bar.i, second_bar.j);
            if (!first_pos.is_adjacent_to(second_pos)) {
                found_not_adjacent = true;
                break;
            }
        }
        if (found_not_adjacent) {
            t_angle.first_ij[0]     = first_pos.i;
            t_angle.first_ij[1]     = first_pos.j;
            t_angle.second_ij[0]    = second_pos.i;
            t_angle.second_ij[1]    = second_pos.j;
            t_angle.rand_angle      = first_pos.angle_to(second_pos);
            t_angle.rand_distance   = first_pos.distance_to(second_pos);
            t_angle.first_energy    = t_event.energy_value[first_pos.i][first_pos.j];
            t_angle.second_energy   = t_event.energy_value[second_pos.i][second_pos.j];
            t_angle.is_valid = true;
            if (first_pos.i == second_pos.i) {
                t_angle.deadtime_weight = 1 / (1 - t_event.module_dead_ratio[first_pos.i]);
            } else {
                t_angle.deadtime_weight = (1 / (1 - t_event.module_dead_ratio[first_pos.i])) * (1 / (1 - t_event.module_dead_ratio[second_pos.i]));
            }
            if (t_angle.deadtime_weight < 0) t_angle.deadtime_weight = 0;
            t_angle.efficiency_weight = t_event.ch_weight[first_pos.i][first_pos.j] * t_event.ch_weight[second_pos.i][second_pos.j];
            if (t_event.weight_is_bad) t_angle.efficiency_weight = 0;
            if (t_event.current > 0) {
                t_angle.current_weight = 1 / t_event.current;
            } else {
                t_angle.current_weight = 0;
            }
            t_angle_tree->Fill();
        } else {
            t_angle_tree->Fill();
        }
    }

    t_angle_file->cd();
    t_angle_tree->Write();
    t_angle_file->Close();

    t_event_depenergy_file->Close();

    return 0;
}
