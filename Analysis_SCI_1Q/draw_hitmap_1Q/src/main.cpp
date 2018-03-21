#include <iostream>
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "CooConv.hpp"
#include "HitmapCanvas.hpp"

using namespace std;

int main(int argc, char** argv) {
    OptionsManager options_mgr;
    if (!options_mgr.parse(argc, argv)) {
        if (options_mgr.get_version_flag()) {
            options_mgr.print_version();
        } else {
            options_mgr.print_help();
        }
        return 2;
    }

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);


    // open pol_event file
    TFile* pol_event_file = new TFile(options_mgr.pol_event_filename.Data(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << options_mgr.pol_event_filename.Data() << endl;
        return 1;
    }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);
    t_pol_event.deactive_all(t_pol_event_tree);
    Long64_t begin_entry = 0;
    if (options_mgr.begin_met_time > 0) {
        begin_entry = t_pol_event.find_entry(t_pol_event_tree, options_mgr.begin_met_time);
        if (begin_entry < 0) {
            cout << "cannot find begin_entry." << endl;
            return 1;
        }
    }
    Long64_t end_entry = t_pol_event_tree->GetEntries();
    if (options_mgr.end_met_time > 0) {
        end_entry = t_pol_event.find_entry(t_pol_event_tree, options_mgr.end_met_time);
        if (end_entry < 0) {
            cout << "cannot find end_entry." << endl;
            return 1;
        }
    }
    t_pol_event.active(t_pol_event_tree, "event_time");
    t_pol_event.active(t_pol_event_tree, "time_aligned");
    t_pol_event.active(t_pol_event_tree, "is_ped");
    t_pol_event.active(t_pol_event_tree, "trigger_bit");
    t_pol_event.active(t_pol_event_tree, "energy_value");
    t_pol_event.active(t_pol_event_tree, "channel_status");
    t_pol_event.active(t_pol_event_tree, "multiplicity");
    t_pol_event_tree->GetEntry(begin_entry);
    int begin_time = static_cast<int>(t_pol_event.event_time);
    t_pol_event_tree->GetEntry(end_entry - 1);
    int end_time = static_cast<int>(t_pol_event.event_time);
    cout << options_mgr.pol_event_filename.Data()
         << " { " << begin_time << "[" << begin_entry << "] => "
         << end_time << "[" << end_entry - 1 << "] }" << endl;

    int ch_count[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ch_count[i][j] = 0;
        }
    }

    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = begin_entry; q < end_entry; q++) {
        cur_percent = static_cast<int>((q - begin_entry) * 100.0 / (end_entry - begin_entry));
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);

        if (t_pol_event.is_ped) continue;
        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) continue;
            for (int j = 0; j < 64; j++) {
                if (t_pol_event.energy_value[i][j] < options_mgr.energy_thr) continue;
                if (t_pol_event.trigger_bit[i][j]) {
                    ch_count[i][j] += 1;
                }
            }
        }

    }
    cout << " DONE ]" << endl;
    pol_event_file->Close();
    delete pol_event_file;
    pol_event_file = NULL;


    // drawing
    gStyle->SetOptStat(0);

    HitmapCanvas hitmap_canvas;
    hitmap_canvas.set_count_map(ch_count);

    hitmap_canvas.draw_count_map();

    rootapp->Run();
    return 0;
}
