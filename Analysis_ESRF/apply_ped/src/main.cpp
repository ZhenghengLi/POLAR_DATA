#include <iostream>
#include <algorithm>
#include "RootInc.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << "<event_data.root> <ped_vec.root> <event_data_sub_ped.root>" << endl;
        return 0;
    }

    string event_data_fn = argv[1];
    string ped_vec_fn = argv[2];
    string event_data_sub_ped_fn = argv[3];

    TFile* t_file_event_data = new TFile(event_data_fn.c_str(), "read");
    if (t_file_event_data->IsZombie()) {
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

    TTree* t_beam_event_tree = static_cast<TTree*>(t_file_event_data->Get("t_beam_event"));
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

    TFile* t_file_ped_vec = new TFile(ped_vec_fn.c_str(), "read");
    if (t_file_ped_vec->IsZombie()) {
        cout << "ped_vec root file open failed. " << endl;
        return 1;
    }
    TVectorF ped_mean_vec[25];
    for (int i = 0; i < 25; i++) {
        TVectorF* tmp_p = static_cast<TVectorF*>(t_file_ped_vec->Get(Form("ped_mean_vec_ct_%02d", i + 1)));
        if (tmp_p == NULL) {
            cout << "read ped_vec failed." << endl;
            return 1;
        }
        ped_mean_vec[i].ResizeTo(64);
        ped_mean_vec[i] = *tmp_p;
    }
    t_file_ped_vec->Close();

    TFile* t_file_event_data_sub_ped = new TFile(event_data_sub_ped_fn.c_str(), "recreate");
    t_file_event_data_sub_ped->cd();
    TTree* t_beam_event_tree_new = t_beam_event_tree->CloneTree(0);
   
    TVectorF energy_adc_vector;
    energy_adc_vector.ResizeTo(64);
    for (Long64_t q = 0; q < t_beam_event_tree->GetEntries(); q++) {
        t_beam_event_tree->GetEntry(q);
        if (q % 10000 == 0) {
            cout << q << endl;
        }
        bool is_overflow = false;
        for (int i =  0; i < 25; i++) {
            if (t_beam_event.time_aligned[i]) {
                copy(&t_beam_event.energy_adc[i * 64], &t_beam_event.energy_adc[i * 64] + 64, energy_adc_vector.GetMatrixArray());
                float cur_common_sum   = 0;
                int   cur_common_n     = 0;
                float cur_common_noise = 0;
                for (int j = 0; j < 64; j++) {
                    if (t_beam_event.compress[i] == 3) {
                        if (energy_adc_vector(j) < 4096 && energy_adc_vector(j) + ped_mean_vec[i](j) > 4000) {
                            is_overflow = true;
                        }
                    } else {
                        if (energy_adc_vector(j) < 4096 && energy_adc_vector(j) == 4095) {
                            is_overflow = true;
                        }
                    }
                    if (is_overflow) {
                        break;
                    }
                    // subtract pedestal 
                    if (t_beam_event.compress[i] != 3) {
                        if (energy_adc_vector(j) < 4096) {
                            energy_adc_vector(j) -= ped_mean_vec[i](j);
                        }
                        if (!t_beam_event.trigger_bit[i * 64 + j]) {
                            cur_common_sum += energy_adc_vector(j);
                            cur_common_n++;
                        }
                    }
                }
                if (is_overflow) {
                    break;
                }
                if (t_beam_event.compress[i] == 0 || t_beam_event.compress[i] == 2) {
                    cur_common_noise = (cur_common_n > 0 ? cur_common_sum / cur_common_n : 0);
                } else if (t_beam_event.compress[i] == 3) {
                    cur_common_noise = t_beam_event.common_noise[i];
                } else {
                    cur_common_noise = 0;
                }
                // subtract common noise
                for (int j = 0; j < 64; j++) {
                    if (energy_adc_vector(j) < 4096) {
                        energy_adc_vector(j) -= cur_common_noise;
                    } else {
                        energy_adc_vector(j) = gRandom->Uniform(-1, 1);
                    }
                }
                copy(energy_adc_vector.GetMatrixArray(), energy_adc_vector.GetMatrixArray() + 64, &t_beam_event.energy_adc[i * 64]);
            }
        }
        if (is_overflow) {
            continue;
        }
        t_beam_event_tree_new->Fill();
    }

    t_file_event_data_sub_ped->cd();
    t_beam_event_tree_new->Write();

    t_file_event_data_sub_ped->Close();

    return 0;
}
