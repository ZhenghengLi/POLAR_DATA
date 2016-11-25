#include <iostream>
#include <algorithm>
#include "RootInc.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << "<event_data_xtalk_corr.root> <adc_per_kev.root> <event_data_dep_energy.root>" << endl;
        return 0;
    }

    string event_data_xtalk_corr_fn = argv[1];
    string adc_per_kev_fn = argv[2];
    string event_data_dep_energy_fn = argv[3];

    TFile* t_file_event_data_xtalk_corr = new TFile(event_data_xtalk_corr_fn.c_str(), "read");
    if (t_file_event_data_xtalk_corr->IsZombie()) {
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

    TTree* t_beam_event_tree = static_cast<TTree*>(t_file_event_data_xtalk_corr->Get("t_beam_event"));
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

    TFile* t_file_adc_per_kev = new TFile(adc_per_kev_fn.c_str(), "read");
    if (t_file_adc_per_kev->IsZombie()) {
        cout << "adc_per_kev root file open failed. " << endl;
        return 1;
    }
    TVectorF adc_per_kev[25];
    for (int i = 0; i < 25; i++) {
        TVectorF* tmp_p = static_cast<TVectorF*>(t_file_adc_per_kev->Get(Form("adc_per_kev_vec_ct_%02d", i + 1)));
        if (tmp_p == NULL) {
            cout << "read adc_per_kev failed." << endl;
            return 1;
        }
        adc_per_kev[i].ResizeTo(64);
        adc_per_kev[i] = *tmp_p;
    }
    t_file_adc_per_kev->Close();

    TFile* t_file_event_data_dep_energy = new TFile(event_data_dep_energy_fn.c_str(), "recreate");
    t_file_event_data_dep_energy->cd();
    TTree* t_beam_event_tree_new = t_beam_event_tree->CloneTree(0);
   
    for (Long64_t q = 0; q < t_beam_event_tree->GetEntries(); q++) {
        t_beam_event_tree->GetEntry(q);
        if (q % 10000 == 0) {
            cout << q << endl;
        }
        bool is_bad_event = false;
        for (int i = 0; i < 25; i++) {
            if (t_beam_event.time_aligned[i]) {
                if (i == 1) {
                    is_bad_event = true;
                    break;
                }
                for (int j = 0; j < 64; j++) {
                    if (adc_per_kev[i](j) < 4.0 && t_beam_event.trigger_bit[i * 64 + j]) {
                        is_bad_event = true;
                        break;
                    }
                    if (adc_per_kev[i](j) < 4.0) {
                        t_beam_event.energy_adc[i * 64 + j] = -1;
                    } else {
                        t_beam_event.energy_adc[i * 64 + j] = t_beam_event.energy_adc[i * 64 + j] / adc_per_kev[i](j);
                    }
                }
            }
            if (is_bad_event) {
                break;
            }
        }
        if (is_bad_event) {
            continue;
        }
        t_beam_event_tree_new->Fill();
    }

    t_file_event_data_dep_energy->cd();
    t_beam_event_tree_new->Write();

    t_file_event_data_dep_energy->Close();

    return 0;
}
