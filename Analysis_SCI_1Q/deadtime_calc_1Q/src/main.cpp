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
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);
    t_pol_event.deactive_all(t_pol_event_tree);
    t_pol_event.active(t_pol_event_tree, "event_time");

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

    // read deadtime ratio histogram
    cout << "reading deadtime ratio histogram ..." << endl;
    TH1D* dead_ratio_hist[25];
    TFile* deadtime_ratio_file = new TFile(options_mgr.deadtime_ratio_filename.Data(), "read");
    if (deadtime_ratio_file->IsZombie()) {
        cout << "deadtime ratio file open failed: " << options_mgr.deadtime_ratio_filename.Data() << endl;
        return 1;
    }
    for (int i = 0; i < 25; i++) {
        dead_ratio_hist[i] = static_cast<TH1D*>(deadtime_ratio_file->Get(Form("dead_ratio_hist_CT_%02d", i + 1)));
        if (dead_ratio_hist[i] == NULL) {
            cout << "cannot find dead_ratio_hist" << endl;
            return 1;
        }
    }
    int hist_begin_time = 0;
    int hist_end_time = 0;
    double hist_binsize = 0;
    TNamed* tmp_tnamed;
    tmp_tnamed = static_cast<TNamed*>(deadtime_ratio_file->Get("begin_time"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed begin_time" << endl;
        return 1;
    } else {
        hist_begin_time = TString(tmp_tnamed->GetTitle()).Atoi();
    }
    tmp_tnamed = static_cast<TNamed*>(deadtime_ratio_file->Get("end_time"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed end_time" << endl;
        return 1;
    } else {
        hist_end_time = TString(tmp_tnamed->GetTitle()).Atoi();
    }
    tmp_tnamed = static_cast<TNamed*>(deadtime_ratio_file->Get("binsize"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed binsize" << endl;
        return 1;
    } else {
        hist_binsize = TString(tmp_tnamed->GetTitle()).Atof();
    }
    if (hist_begin_time < begin_time || hist_end_time > end_time) {
        cout << "the deadtime_ratio file does not match" << endl;
        return 1;
    }

    // open dead_ratio_file
    struct {
        Double_t event_time_d;
        Float_t  module_dead_ratio[25];
    } t_dead_ratio;
    TFile* deadtime_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (deadtime_file->IsZombie()) {
        cout << "deadtime_file open failed." << endl;
        return 1;
    }
    TTree* t_dead_ratio_tree = new TTree("t_dead_ratio", "deadtime ratio calculated by a larger bin size");
    t_dead_ratio_tree->Branch("event_time_d",         &t_dead_ratio.event_time_d,        "event_time_d/D"            );
    t_dead_ratio_tree->Branch("module_dead_ratio",     t_dead_ratio.module_dead_ratio,   "module_dead_ratio[25]/F"   );

    int pre_percent = 0;
    int cur_percent = 0;
    cout << "writing deadtime ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);
        t_dead_ratio.event_time_d = t_pol_event.event_time;
        for (int i = 0; i < 25; i++) {
            t_dead_ratio.module_dead_ratio[i] = dead_ratio_hist[i]->Interpolate(t_pol_event.event_time);
        }
        t_dead_ratio_tree->Fill();
    }
    cout << " DONE ]" << endl;

    deadtime_file->cd();
    t_dead_ratio_tree->Write();
    TNamed("binsize", Form("%f", hist_binsize)).Write();
    deadtime_file->Close();

    pol_event_file->Close();

    return 0;
}
