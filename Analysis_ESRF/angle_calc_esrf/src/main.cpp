#include <iostream>
#include <queue>
#include <algorithm>
#include "RootInc.hpp"
#include "Na22Info.hpp"
#include "BarPos.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << "<event_data_dep_energy.root> <angle_data.root>" << endl;
        return 0;
    }

    string event_data_dep_energy_fn = argv[1];
    string angle_data_fn = argv[2];

    TFile* t_file_event_data_dep_energy = new TFile(event_data_dep_energy_fn.c_str(), "read");
    if (t_file_event_data_dep_energy->IsZombie()) {
        cout << "root file open failed." << endl;
        return 1;
    }

    struct {
        Int_t    type;
        Bool_t   trig_accepted[25];
        Bool_t   time_aligned[25];
        Int_t    pkt_count;
        Int_t    lost_count;
        Bool_t   trigger_bit[1600];
        Int_t    trigger_n;
        Int_t    multiplicity[25];
        Float_t  energy_adc[1600];
        Int_t    compress[25];
        Float_t  common_noise[25];
        Bool_t   bar_beam[1600];
        Double_t time_second[25];
        Float_t  max_rate[1600];
    } t_beam_event;

    TTree* t_beam_event_tree = static_cast<TTree*>(t_file_event_data_dep_energy->Get("t_beam_event"));
    if (t_beam_event_tree == NULL) {
        cout << "t_beam_event not found." << endl;
        return 1;
    }

    t_beam_event_tree->SetBranchAddress("type",                &t_beam_event.type               );  
    t_beam_event_tree->SetBranchAddress("trig_accepted",        t_beam_event.trig_accepted      );  
    t_beam_event_tree->SetBranchAddress("time_aligned",         t_beam_event.time_aligned       );  
    t_beam_event_tree->SetBranchAddress("pkt_count",           &t_beam_event.pkt_count          );  
    t_beam_event_tree->SetBranchAddress("lost_count",          &t_beam_event.lost_count         );  
    t_beam_event_tree->SetBranchAddress("trigger_bit",          t_beam_event.trigger_bit        );  
    t_beam_event_tree->SetBranchAddress("trigger_n",           &t_beam_event.trigger_n          );  
    t_beam_event_tree->SetBranchAddress("multiplicity",         t_beam_event.multiplicity       );  
    t_beam_event_tree->SetBranchAddress("energy_adc",           t_beam_event.energy_adc         );  
    t_beam_event_tree->SetBranchAddress("compress",             t_beam_event.compress           );  
    t_beam_event_tree->SetBranchAddress("common_noise",         t_beam_event.common_noise       );  
    t_beam_event_tree->SetBranchAddress("bar_beam",             t_beam_event.bar_beam           );  
    t_beam_event_tree->SetBranchAddress("time_second",          t_beam_event.time_second        );  
    t_beam_event_tree->SetBranchAddress("max_rate",             t_beam_event.max_rate           );  

    // open angle data file
    struct {
        Int_t     first_ij[2];
        Int_t     second_ij[2];
        Float_t   rand_angle;
        Float_t   first_dep;
        Float_t   second_dep;
    } t_angle;
    TFile* t_angle_data_file = new TFile(angle_data_fn.c_str(), "recreate");
    if (t_angle_data_file->IsZombie()) {
        cout << "angle data file open failed." << endl;
        return 1;
    }
    TTree* t_angle_tree = new TTree("t_angle", "angle data");
    t_angle_tree->Branch("first_ij",         t_angle.first_ij,         "first_ij[2]/I"     );
    t_angle_tree->Branch("second_ij",        t_angle.second_ij,        "second_ij[2]/I"    );
    t_angle_tree->Branch("rand_angle",      &t_angle.rand_angle,       "rand_angle/F"      );
    t_angle_tree->Branch("first_dep",       &t_angle.first_dep,        "first_dep/F"       );
    t_angle_tree->Branch("second_dep",      &t_angle.second_dep,       "second_dep/F"      );

    // calculate angle
    priority_queue<Bar> bar_queue;
    bool found_not_adjacent = false;
    bool is_bad_event = false;
    Bar first_bar;
    Pos first_pos;
    Bar second_bar;
    Pos second_pos;
    for (Long64_t q = 0; q < t_beam_event_tree->GetEntries(); q++) {
        t_beam_event_tree->GetEntry(q);
        if (q % 10000 == 0) {
            cout << q << endl;
        }
        if (t_beam_event.lost_count > 0) continue;

        // cut barbeam
        bool found_barbeam = false;
        for (int i = 0; i < 1600; i++) {
            if (t_beam_event.bar_beam[i] && t_beam_event.max_rate[i] > 10) {
                found_barbeam = true;
                break;
            }
        }
        if (!found_barbeam) continue;

        while (!bar_queue.empty()) bar_queue.pop();
        found_not_adjacent = false;
        is_bad_event = false;
        for (int i = 0; i < 25; i++) {
            if (!t_beam_event.time_aligned[i]) continue;
            for (int j = 0; j < 64; j++) {
                if (t_beam_event.trigger_bit[i * 64 + j] && t_beam_event.energy_adc[i * 64 + j] < 0) {
                   is_bad_event = true; 
                   break;
                }
                if (t_beam_event.trigger_bit[i * 64 + j]) {
                    bar_queue.push(Bar(t_beam_event.energy_adc[i * 64 + j], i, j));
                }
            }
            if (is_bad_event) break;
        }
        if (is_bad_event) continue;
        if (bar_queue.empty()) continue;
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
            t_angle.first_dep       = t_beam_event.energy_adc[first_pos.i * 64 + first_pos.j];
            t_angle.second_dep      = t_beam_event.energy_adc[second_pos.i * 64 + second_pos.j];
            t_angle_tree->Fill();
        }
    }

    t_angle_data_file->cd();
    t_angle_tree->Write();
    t_angle_data_file->Close();

    t_file_event_data_dep_energy->Close();

    return 0;
}
