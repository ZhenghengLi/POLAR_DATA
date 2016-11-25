#include <iostream>
#include "RootInc.hpp"
#include "EventIterator.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << " <time_window.root> <decoded_data.root> <beam_event.root>" << endl;
        return 2;
    }
    
    string time_window_fn = argv[1];
    string decoded_data_fn = argv[2];
    string beam_event_fn = argv[3];

    TFile* t_file_time_window = new TFile(time_window_fn.c_str(), "read");
    TMatrixF begin_time_mat  = *static_cast<TMatrixF*>(t_file_time_window->Get("begin_time_mat"));
    TMatrixF end_time_mat    = *static_cast<TMatrixF*>(t_file_time_window->Get("end_time_mat"));
    TMatrixF max_count_mat   = *static_cast<TMatrixF*>(t_file_time_window->Get("max_count_mat"));
    t_file_time_window->Close();

    EventIterator eventIter;
    if (!eventIter.open(decoded_data_fn.c_str())) {
        cout << "root file open failed." << endl;
        return 1;
    }

    eventIter.print_file_info();

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

    TFile* t_file_merged_out = new TFile(beam_event_fn.c_str(), "recreate");
    t_file_merged_out->cd();
    TTree* t_beam_event_tree = new TTree("t_beam_event", "event data with barbeam");
    t_beam_event_tree->Branch("type",                &t_beam_event.type,                 "type/I"                );
    t_beam_event_tree->Branch("trig_accepted",        t_beam_event.trig_accepted,        "trig_accepted[25]/O"   );
    t_beam_event_tree->Branch("time_aligned",         t_beam_event.time_aligned,         "time_aligned[25]/O"    );
    t_beam_event_tree->Branch("pkt_count",           &t_beam_event.pkt_count,            "pkt_count/I"           );
    t_beam_event_tree->Branch("lost_count",          &t_beam_event.lost_count,           "lost_count/I"          );
    t_beam_event_tree->Branch("trigger_bit",          t_beam_event.trigger_bit,          "trigger_bit[1600]/O"   );
    t_beam_event_tree->Branch("trigger_n",           &t_beam_event.trigger_n,            "trigger_n/I"           );
    t_beam_event_tree->Branch("multiplicity",         t_beam_event.multiplicity,         "multiplicity[25]/I"    );
    t_beam_event_tree->Branch("energy_adc",           t_beam_event.energy_adc,           "energy_adc[1600]/F"    );
    t_beam_event_tree->Branch("compress",             t_beam_event.compress,             "compress[25]/I"        );
    t_beam_event_tree->Branch("common_noise",         t_beam_event.common_noise,         "common_noise[25]/F"    );
    t_beam_event_tree->Branch("bar_beam",             t_beam_event.bar_beam,             "bar_beam[1600]/O"      );
    t_beam_event_tree->Branch("time_second",          t_beam_event.time_second,          "time_second[25]/D"     );
    t_beam_event_tree->Branch("max_rate",             t_beam_event.max_rate,             "max_rate[1600]/F"      );
    
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
        t_beam_event.type = eventIter.t_trigger.type;
        t_beam_event.pkt_count = eventIter.t_trigger.pkt_count;
        t_beam_event.lost_count = eventIter.t_trigger.lost_count;
        t_beam_event.trigger_n = eventIter.t_trigger.trigger_n;
        for (int i = 0; i < 25; i++) {
            t_beam_event.trig_accepted[i] = eventIter.t_trigger.trig_accepted[i];
            t_beam_event.time_aligned[i] = false;
            t_beam_event.multiplicity[i] = 0;
            t_beam_event.compress[i] = -1;
            t_beam_event.common_noise[i] = 0;
            t_beam_event.time_second[i] = -1;
            for (int j = 0; j < 64; j++) {
                t_beam_event.trigger_bit[i * 64 + j] = false;
                t_beam_event.energy_adc[i * 64 + j] = 0;
                t_beam_event.bar_beam[i * 64 + j] = false;
                t_beam_event.max_rate[i * 64 + j] = -1;
            }
        }
        while (eventIter.phy_modules_next_packet()) {
            int idx = eventIter.t_modules.ct_num - 1;
            t_beam_event.time_aligned[idx] = true;
            t_beam_event.multiplicity[idx] = eventIter.t_modules.multiplicity;
            t_beam_event.compress[idx] = eventIter.t_modules.compress;
            t_beam_event.common_noise[idx] = eventIter.t_modules.common_noise;
            t_beam_event.time_second[idx] = eventIter.t_modules.time_second;
            for (int j = 0; j < 64; j++) {
                t_beam_event.trigger_bit[idx * 64 + j] = eventIter.t_modules.trigger_bit[j];
                t_beam_event.energy_adc[idx * 64 + j] = eventIter.t_modules.energy_adc[j];
                if (eventIter.t_modules.time_second >= begin_time_mat(idx, j) && eventIter.t_modules.time_second <= end_time_mat(idx, j)) {
                    t_beam_event.bar_beam[idx * 64 + j] = true;
                    t_beam_event.max_rate[idx * 64 + j] = max_count_mat(idx, j);
                }
            }
        }
        t_beam_event_tree->Fill();
    }
    cout << " DONE ]" << endl;

    eventIter.close();

    t_beam_event_tree->Write();
    t_file_merged_out->Close();

    return 0;
}
