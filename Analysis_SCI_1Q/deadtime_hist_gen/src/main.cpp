#include <iostream>
#include <deque>
#include <algorithm>
#include <numeric>
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
    if (t_pol_event_tree->GetEntries() < 2000) {
        cout << "the TTree t_pol_event is too short." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);
    t_pol_event.deactive_all(t_pol_event_tree);
    t_pol_event.active(t_pol_event_tree, "event_time");
    t_pol_event.active(t_pol_event_tree, "trig_accepted");
    t_pol_event.active(t_pol_event_tree, "time_aligned");
    t_pol_event.active(t_pol_event_tree, "fe_time_wait");
    t_pol_event.active(t_pol_event_tree, "fe_dead_ratio");
    t_pol_event.active(t_pol_event_tree, "compress");
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
    TH1D* module_dead_ratio_hist[25];
    for (int i = 0; i < 25; i++) {
        module_dead_ratio_hist[i] = new TH1D(Form("module_dead_ratio_hist_CT_%02d", i + 1),
                Form("module_dead_ratio_hist_CT_%02d", i + 1), nbins, begin_time, end_time);
        module_dead_ratio_hist[i]->SetDirectory(NULL);
    }
    TH1D* event_dead_ratio_hist = new TH1D("event_dead_ratio_hist", "event_dead_ratio_hist", nbins, begin_time, end_time);


    bool is_first = true;
    deque<int> n_mods_deque;

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

        // calculate module dead ratio hist
        for (int i = 0; i < 25; i++) {
            if (t_pol_event.time_aligned[i]) {
                module_dead_ratio_hist[i]->Fill(t_pol_event.event_time, t_pol_event.fe_time_wait[i] * t_pol_event.fe_dead_ratio[i]);
            }
        }

        //////////////////////////////////////////////////////////
        // calculate event dead ratio hist
        //////////////////////////////////////////////////////////

        if (is_first) {
            is_first = false;
            for (Long64_t t = q; t < q + 1000; t++) {
                t_pol_event_tree->GetEntry(t);
                int n_mods = accumulate(t_pol_event.trig_accepted, t_pol_event.trig_accepted + 25, 0);
                n_mods_deque.push_back(n_mods);
            }
        } else {
            Long64_t t = q + 999;
            if (t < t_pol_event_tree->GetEntries()) {
                t_pol_event_tree->GetEntry(t);
                int n_mods = accumulate(t_pol_event.trig_accepted, t_pol_event.trig_accepted + 25, 0);
                n_mods_deque.pop_front();
                n_mods_deque.push_back(n_mods);
            }
        }
        double f_single = 0;
        for (deque<int>::iterator iter = n_mods_deque.begin(); iter != n_mods_deque.end(); iter++) {
            if (*iter < 2) f_single += 1.0;
        }
        double f_single_ratio = f_single / static_cast<double>(n_mods_deque.size());
        double f_multi_ratio  = 1.0 - f_single_ratio;

        t_pol_event_tree->GetEntry(q);
        double f_modules = 0;
        bool is_compressed = false;
        for (int i = 0; i < 25; i++) {
            if (t_pol_event.trig_accepted[i]) {
                f_modules += 1.0;
                if (t_pol_event.compress[i] == 3) is_compressed = true;
            }
        }
        double deadtime_FEE = (is_compressed ? deadtime_FEE_3 : deadtime_FEE_0);
        double f_FEE_ratio = f_modules / 25.0;
        double f_CT_ratio  = 1.0 - f_FEE_ratio;
        double event_deadtime = f_single_ratio * (f_FEE_ratio * deadtime_FEE + f_CT_ratio * deadtime_CT) + f_multi_ratio * deadtime_CT;

        event_dead_ratio_hist->Fill(event_deadtime);

        //////////////////////////////////////////////////////////

    }
    cout << " DONE ]" << endl;

    // calculate dead ratio
    for (int i = 0; i < 25; i++) {
        module_dead_ratio_hist[i]->Scale(1, "width");
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
        module_dead_ratio_hist[i]->Write();
    }
    event_dead_ratio_hist->Write();
    TNamed("begin_time", Form("%d", begin_time)).Write();
    TNamed("end_time", Form("%d", end_time)).Write();
    TNamed("binsize", Form("%f", options_mgr.binw)).Write();
    deadtime_file->Close();

    pol_event_file->Close();

    cout << "[DONE]" << endl;

    return 0;
}
