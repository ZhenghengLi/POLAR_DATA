#include <iostream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "Na22Check.hpp"

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
    t_pol_event.active(t_pol_event_tree, "time_aligned");
    t_pol_event.active(t_pol_event_tree, "trigger_bit");
    t_pol_event.active(t_pol_event_tree, "trigger_n");
    t_pol_event.active(t_pol_event_tree, "multiplicity");
    t_pol_event.active(t_pol_event_tree, "energy_value");
    t_pol_event.active(t_pol_event_tree, "channel_status");
    t_pol_event.active(t_pol_event_tree, "aux_interval");
    t_pol_event.active(t_pol_event_tree, "fe_temp");
    t_pol_event.active(t_pol_event_tree, "fe_hv");
    t_pol_event.active(t_pol_event_tree, "is_ped");
    t_pol_event.active(t_pol_event_tree, "pkt_count");
    t_pol_event.active(t_pol_event_tree, "lost_count");

    // open na22_flag_file
    struct {
        Double_t event_time;
        Bool_t   is_valid;
        Bool_t   is_na22;
        Int_t    first_ij[2];
        Int_t    second_ij[2];
        Int_t    source_id;
        Float_t  angle;
        Float_t  distance;
    } t_na22_flag;
    TTree* t_na22_flag_tree;
    TFile* na22_flag_file = new TFile(options_mgr.na22_flag_filename.Data(), "recreate");
    if (na22_flag_file->IsZombie()) {
        cout << "na22_flag_file open failed." << endl;
        return 1;
    }
    t_na22_flag_tree = new TTree("t_na22_flag", "Na22 event flag");
    t_na22_flag_tree->Branch("event_time",     &t_na22_flag.event_time,      "event_time/D"       );
    t_na22_flag_tree->Branch("is_valid",       &t_na22_flag.is_valid,        "is_valid/O"         );
    t_na22_flag_tree->Branch("is_na22",        &t_na22_flag.is_na22,         "is_na22/O"          );
    t_na22_flag_tree->Branch("first_ij",        t_na22_flag.first_ij,        "first_ij[2]/I"      );
    t_na22_flag_tree->Branch("second_ij",       t_na22_flag.second_ij,       "second_ij[2]/I"     );
    t_na22_flag_tree->Branch("source_id",      &t_na22_flag.source_id,       "source_id/I"        );
    t_na22_flag_tree->Branch("angle",          &t_na22_flag.angle,           "angle/F"            );
    t_na22_flag_tree->Branch("distance",       &t_na22_flag.distance,        "distance/F"         );

    // collecting Na22 event flag
    Na22Check na22_checker;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "tagging Na22 event ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);

        t_na22_flag.event_time = t_pol_event.event_time;
        t_na22_flag.is_na22 = na22_checker.check_na22_event(t_pol_event);
        t_na22_flag.is_valid = na22_checker.cur_is_valid;
        t_na22_flag.first_ij[0] = na22_checker.cur_first_ij[0];
        t_na22_flag.first_ij[1] = na22_checker.cur_first_ij[1];
        t_na22_flag.second_ij[0] = na22_checker.cur_second_ij[0];
        t_na22_flag.second_ij[1] = na22_checker.cur_second_ij[1];
        t_na22_flag.source_id = na22_checker.cur_source_id;
        t_na22_flag.angle = na22_checker.cur_angle;
        t_na22_flag.distance = na22_checker.cur_distance;

        t_na22_flag_tree->Fill();

    }
    t_na22_flag_tree->Write();
    cout << " DONE ]" << endl;

    na22_flag_file->Close();

    return 0;
}
