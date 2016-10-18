#include <iostream>
#include <algorithm>
#include "RootInc.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << "<event_data_sub_ped.root> <xtalk_mat.root> <event_data_xtalk_corr.root>" << endl;
        return 0;
    }

    string event_data_sub_ped_fn = argv[1];
    string xtalk_mat_fn = argv[2];
    string event_data_xtalk_corr_fn = argv[3];

    TFile* t_file_event_data_sub_ped = new TFile(event_data_sub_ped_fn.c_str(), "read");
    if (t_file_event_data_sub_ped->IsZombie()) {
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

    TTree* t_beam_event_tree = static_cast<TTree*>(t_file_event_data_sub_ped->Get("t_beam_event"));
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

    TFile* t_file_xtalk_mat = new TFile(xtalk_mat_fn.c_str(), "read");
    if (t_file_xtalk_mat->IsZombie()) {
        cout << "xtalk_mat root file open failed. " << endl;
        return 1;
    }
    TMatrixF xtalk_mat_inv[25];
    for (int i = 0; i < 25; i++) {
        TMatrixF* tmp_p = static_cast<TMatrixF*>(t_file_xtalk_mat->Get(Form("xtalk_mat_inv_ct_%02d", i + 1)));
        if (tmp_p == NULL) {
            cout << "read xtalk_mat failed." << endl;
            return 1;
        }
        xtalk_mat_inv[i].ResizeTo(64, 64);
        xtalk_mat_inv[i] = *tmp_p;
    }
    t_file_xtalk_mat->Close();

    TFile* t_file_event_data_xtalk_corr = new TFile(event_data_xtalk_corr_fn.c_str(), "recreate");
    t_file_event_data_xtalk_corr->cd();
    TTree* t_beam_event_tree_new = t_beam_event_tree->CloneTree(0);
   
    TVectorF energy_adc_vector;
    energy_adc_vector.ResizeTo(64);
    for (Long64_t q = 0; q < t_beam_event_tree->GetEntries(); q++) {
        t_beam_event_tree->GetEntry(q);
        if (q % 10000 == 0) {
            cout << q << endl;
        }
        for (int i =  0; i < 25; i++) {
            if (t_beam_event.time_aligned[i]) {
                copy(&t_beam_event.energy_adc[i * 64], &t_beam_event.energy_adc[i * 64] + 64, energy_adc_vector.GetMatrixArray());
                energy_adc_vector = xtalk_mat_inv[i] * energy_adc_vector;
                copy(energy_adc_vector.GetMatrixArray(), energy_adc_vector.GetMatrixArray() + 64, &t_beam_event.energy_adc[i * 64]);
            }
        }
        t_beam_event_tree_new->Fill();
    }

    t_file_event_data_xtalk_corr->cd();
    t_beam_event_tree_new->Write();

    t_file_event_data_xtalk_corr->Close();

    return 0;
}
