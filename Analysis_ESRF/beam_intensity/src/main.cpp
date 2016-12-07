#include <iostream>
#include <cstdlib>
#include <list>
#include <iterator>
#include "RootInc.hpp"
#include "Na22Info.hpp"
#include "CooConv.hpp"

using namespace std;

static double start_unixtime = 1432392039.0;
static double pos_x_0 = -35.68;
static double pos_y_0 = 155.39;

struct Motor_T {
    Double_t unixtime;
    Double_t y;
    Double_t z;
    Double_t current;
};

int main(int argc, char** argv) {
    if (argc < 6) {
        cout << "USAGE: " << argv[0] << " <event_data.root> <rate_data.root> <log_data.root> <intensity.root> <time_diff.root>" << endl;
        return 1;
    }
    string event_data_filename = argv[1];
    string rate_data_filename = argv[2];
    string log_data_filename = argv[3];
    string intensity_filename = argv[4];
    string time_diff_filename = argv[5];

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
    t_event_tree->GetEntry(0);
    double ct_time_second_first = t_event.ct_time_second;
    t_event_tree->GetEntry(t_event_tree->GetEntries() - 1);
    double ct_time_second_last = t_event.ct_time_second;
    double ct_time_second_length = ct_time_second_last - ct_time_second_first;

    // open rate_data file ============================================================
    cout << "open rate_data file ..." << endl;
    TFile* t_file_rate = new TFile(rate_data_filename.c_str(), "read");
    TMatrixF begin_time_mat(25, 64);
    TMatrixF end_time_mat(25, 64);
    TMatrixF* tmp_mat;
    tmp_mat = static_cast<TMatrixF*>(t_file_rate->Get("begin_time_mat"));
    if (tmp_mat == NULL) {
        cout << "cannot read TMatrixF begin_time_mat" << endl;
        return 1;
    } else {
        begin_time_mat = *tmp_mat;
    }
    tmp_mat = static_cast<TMatrixF*>(t_file_rate->Get("end_time_mat"));
    if (tmp_mat == NULL) {
        cout << "cannot read TMatrixF end_time_mat" << endl;
        return 1;
    } else {
        end_time_mat = *tmp_mat;
    }
    t_file_rate->Close();
    TMatrixF bar_time_obox(25, 64);
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            bar_time_obox(i, j) = (end_time_mat(i, j) + begin_time_mat(i, j)) / 2 + start_unixtime;
        }
    }

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
    list<Motor_T> t_corrected_list;
    double pre_unixtime = 0;
    for (Long64_t i = 0; i < t_corrected_tree->GetEntries(); i++) {
        t_corrected_tree->GetEntry(i);
        if (t_corrected.unixtime <= pre_unixtime) {
            continue;
        } else {
            pre_unixtime = t_corrected.unixtime;
        }
        if (t_corrected.unixtime > start_unixtime && t_corrected.unixtime < start_unixtime + ct_time_second_length) {
            t_corrected_list.push_back(t_corrected);
        }
    }

    // calculate time difference
    cout << "calculate time difference ..." << endl;
    TFile* t_file_time_diff = new TFile(time_diff_filename.c_str(), "recreate");
    TH1F* time_diff_hist = new TH1F("time_diff_hist", "time_diff_hist", 2000, -1000, 1000);
    int match_count = 0;
    int total_check = 0;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            double cur_x = pos_x_0 + ijtox(i, j) / 8 * ModD + ijtox(i, j) % 8 * BarD;
            double cur_y = pos_y_0 + ijtoy(i, j) / 8 * ModD + ijtoy(i, j) % 8 * BarD;
            for (list<Motor_T>::iterator motor_iter = t_corrected_list.begin(); motor_iter != t_corrected_list.end(); motor_iter++) {
                total_check++;
                if (fabs(cur_x - (*motor_iter).z) < BarD / 2 && fabs(cur_y - (*motor_iter).y) < BarD / 2) {
                    time_diff_hist->Fill(bar_time_obox(i, j) - (*motor_iter).unixtime);
                    match_count++;
                    break;
                }
            }
        }
    }
    cout << "total_check = " << total_check << endl;
    cout << "match_count = " << match_count << endl;
    t_file_time_diff->cd();
    time_diff_hist->Write();
    t_file_time_diff->Close();

    t_file_event->Close();
    t_file_log->Close();


    return 0;
}
