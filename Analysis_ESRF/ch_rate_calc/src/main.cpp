#include <iostream>
#include "TFile.h"
#include "TTree.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <event_data.root> <rate_data.root>" << endl;
        return 1;
    }
    string event_data_filename = argv[1];
    string rate_data_filename = argv[2];

    TFile* t_file_in = new TFile(event_data_filename.c_str(), "read");
    if (t_file_in->IsZombie()) {
        cout << "root file open failed" << endl;
        return 1;
    }
    TTree* t_event_tree = static_cast<TTree*>(t_file_in->Get("t_event"));
    if (t_event_tree == NULL) {
        cout << "cannot find TTree t_event" << endl;
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

    // set address
    t_event_tree->SetBranchAddress("type",                &t_event.type                    );
    t_event_tree->SetBranchAddress("packet_num",          &t_event.packet_num              );
    t_event_tree->SetBranchAddress("ct_time_second",      &t_event.ct_time_second          );
    t_event_tree->SetBranchAddress("ct_time_wait",        &t_event.ct_time_wait            );
    t_event_tree->SetBranchAddress("ct_dead_ratio",       &t_event.ct_dead_ratio           );
    t_event_tree->SetBranchAddress("fe_time_second",       t_event.fe_time_second          );
    t_event_tree->SetBranchAddress("fe_time_wait",         t_event.fe_time_wait            );
    t_event_tree->SetBranchAddress("fe_dead_ratio",        t_event.fe_dead_ratio           );
    t_event_tree->SetBranchAddress("trig_accepted",        t_event.trig_accepted           );
    t_event_tree->SetBranchAddress("time_aligned",         t_event.time_aligned            );
    t_event_tree->SetBranchAddress("raw_rate",             t_event.raw_rate                );
    t_event_tree->SetBranchAddress("pkt_count",           &t_event.pkt_count               );
    t_event_tree->SetBranchAddress("lost_count",          &t_event.lost_count              );
    t_event_tree->SetBranchAddress("trigger_bit",          t_event.trigger_bit             );
    t_event_tree->SetBranchAddress("trigger_n",           &t_event.trigger_n               );
    t_event_tree->SetBranchAddress("multiplicity",         t_event.multiplicity            );
    t_event_tree->SetBranchAddress("energy_value",         t_event.energy_value            );
    t_event_tree->SetBranchAddress("channel_status",       t_event.channel_status          );
    t_event_tree->SetBranchAddress("compress",             t_event.compress                );
    t_event_tree->SetBranchAddress("common_noise",         t_event.common_noise            );

    // select branch
    t_event_tree->SetBranchStatus("*", false);
    t_event_tree->SetBranchStatus("ct_time_second", true);
    t_event_tree->SetBranchStatus("ct_time_wait", true);

    cout << "reading data ..." << endl;
    for (Long64_t i = 0; i < t_event_tree->GetEntries(); i++) {
        t_event_tree->GetEntry(i);
        cout << t_event.ct_time_second << " " << t_event.ct_time_wait << endl;

    }
    cout << "done." << endl;

    t_file_in->Close();

    return 0;
}
