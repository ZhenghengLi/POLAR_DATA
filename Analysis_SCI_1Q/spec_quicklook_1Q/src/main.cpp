#include <iostream>
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "CooConv.hpp"
#include "SpectrumCanvas.hpp"

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

    double vthr_max = 2000;
    double vthr_min = 0;

    if (options_mgr.energy_flag) {
        vthr_max = 150;
        vthr_min = -50;
    }

    // open data file
    TFile* event_file = new TFile(options_mgr.pol_event_filename.Data(), "read");
    if (event_file->IsZombie()) {
        cout << "event_file open failed." << endl;
        return 0;
    }
    TTree* event_tree = NULL;
    event_tree = static_cast<TTree*>(event_file->Get("t_pol_event"));
    if (event_tree == NULL) {
        event_tree = static_cast<TTree*>(event_file->Get("t_event"));
    }
    if (event_tree == NULL) {
        cout << "cannot find TTree t_pol_event or t_event" << endl;
        return 1;
    }

    Bool_t trig_accepted[25];
    Bool_t time_aligned[25];
    Bool_t trigger_bit[25][64];
    Float_t energy_value[25][64];
    event_tree->SetBranchAddress("trig_accepted", trig_accepted);
    event_tree->SetBranchAddress("time_aligned",  time_aligned );
    event_tree->SetBranchAddress("trigger_bit",   trigger_bit  );
    event_tree->SetBranchAddress("energy_value",  energy_value );
    event_tree->SetBranchStatus("*", false);
    event_tree->SetBranchStatus("trig_accepted", true);
    event_tree->SetBranchStatus("time_aligned",  true);
    event_tree->SetBranchStatus("trigger_bit",   true);
    event_tree->SetBranchStatus("energy_value",  true);


    int mod_count[25];
    int bar_count[25][64];
    for (int i = 0; i < 25; i++) {
        mod_count[i] = 0;
        for (int j = 0; j < 64; j++) {
            bar_count[i][j] = 0;
        }
    }
    TH1D* tri_spec[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            tri_spec[i][j] = new TH1D(Form("tri_spec_%02d_%02d", i + 1, j + 1), Form("tri_spec_%02d_%02d", i + 1, j + 1), 200, vthr_min, vthr_max);
            tri_spec[i][j]->SetDirectory(NULL);
            // tri_spec[i][j]->SetLineColor(kRed);
        }
    }

    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        event_tree->GetEntry(q);

        for (int i = 0; i < 25; i++) {
            if (trig_accepted[i]) mod_count[i]++;
            if (!time_aligned[i]) continue;
            for (int j = 0; j < 64; j++) {
                if (trigger_bit[i][j]) {
                    tri_spec[i][j]->Fill(energy_value[i][j]);
                    bar_count[i][j]++;
                }
            }
        }

    }
    cout << " DONE ]" << endl;
    event_file->Close();
    delete event_file;
    event_file = NULL;


    // drawing
    gStyle->SetOptStat(0);

    SpectrumCanvas spec_canvas;
    spec_canvas.set_tri_spec(tri_spec);
    spec_canvas.set_count_map(mod_count, bar_count);

    spec_canvas.draw_count_map();

    rootapp->Run();
    return 0;
}
