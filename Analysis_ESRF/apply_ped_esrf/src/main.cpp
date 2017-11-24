#include <iostream>
#include <algorithm>
#include "RootInc.hpp"
#include "ChannelStatus.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << "<event_data.root> <ped_vec.root> <event_data_subped.root>" << endl;
        return 0;
    }

    string event_data_fn = argv[1];
    string ped_vec_fn = argv[2];
    string event_data_subped_fn = argv[3];

    TFile* t_event_file = new TFile(event_data_fn.c_str(), "read");
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

    TFile* t_event_subped_file = new TFile(event_data_subped_fn.c_str(), "recreate");
    if (t_event_subped_file->IsZombie()) {
        cout << "output root file open failed." << endl;
        return 1;
    }
    t_event_subped_file->cd();
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
                float cur_common_sum   = 0;
                int   cur_common_n     = 0;
                float cur_common_noise = 0;
                for (int j = 0; j < 64; j++) {
                    if (t_event.compress[i] == 3) {
                        if (!(t_event.channel_status[i][j] & NO_READOUT) && energy_value_vector(j) + ped_mean_vec[i](j) > 4000) {
                            t_event.channel_status[i][j] |= OVER_FLOW;
                        }
                    } else {
                        if (!(t_event.channel_status[i][j] & NO_READOUT) && energy_value_vector(j) == 4095) {
                            t_event.channel_status[i][j] |= OVER_FLOW;
                        }
                    }
                    // subtract pedestal
                    if (t_event.compress[i] != 3) {
                        if (!(t_event.channel_status[i][j] & NO_READOUT)) {
                            energy_value_vector(j) -= ped_mean_vec[i](j);
                        }
                        if (!t_event.trigger_bit[i][j]) {
                            cur_common_sum += energy_value_vector(j);
                            cur_common_n++;
                        }
                    }
                }
                if (t_event.compress[i] == 0 || t_event.compress[i] == 2) {
                    cur_common_noise = (cur_common_n > 0 ? cur_common_sum / cur_common_n : 0);
                    t_event.common_noise[i] = cur_common_noise;
                } else if (t_event.compress[i] == 3) {
                    cur_common_noise = t_event.common_noise[i] * 2.0;
                } else {
                    cur_common_noise = 0;
                }
                t_event.common_noise[i] = cur_common_noise;
                // subtract common noise
                for (int j = 0; j < 64; j++) {
                    energy_value_vector(j) -= cur_common_noise;
                }
                copy(energy_value_vector.GetMatrixArray(), energy_value_vector.GetMatrixArray() + 64, t_event.energy_value[i]);
            }
        }
        t_event_tree_new->Fill();
    }

    t_event_subped_file->cd();
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
            meta->SetTitle("1");
            meta->Write();
        } else {
            meta = static_cast<TNamed*>(key->ReadObj());
            meta->Write();
        }
    }

    t_event_subped_file->Close();

    return 0;
}
