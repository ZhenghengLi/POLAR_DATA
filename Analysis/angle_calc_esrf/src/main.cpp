#include <iostream>
#include <algorithm>
#include "RootInc.hpp"

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

    for (Long64_t q = 0; q < t_beam_event_tree->GetEntries(); q++) {
        t_beam_event_tree->GetEntry(q);
        if (q % 10000 == 0) {
            cout << q << endl;
        }
        for (int i = 0; i < 25; i++) {
            if (t_beam_event.time_aligned[i]) {

            }
        }
    }

    t_file_event_data_dep_energy->Close();

    return 0;
}
