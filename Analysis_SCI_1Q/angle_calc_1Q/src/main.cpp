#include <iostream>
#include <fstream>
#include <sstream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "Na22Check.hpp"
#include "BarPos.hpp"
#include "PosConv.hpp"

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

    bool bar_mask[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            bar_mask[i][j] = false;
        }
    }
    // read bar_mask
    if (!options_mgr.bar_mask_filename.IsNull()) {
        char line_buffer[100];
        ifstream infile;
        infile.open(options_mgr.bar_mask_filename.Data());
        if (!infile.is_open()) {
            cout << "bar_mask_file open failed." << endl;
            return 1;
        }
        stringstream ss;
        int ct_num;
        int ch_idx;
        while (true) {
            infile.getline(line_buffer, 100);
            if (infile.eof()) break;
            if (string(line_buffer).find("#") != string::npos) {
                continue;
            } else {
                ss.clear();
                ss.str(line_buffer);
                ss >> ct_num >> ch_idx;
                if (ct_num < 1 || ct_num > 25) {
                    cout << "ct_num out of range" << endl;
                    return 1;
                }
                if (ch_idx < 0 || ch_idx > 63) {
                    cout << "ch_idx out of range" << endl;
                    return 1;
                }
                cout << "kill bar ct_" << ct_num << ", ch_" << ch_idx << endl;
                bar_mask[ct_num - 1][ch_idx] = true;
            }
        }
        infile.close();
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

    // open deadtime_file
    struct {
        Double_t event_time_d;
        Float_t  module_dead_ratio[25];
    } t_dead_ratio;
    TFile* deadtime_file = NULL;
    TTree* t_dead_ratio_tree = NULL;
    if (!options_mgr.no_deadtime) {
        deadtime_file = new TFile(options_mgr.deadtime_filename.Data(), "read");
        if (deadtime_file->IsZombie()) {
            cout << "deadtime_file open failed." << endl;
            return 1;
        }
        t_dead_ratio_tree = static_cast<TTree*>(deadtime_file->Get("t_dead_ratio"));
        if (t_dead_ratio_tree == NULL) {
            cout << "cannot find TTree t_dead_ratio" << endl;
            return 1;
        }
        t_dead_ratio_tree->SetBranchAddress("event_time_d",         &t_dead_ratio.event_time_d        );
        t_dead_ratio_tree->SetBranchAddress("module_dead_ratio",     t_dead_ratio.module_dead_ratio   );
        if (t_dead_ratio_tree->GetEntries() != t_pol_event_tree->GetEntries()) {
            cout << "Entries is different between TTree t_dead_ratio and t_pol_event" << endl;
            return 1;
        } else {
            t_pol_event_tree->AddFriend(t_dead_ratio_tree);
        }
    }

    t_pol_event.deactive_all(t_pol_event_tree);
    t_pol_event.active(t_pol_event_tree, "event_time");
    t_pol_event.active(t_pol_event_tree, "time_aligned");
    t_pol_event.active(t_pol_event_tree, "trigger_bit");
    t_pol_event.active(t_pol_event_tree, "energy_value");
    t_pol_event.active(t_pol_event_tree, "channel_status");
    t_pol_event.active(t_pol_event_tree, "is_ped");
    t_pol_event.active(t_pol_event_tree, "pkt_count");
    t_pol_event.active(t_pol_event_tree, "lost_count");
    t_pol_event.active(t_pol_event_tree, "wgs84_xyz");
    if (!options_mgr.no_deadtime) {
        t_pol_event.active(t_pol_event_tree, "module_dead_ratio");
    }

    // open angle file
    struct {
        Double_t  event_time;
        Int_t     first_ij[2];
        Int_t     second_ij[2];
        Float_t   rand_angle;
        Float_t   rand_distance;
        Float_t   first_energy;
        Float_t   second_energy;
        Bool_t    is_adjacent;
        Bool_t    is_na22;
        Bool_t    is_bad_calib;
        Bool_t    with_badch;
        Float_t   weight;
        Float_t   latitude;
        Float_t   longitude;
    } t_pol_angle;

    TFile* t_pol_angle_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (t_pol_angle_file->IsZombie()) {
        cout << "angle data file open failed." << endl;
        return 1;
    }
    TTree* t_pol_angle_tree = new TTree("t_pol_angle", "angle data");
    t_pol_angle_tree->Branch("event_time",        &t_pol_angle.event_time,         "event_time/D"        );
    t_pol_angle_tree->Branch("first_ij",           t_pol_angle.first_ij,           "first_ij[2]/I"       );
    t_pol_angle_tree->Branch("second_ij",          t_pol_angle.second_ij,          "second_ij[2]/I"      );
    t_pol_angle_tree->Branch("rand_angle",        &t_pol_angle.rand_angle,         "rand_angle/F"        );
    t_pol_angle_tree->Branch("rand_distance",     &t_pol_angle.rand_distance,      "rand_distance/F"     );
    t_pol_angle_tree->Branch("first_energy",      &t_pol_angle.first_energy,       "first_energy/F"      );
    t_pol_angle_tree->Branch("second_energy",     &t_pol_angle.second_energy,      "second_energy/F"     );
    t_pol_angle_tree->Branch("is_adjacent",       &t_pol_angle.is_adjacent,        "is_adjacent/O"       );
    t_pol_angle_tree->Branch("is_na22",           &t_pol_angle.is_na22,            "is_na22/O"           );
    t_pol_angle_tree->Branch("is_bad_calib",      &t_pol_angle.is_bad_calib,       "is_bad_calib/O"      );
    t_pol_angle_tree->Branch("with_badch",        &t_pol_angle.with_badch,         "with_badch/O"        );
    t_pol_angle_tree->Branch("weight",            &t_pol_angle.weight,             "weight/F"            );
    t_pol_angle_tree->Branch("latitude",          &t_pol_angle.latitude,           "latitude/F"          );
    t_pol_angle_tree->Branch("longitude",         &t_pol_angle.longitude,          "longitude/F"         );

    // print file info
    Long64_t begin_entry = 0;
    Long64_t end_entry = t_pol_event_tree->GetEntries();
    t_pol_event_tree->GetEntry(begin_entry);
    int begin_time = static_cast<int>(t_pol_event.event_time);
    t_pol_event_tree->GetEntry(end_entry - 1);
    int end_time = static_cast<int>(t_pol_event.event_time);
    cout << options_mgr.pol_event_filename.Data()
         << " { " << begin_time << "[" << begin_entry << "] => "
         << end_time << "[" << end_entry - 1 << "] }" << endl;

    // calculate angle
    Na22Check na22_checker;
    Bar first_bar;
    Pos first_pos;
    Bar second_bar;
    Pos second_pos;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data and calculating angle ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = begin_entry; q < end_entry; q++) {
        cur_percent = static_cast<int>(q * 100.0 / (end_entry - begin_entry));
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);

        // start init angle
        t_pol_angle.event_time = t_pol_event.event_time;
        t_pol_angle.latitude = wgs84_to_latitude(t_pol_event.wgs84_xyz[0], t_pol_event.wgs84_xyz[1], t_pol_event.wgs84_xyz[2]);
        t_pol_angle.longitude = wgs84_to_longitude(t_pol_event.wgs84_xyz[0], t_pol_event.wgs84_xyz[1], t_pol_event.wgs84_xyz[2]);
        for (int i = 0; i < 2; i++) {
            t_pol_angle.first_ij[i] = -1;
            t_pol_angle.second_ij[i] = -1;
        }
        t_pol_angle.rand_angle = -1;
        t_pol_angle.rand_distance = -1;
        t_pol_angle.first_energy = -1;
        t_pol_angle.second_energy = -1;
        t_pol_angle.is_adjacent = false;
        t_pol_angle.is_na22 = false;
        t_pol_angle.is_bad_calib = false;
        t_pol_angle.with_badch = false;
        t_pol_angle.weight = 0;
        // stop init angle

        //////////////////////////////////////////////////////////////////////
        bool not_ready = false;
        // ped check
        if (t_pol_event.is_ped) not_ready = true;
        // good event check
        if (t_pol_event.pkt_count > 25) not_ready = true;
        if (t_pol_event.lost_count > 0) not_ready = true;
        // if not ready to calculate angle, save now
        if (not_ready) {
            t_pol_angle_tree->Fill();
            continue;
        }

        // overflow check
        int num_of_overflow = 0;
        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) continue;
            for (int j = 0; j < 64; j++) {
                if (t_pol_event.channel_status[i][j] & POLEvent::ADC_OVERFLOW) {
                    num_of_overflow++;
                }
            }
        }
        if (num_of_overflow > 2) {
            t_pol_angle_tree->Fill();
            continue;
        }

        // bad event check
        bool is_bad_event = false;
        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) continue;
            for (int j = 0; j < 64; j++) {
                if (t_pol_event.trigger_bit[i][j] && (t_pol_event.channel_status[i][j] & POLEvent::ADC_NOT_READOUT)) {
                    is_bad_event = true;
                    break;
                }
            }
            if (is_bad_event) break;
        }
        if (is_bad_event) {
            t_pol_angle_tree->Fill();
            continue;
        }

        /////////////////////////////////////////////////////////////////////

        // na22 check
        if (na22_checker.check_na22_event(t_pol_event)) {
            t_pol_angle.is_na22 = true;
        }

        // bad calib check
        bool cur_is_bad_calib = false;
        for (int i = 0; i < 25; i++) {
            if (t_pol_event.time_aligned[i]) {
                for (int j = 0; j < 64; j++) {
                    if (t_pol_event.trigger_bit[i][j] && (t_pol_event.channel_status[i][j] & POLEvent::BAD_CALIB)) {
                        cur_is_bad_calib = true;
                        break;
                    }
                }
            }
            if (cur_is_bad_calib) break;
        }
        t_pol_angle.is_bad_calib = cur_is_bad_calib;

        // bad channel check
        bool cur_with_badch = false;
        for (int i = 0; i < 25; i++) {
            if (t_pol_event.time_aligned[i]) {
                for (int j = 0; j < 64; j++) {
                    if (t_pol_event.trigger_bit[i][j] && bar_mask[i][j]) {
                        cur_with_badch = true;
                        break;
                    }
                }
            }
            if (cur_with_badch) break;
        }
        t_pol_angle.with_badch = cur_with_badch;

        ///////////////////////////////////////////////////////////////////////////

        // start angle calculating
        priority_queue<Bar> bar_queue;
        // clear bar_queue
        while (!bar_queue.empty()) bar_queue.pop();

        // find the first two bars'
        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) continue;
            for (int j = 0; j < 64; j++) {
                if (t_pol_event.trigger_bit[i][j]) {
                    bar_queue.push(Bar(t_pol_event.energy_value[i][j], i, j));
                }
            }
        }
        if (bar_queue.empty()) {
            t_pol_angle_tree->Fill();
            continue;
        }
        // get first bar
        first_bar = bar_queue.top();
        bar_queue.pop();
        first_pos.randomize(first_bar.i, first_bar.j);

        if (options_mgr.strict_flag) {
            // get second bar
            if (bar_queue.empty()) {
                t_pol_angle_tree->Fill();
                continue;
            }
            second_bar = bar_queue.top();
            bar_queue.pop();
            second_pos.randomize(second_bar.i, second_bar.j);
        } else {
            bool found_not_adjacent = false;
            while (!bar_queue.empty()) {
                second_bar = bar_queue.top();
                bar_queue.pop();
                second_pos.randomize(second_bar.i, second_bar.j);
                if (!first_pos.is_adjacent_to(second_pos)) {
                    found_not_adjacent = true;
                    break;
                }
            }
            if (!found_not_adjacent) {
                t_pol_angle_tree->Fill();
                continue;
            }
        }

        // calculate angle according to the first two bars
        t_pol_angle.first_ij[0]     = first_pos.i;
        t_pol_angle.first_ij[1]     = first_pos.j;
        t_pol_angle.second_ij[0]    = second_pos.i;
        t_pol_angle.second_ij[1]    = second_pos.j;
        t_pol_angle.rand_angle      = first_pos.angle_to(second_pos);
        t_pol_angle.rand_distance   = first_pos.distance_to(second_pos);
        t_pol_angle.first_energy    = t_pol_event.energy_value[first_pos.i][first_pos.j];
        t_pol_angle.second_energy   = t_pol_event.energy_value[second_pos.i][second_pos.j];
        t_pol_angle.is_adjacent     = first_pos.is_adjacent_to(second_pos);
        if (options_mgr.no_deadtime) {
            t_pol_angle.weight = 1.0;
        } else {
            if (first_pos.i == second_pos.i) {
                t_pol_angle.weight = 1.0 / (1.0 - t_dead_ratio.module_dead_ratio[first_pos.i]);
            } else {
                t_pol_angle.weight = (1.0 / (1.0 - t_dead_ratio.module_dead_ratio[first_pos.i])) *
                    (1 / (1 - t_dead_ratio.module_dead_ratio[second_pos.i]));
            }
        }
        // save angle
        t_pol_angle_tree->Fill();
    }
    cout << " DONE ]" << endl;

    t_pol_angle_file->cd();
    t_pol_angle_tree->Write();
    if (options_mgr.strict_flag) {
        TNamed("strict_mode", "true").Write();
    } else {
        TNamed("strict_mode", "false").Write();
    }
    t_pol_angle_file->Close();

    pol_event_file->Close();
    if (deadtime_file != NULL) {
        deadtime_file->Close();
    }

    return 0;
}
