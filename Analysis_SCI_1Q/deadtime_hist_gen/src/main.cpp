#include <iostream>
#include <cmath>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "POLEvent.hpp"

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

    // open pol_event_file
    TFile* pol_event_file = new TFile(options_mgr.pol_event_filename.Data(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << options_mgr.pol_event_filename.Data() << endl;
        return 1;
    }
    // TNamed* m_level_num = static_cast<TNamed*>(pol_event_file->Get("m_level_num"));
    // if (m_level_num == NULL) {
    //     cout << "cannot find TNamed m_level_num." << endl;
    //     return 1;
    // }
    // if (TString(m_level_num->GetTitle()).Atoi() != 0) {
    //     cout << "m_level_num is not 0." << endl;
    //     return 1;
    // }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);
    t_pol_event.deactive_all(t_pol_event_tree);
    t_pol_event.active(t_pol_event_tree, "event_time");
    t_pol_event.active(t_pol_event_tree, "time_aligned");
    t_pol_event.active(t_pol_event_tree, "fe_time_wait");
    t_pol_event.active(t_pol_event_tree, "fe_dead_ratio");
    cout << options_mgr.pol_event_filename.Data() << endl;
    Long64_t begin_entry = 0;
    Long64_t end_entry = t_pol_event_tree->GetEntries();
    t_pol_event_tree->GetEntry(begin_entry);
    int begin_time = static_cast<int>(t_pol_event.event_time);
    t_pol_event_tree->GetEntry(end_entry - 1);
    int end_time = static_cast<int>(t_pol_event.event_time);
    cout << options_mgr.pol_event_filename.Data()
         << " { " << begin_time << "[" << begin_entry << "] => "
         << end_time << "[" << end_entry - 1 << "] }" << endl;

    // prepare histogram
    int nbins = static_cast<int>((end_time - begin_time) / options_mgr.binw);
    TH1D* dead_ratio_hist[25];
    for (int i = 0; i < 25; i++) {
        dead_ratio_hist[i] = new TH1D(Form("dead_ratio_hist_CT_%02d", i + 1),
                Form("dead_ratio_hist_CT_%02d", i + 1), nbins, begin_time, end_time);
        dead_ratio_hist[i]->SetDirectory(NULL);
    }
    // collecting deadtime data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);

        for (int i = 0; i < 25; i++) {
            if (t_pol_event.time_aligned[i]) {
                dead_ratio_hist[i]->Fill(t_pol_event.event_time, t_pol_event.fe_time_wait[i] * t_pol_event.fe_dead_ratio[i]);
            }
        }

    }
    cout << " DONE ]" << endl;

    // calculate dead ratio
    for (int i = 0; i < 25; i++) {
        dead_ratio_hist[i]->Scale(1, "width");
    }

    cout << "write dead ratio histogram ... " << flush;
    // open dead_ratio_file
    TFile* deadtime_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (deadtime_file->IsZombie()) {
        cout << "deadtime_file open failed." << endl;
        return 1;
    }

    deadtime_file->cd();
    for (int i = 0; i < 25; i++) {
        dead_ratio_hist[i]->Write();
    }
    TNamed("begin_time", Form("%d", begin_time)).Write();
    TNamed("end_time", Form("%d", end_time)).Write();
    TNamed("binsize", Form("%f", options_mgr.binw)).Write();
    deadtime_file->Close();

    pol_event_file->Close();

    cout << "[DONE]" << endl;

    return 0;
}
