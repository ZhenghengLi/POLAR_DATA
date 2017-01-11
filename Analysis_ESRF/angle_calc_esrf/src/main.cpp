#include <iostream>
#include <queue>
#include <algorithm>
#include "RootInc.hpp"
#include "Na22Info.hpp"
#include "BarPos.hpp"
#include "ChannelStatus.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << "<event_data_depenergy.root> <angle_data.root>" << endl;
        return 0;
    }

    string event_data_depenergy_fn = argv[1];
    string angle_data_fn = argv[2];

    TFile* t_event_depenergy_file = new TFile(event_data_depenergy_fn.c_str(), "read");
    if (t_event_depenergy_file->IsZombie()) {
        cout << "root file open failed." << endl;
        return 1;
    }

    struct {
        Int_t    type;
        Int_t    packet_num;
        Double_t ct_time_second;
        Double_t ct_time_wait;
        Float_t  ct_dead_ratio;
        Double_t fe_time_second[25];
        Double_t fe_time_wait[25];
        Float_t  fe_dead_ratio[25];
        Bool_t   trig_accepted[25];
        Bool_t   time_aligned[25];
        Int_t    raw_rate[25];
        Int_t    pkt_count;
        Int_t    lost_count;
        Bool_t   trigger_bit[25][64];
        Int_t    trigger_n;
        Int_t    multiplicity[25];
        Float_t  energy_value[25][64];
        UShort_t channel_status[25][64];
        Int_t    compress[25];
        Float_t  common_noise[25];
    } t_event;

    TTree* t_event_tree = static_cast<TTree*>(t_event_depenergy_file->Get("t_event"));
    if (t_event_tree == NULL) {
        cout << "t_event is not found." << endl;
        return 1;
    }

    t_event_tree->SetBranchAddress("type",                &t_event.type                 );
    t_event_tree->SetBranchAddress("packet_num",          &t_event.packet_num           );
    t_event_tree->SetBranchAddress("ct_time_second",      &t_event.ct_time_second       );
    t_event_tree->SetBranchAddress("ct_time_wait",        &t_event.ct_time_wait         );
    t_event_tree->SetBranchAddress("ct_dead_ratio",       &t_event.ct_dead_ratio        );
    t_event_tree->SetBranchAddress("fe_time_second",       t_event.fe_time_second       );
    t_event_tree->SetBranchAddress("fe_time_wait",         t_event.fe_time_wait         );
    t_event_tree->SetBranchAddress("fe_dead_ratio",        t_event.fe_dead_ratio        );
    t_event_tree->SetBranchAddress("trig_accepted",        t_event.trig_accepted        );
    t_event_tree->SetBranchAddress("time_aligned",         t_event.time_aligned         );
    t_event_tree->SetBranchAddress("raw_rate",             t_event.raw_rate             );
    t_event_tree->SetBranchAddress("pkt_count",           &t_event.pkt_count            );
    t_event_tree->SetBranchAddress("lost_count",          &t_event.lost_count           );
    t_event_tree->SetBranchAddress("trigger_bit",          t_event.trigger_bit          );
    t_event_tree->SetBranchAddress("trigger_n",           &t_event.trigger_n            );
    t_event_tree->SetBranchAddress("multiplicity",         t_event.multiplicity         );
    t_event_tree->SetBranchAddress("energy_value",         t_event.energy_value         );
    t_event_tree->SetBranchAddress("channel_status",       t_event.channel_status       );
    t_event_tree->SetBranchAddress("compress",             t_event.compress             );
    t_event_tree->SetBranchAddress("common_noise",         t_event.common_noise         );

    // open angle data file
    struct {
        Double_t  ct_time_second;
        Int_t     first_ij[2];
        Int_t     second_ij[2];
        Float_t   rand_angle;
        Float_t   first_energy;
        Float_t   second_energy;
        Bool_t    is_valid;
    } t_angle;
    TFile* t_angle_file = new TFile(angle_data_fn.c_str(), "recreate");
    if (t_angle_file->IsZombie()) {
        cout << "angle data file open failed." << endl;
        return 1;
    }
    TTree* t_angle_tree = new TTree("t_angle", "angle data");
    t_angle_tree->Branch("ct_time_second",  &t_angle.ct_time_second,   "ct_time_second/D"  );
    t_angle_tree->Branch("first_ij",         t_angle.first_ij,         "first_ij[2]/I"     );
    t_angle_tree->Branch("second_ij",        t_angle.second_ij,        "second_ij[2]/I"    );
    t_angle_tree->Branch("rand_angle",      &t_angle.rand_angle,       "rand_angle/F"      );
    t_angle_tree->Branch("first_energy",    &t_angle.first_energy,     "first_energy/F"    );
    t_angle_tree->Branch("second_energy",   &t_angle.second_energy,    "second_energy/F"   );
    t_angle_tree->Branch("is_valid",        &t_angle.is_valid,         "is_valid/O"        );

    // calculate angle
    priority_queue<Bar> bar_queue;
    bool found_not_adjacent = false;
    bool is_bad_event = false;
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
            t_angle.first_energy    = t_event.energy_value[first_pos.i][first_pos.j];
            t_angle.second_energy   = t_event.energy_value[second_pos.i][second_pos.j];
            t_angle.is_valid = true;
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
