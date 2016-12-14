#include <iostream>
#include <algorithm>
#include "RootInc.hpp"
#include "ChannelStatus.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << "<event_data_subped.root> <xtalk_mat.root> <event_data_xtalkcorr.root>" << endl;
        return 0;
    }

    string event_data_subped_fn = argv[1];
    string xtalk_mat_fn = argv[2];
    string event_data_xtalkcorr_fn = argv[3];

    TFile* t_event_file = new TFile(event_data_subped_fn.c_str(), "read");
    if (t_event_file->IsZombie()) {
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

    TTree* t_event_tree = static_cast<TTree*>(t_event_file->Get("t_event"));
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

    TFile* t_event_xtalkcorr_file = new TFile(event_data_xtalkcorr_fn.c_str(), "recreate");
    if (t_event_xtalkcorr_file->IsZombie()) {
        cout << "output root file open failed." << endl;
        return 1;
    }
    t_event_xtalkcorr_file->cd();
    TTree* t_event_tree_new = t_event_tree->CloneTree(0);

    TVectorF energy_value_vector;
    energy_value_vector.ResizeTo(64);
    for (Long64_t q = 0; q < t_event_tree->GetEntries(); q++) {
        t_event_tree->GetEntry(q);
        if (q % 10000 == 0) {
            cout << q << endl;
        }
        for (int i =  0; i < 25; i++) {
            if (t_event.time_aligned[i]) {
                copy(t_event.energy_value[i], t_event.energy_value[i] + 64, energy_value_vector.GetMatrixArray());
                energy_value_vector = xtalk_mat_inv[i] * energy_value_vector;
                copy(energy_value_vector.GetMatrixArray(), energy_value_vector.GetMatrixArray() + 64, t_event.energy_value[i]);
            }
        }
        t_event_tree_new->Fill();
    }

    t_event_xtalkcorr_file->cd();
    t_event_tree_new->Write();

    // write meta
    TIter fileIter(t_event_file->GetListOfKeys());
    TKey* key = NULL;
    TNamed* meta = NULL;
    while ((key = static_cast<TKey*>(fileIter.Next())) != NULL) {
        if (string(key->GetClassName()) != "TNamed") continue;
        if (string(key->GetName()) == "m_energy_unit") {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        } else if (string(key->GetName()) == "m_level_num") {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->SetTitle("2");
            meta->Write();
        } else {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        }
    }

    t_event_xtalkcorr_file->Close();

    return 0;
}
