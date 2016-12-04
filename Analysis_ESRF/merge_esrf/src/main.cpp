#include <iostream>
#include "RootInc.hpp"
#include "EventIterator.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <decoded_data.root> <event_data.root>" << endl;
        return 2;
    }

    string decoded_data_fn = argv[1];
    string event_data_fn = argv[2];

    EventIterator eventIter;
    if (!eventIter.open(decoded_data_fn.c_str())) {
        cout << "root file open failed." << endl;
        return 1;
    }

    eventIter.print_file_info();

    struct {
        Int_t    type;
        Double_t ct_time_second;
        Bool_t   trig_accepted[25];
        Bool_t   time_aligned[25];
        Int_t    pkt_count;
        Int_t    lost_count;
        Bool_t   trigger_bit[25][64];
        Int_t    trigger_n;
        Int_t    multiplicity[25];
        Float_t  energy_value[25][64];
        UShort_t channel_status[25][64];
        Int_t    compress[25];
        Float_t  common_noise[25];
        Double_t fe_time_second[25];
    } t_event;

    TFile* t_file_merged_out = new TFile(event_data_fn.c_str(), "recreate");
    t_file_merged_out->cd();
    TTree* t_event_tree = new TTree("t_event", "event data only after merged");
    t_event_tree->Branch("type",                &t_event.type,                 "type/I"                      );
    t_event_tree->Branch("ct_time_second",      &t_event.ct_time_second,       "ct_time_second/D"            );
    t_event_tree->Branch("trig_accepted",        t_event.trig_accepted,        "trig_accepted[25]/O"         );
    t_event_tree->Branch("time_aligned",         t_event.time_aligned,         "time_aligned[25]/O"          );
    t_event_tree->Branch("pkt_count",           &t_event.pkt_count,            "pkt_count/I"                 );
    t_event_tree->Branch("lost_count",          &t_event.lost_count,           "lost_count/I"                );
    t_event_tree->Branch("trigger_bit",          t_event.trigger_bit,          "trigger_bit[25][64]/O"       );
    t_event_tree->Branch("trigger_n",           &t_event.trigger_n,            "trigger_n/I"                 );
    t_event_tree->Branch("multiplicity",         t_event.multiplicity,         "multiplicity[25]/I"          );
    t_event_tree->Branch("energy_value",         t_event.energy_value,         "energy_value[25][64]/F"      );
    t_event_tree->Branch("channel_status",       t_event.channel_status,       "channel_status[25][64]/s"    );
    t_event_tree->Branch("compress",             t_event.compress,             "compress[25]/I"              );
    t_event_tree->Branch("common_noise",         t_event.common_noise,         "common_noise[25]/F"          );
    t_event_tree->Branch("fe_time_second",       t_event.fe_time_second,       "fe_time_second[25]/D"        );

    eventIter.phy_trigger_set_start();
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "converting ... " << endl;
    cout << "[ " << flush;
    while (eventIter.phy_trigger_next_event()) {
        cur_percent = static_cast<int>(100 * eventIter.phy_trigger_get_cur_entry() / eventIter.phy_trigger_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        if (eventIter.t_trigger.is_bad > 0 || eventIter.t_trigger.lost_count > 0) {
            continue;
        }
        t_event.type = eventIter.t_trigger.type;
        t_event.ct_time_second = eventIter.t_trigger.time_second;
        t_event.pkt_count = eventIter.t_trigger.pkt_count;
        t_event.lost_count = eventIter.t_trigger.lost_count;
        t_event.trigger_n = eventIter.t_trigger.trigger_n;
        for (int i = 0; i < 25; i++) {
            t_event.trig_accepted[i] = eventIter.t_trigger.trig_accepted[i];
            t_event.time_aligned[i] = false;
            t_event.multiplicity[i] = -1;
            t_event.compress[i] = -1;
            t_event.common_noise[i] = -1;
            t_event.fe_time_second[i] = -1;
            for (int j = 0; j < 64; j++) {
                t_event.trigger_bit[i][j] = false;
                t_event.energy_value[i][j] = -1;
                t_event.channel_status[i][j] = 0;
            }
        }
        while (eventIter.phy_modules_next_packet()) {
            int idx = eventIter.t_modules.ct_num - 1;
            t_event.time_aligned[idx] = true;
            t_event.multiplicity[idx] = eventIter.t_modules.multiplicity;
            t_event.compress[idx] = eventIter.t_modules.compress;
            t_event.common_noise[idx] = eventIter.t_modules.common_noise;
            t_event.fe_time_second[idx] = eventIter.t_modules.time_second;
            for (int j = 0; j < 64; j++) {
                t_event.trigger_bit[idx][j] = eventIter.t_modules.trigger_bit[j];
                if (eventIter.t_modules.energy_adc[j] > 4095) {
                    t_event.channel_status[idx][j] = 1;
                    t_event.energy_value[idx][j] = t_event.common_noise[idx];
                } else {
                    t_event.channel_status[idx][j] = 0;
                    t_event.energy_value[idx][j] = eventIter.t_modules.energy_adc[j];
                }
            }
        }
        t_event_tree->Fill();
    }
    cout << " DONE ]" << endl;

    eventIter.close();

    t_event_tree->Write();

    // write meta
    TNamed("m_energy_unit", "ADC").Write();
    TNamed("m_level_num", "0");

    t_file_merged_out->Close();

    return 0;
}
