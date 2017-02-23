#include <iostream>
#include <cstdlib>
#include <list>
#include <iterator>
#include "RootInc.hpp"
#include "Na22Info.hpp"
#include "CooConv.hpp"

using namespace std;

static double first_time_second = 81.8464;
static double start_unixtime = 1432508820.0 - first_time_second;

struct Motor_T {
    Double_t unixtime;
    Double_t y;
    Double_t z;
    Double_t current;
};

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << " <event_data.root> <log_data.root> <intensity.root>" << endl;
        return 1;
    }
    string event_data_filename = argv[1];
    string log_data_filename =   argv[2];
    string intensity_filename =  argv[3];

    // open event_data file =============================================================
    cout << "open event_data file ..." << endl;
    TFile* t_file_event = new TFile(event_data_filename.c_str(), "read");
    if (t_file_event->IsZombie()) {
        cout << "event root file open failed." << endl;
        return 1;
    }
    TTree* t_event_tree = static_cast<TTree*>(t_file_event->Get("t_event"));
    if (t_event_tree == NULL) {
        cout << "cannot find TTree t_event" << endl;
        return 1;
    }
    struct {
        Double_t ct_time_second;
    } t_event;
    t_event_tree->SetBranchAddress("ct_time_second",      &t_event.ct_time_second          );
    t_event_tree->SetBranchStatus("*", false);
    t_event_tree->SetBranchStatus("ct_time_second", true);

    // open log_data file =============================================================
    cout << "open log_data file ..." << endl;
    TFile* t_file_log = new TFile(log_data_filename.c_str(), "read");
    if (t_file_log->IsZombie()) {
        cout << "rate root file open failed." << endl;
        return 1;
    }
    TTree* t_corrected_tree = static_cast<TTree*>(t_file_log->Get("corrected"));
    if (t_corrected_tree == NULL) {
        cout << "cannot file TTree corrected" << endl;
        return 1;
    }
    Motor_T t_corrected;
    t_corrected_tree->SetBranchAddress("unixtime",   &t_corrected.unixtime );
    t_corrected_tree->SetBranchAddress("y",          &t_corrected.y        );
    t_corrected_tree->SetBranchAddress("z",          &t_corrected.z        );
    t_corrected_tree->SetBranchAddress("current",    &t_corrected.current  );
    t_corrected_tree->SetBranchStatus("*", false);
    t_corrected_tree->SetBranchStatus("unixtime", true);
    t_corrected_tree->SetBranchStatus("y", true);
    t_corrected_tree->SetBranchStatus("z", true);
    t_corrected_tree->SetBranchStatus("current", true);

    double time_diff_mean = 11.0;

    // calculate beam intensity for each event
    TFile* t_file_intensity = new TFile(intensity_filename.c_str(), "recreate");
    if (t_file_intensity->IsZombie()) {
        cout << "intensity root file open failed." << endl;
        return 1;
    }
    struct {
        double ct_time_second_3;
        double current;
    } t_beam_intensity;
    TTree* t_beam_intensity_tree = new TTree("t_beam_intensity", "beam intensity for each event");
    t_beam_intensity_tree->Branch("ct_time_second_3",  &t_beam_intensity.ct_time_second_3,  "ct_time_second_3/D" );
    t_beam_intensity_tree->Branch("current",           &t_beam_intensity.current,           "current/D"          );
    Long64_t motor_cur_entry = 0;
    bool     motor_reach_end = false;
    t_corrected_tree->GetEntry(motor_cur_entry);
    double current_before = t_corrected.current;
    double unixcor_before = t_corrected.unixtime + time_diff_mean;
    motor_cur_entry++;
    t_corrected_tree->GetEntry(motor_cur_entry);
    double current_after  = t_corrected.current;
    double unixcor_after  = t_corrected.unixtime + time_diff_mean;
    double current_slope = (current_after - current_before) / (unixcor_after - unixcor_before);
    for (Long64_t i = 0; i < t_event_tree->GetEntries(); i++) {
        t_event_tree->GetEntry(i);
        while (t_event.ct_time_second + start_unixtime > unixcor_after) {
            do {
                motor_cur_entry++;
                if (motor_cur_entry < t_corrected_tree->GetEntries()) {
                    t_corrected_tree->GetEntry(motor_cur_entry);
                } else {
                    motor_reach_end = true;
                    break;
                }
            } while (t_corrected.unixtime + time_diff_mean - unixcor_after < 0.5);
            if (motor_reach_end) {
                break;
            } else {
                current_before = current_after;
                unixcor_before = unixcor_after;
                current_after  = t_corrected.current;
                unixcor_after  = t_corrected.unixtime + time_diff_mean;
                current_slope  = (current_after - current_before) / (unixcor_after - unixcor_before);
            }
        }
        // save current
        t_beam_intensity.ct_time_second_3 = t_event.ct_time_second;
        t_beam_intensity.current = current_before + current_slope * (t_event.ct_time_second + start_unixtime - unixcor_before);
        t_beam_intensity_tree->Fill();
    }
    t_file_intensity->cd();
    t_beam_intensity_tree->Write();
    TNamed("m_fromfile", TSystem().BaseName(event_data_filename.c_str())).Write();
    t_file_intensity->Close();

    t_file_event->Close();
    t_file_log->Close();

    return 0;
}
