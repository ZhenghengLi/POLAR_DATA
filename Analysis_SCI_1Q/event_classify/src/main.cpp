#include <iostream>
#include <algorithm>
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "EventFilter.hpp"

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

    // read event data
    TFile* pol_event_file = new TFile(options_mgr.pol_event_filename.Data(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << options_mgr.pol_event_filename.Data() << endl;
        return 1;
    }
    TNamed* m_level_num = static_cast<TNamed*>(pol_event_file->Get("m_level_num"));
    if (m_level_num != NULL) {
        if (TString(m_level_num->GetTitle()).Atoi() != 0) {
            cout << "m_level_num is not 0." << endl;
            return 1;
        }
    }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);

    // open output file
    TFile* output_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (output_file->IsZombie()) {
        cout << "output_file open failed: " << options_mgr.output_filename.Data() << endl;
        return 1;
    }
    struct {
        Long64_t event_id_c;
        Double_t event_time_c;
        UShort_t event_type;
    } t_event_type;
    TTree* t_event_type_tree = new TTree("t_event_type", "Event Type");
    t_event_type_tree->Branch("event_id_c",    &t_event_type.event_id_c,   "event_id_c/L"   );
    t_event_type_tree->Branch("event_time_c",  &t_event_type.event_time_c, "event_time_c/D" );
    t_event_type_tree->Branch("event_type",    &t_event_type.event_type,   "event_type/s"   );

    EventFilter event_filter;


    int pre_percent = 0;
    int cur_percent = 0;
    cout << "classifying events ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);

        t_event_type.event_id_c   = t_pol_event.event_id;
        t_event_type.event_time_c = t_pol_event.event_time;
        t_event_type.event_type   = event_filter.classify(t_pol_event);

        t_event_type_tree->Fill();


    }

    pol_event_file->Close();
    delete pol_event_file;
    pol_event_file = NULL;

    cout << " DONE ]" << endl;

    // write TTree
    output_file->cd();
    t_event_type_tree->Write();


    output_file->Close();
    delete output_file;

    return 0;
}
