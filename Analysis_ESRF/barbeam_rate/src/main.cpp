#include <iostream>
#include "RootInc.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 5) {
        cout << "USAGE: " << argv[0] << " <event_data.root> <intensity.root> <rate.root> <output.root>" << endl;
        return 2;
    }
    string event_data_fn = argv[1];
    string intensity_fn = argv[2];
    string rate_fn = argv[3];
    string output_fn = argv[4];

    // open event_data
    TFile* t_event_file = new TFile(event_data_fn.c_str(), "read");
    if (t_event_file->IsZombie()) {
        cout << "event root file open failed." << endl;
        return 1;
    }
    TTree* t_event_tree = static_cast<TTree*>(t_event_file->Get("t_event"));
    if (t_event_tree == NULL) {
        cout << "cannot find TTree t_event" << endl;
        return 1;
    }
    // open intensity file
    TFile* t_beam_intensity_file = new TFile(intensity_fn.c_str(), "read");
    if (t_beam_intensity_file->IsZombie()) {
        cout << "intensity root file open failed." << endl;
        return 1;
    }
    TTree* t_beam_intensity_tree = static_cast<TTree*>(t_beam_intensity_file->Get("t_beam_intensity"));
    if (t_beam_intensity_tree == NULL) {
        cout << "cannot find TTree t_beam_intensity" << endl;
        return 1;
    }
    t_event_tree->AddFriend(t_beam_intensity_tree);
    struct {
        Int_t    type;
        Double_t ct_time_second;
        Bool_t   time_aligned[25];
        Bool_t   trigger_bit[25][64];
        Float_t  energy_value[25][64];
        Double_t current;
    } t_event;
    t_event_tree->SetBranchAddress("type",            &t_event.type             );
    t_event_tree->SetBranchAddress("ct_time_second",  &t_event.ct_time_second   );
    t_event_tree->SetBranchAddress("time_aligned",     t_event.time_aligned     );
    t_event_tree->SetBranchAddress("trigger_bit",      t_event.trigger_bit      );
    t_event_tree->SetBranchAddress("energy_value",     t_event.energy_value     );
    t_event_tree->SetBranchAddress("current",         &t_event.current          );
    t_event_tree->SetBranchStatus("*", false);
    t_event_tree->SetBranchStatus("type", true);
    t_event_tree->SetBranchStatus("ct_time_second", true);
    t_event_tree->SetBranchStatus("time_aligned", true);
    t_event_tree->SetBranchStatus("trigger_bit", true);
    t_event_tree->SetBranchStatus("energy_value");
    t_event_tree->SetBranchStatus("current", true);

    // read barbeam time span
    TFile* t_rate_file = new TFile(rate_fn.c_str(), "read");
    TMatrixF* tmp_mat;
    TMatrixF begin_time_mat(25, 64), end_time_mat(25, 64);
    tmp_mat = static_cast<TMatrixF*>(t_rate_file->Get("begin_time_mat"));
    if (tmp_mat != NULL) {
        begin_time_mat = *tmp_mat;
    } else {
        cout << "cannot find TMatrixF begin_time_mat" << endl;
        return 1;
    }
    tmp_mat = static_cast<TMatrixF*>(t_rate_file->Get("end_time_mat"));
    if (tmp_mat != NULL) {
        end_time_mat = *tmp_mat;
    } else {
        cout << "cannot find TMatrixF end_tim_mat" << endl;
        return 1;
    }
    t_rate_file->Close();
    delete t_rate_file;
    t_rate_file = NULL;

    // open output file
    TFile* t_file_out = new TFile(output_fn.c_str(), "recreate");

    int    barbeam_total_counts[25][64];
    double barbeam_total_intensity[25][64];
    // initialize
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            barbeam_total_counts[i][j] = 0;
            barbeam_total_intensity[i][j] = 0;
        }
    }

    // read data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_event_tree->GetEntry(q);
        // if (t_event.type != 0xF000) continue;
        for (int i = 0; i < 25; i++) {
            if (!t_event.time_aligned[i]) continue;
            for (int j = 0; j < 64; j++) {
                if (t_event.trigger_bit[i][j] && t_event.ct_time_second > begin_time_mat[i][j] && t_event.ct_time_second < end_time_mat[i][j]) {
                    if (t_event.energy_value[i][j] < 25.0) continue;
                    barbeam_total_counts[i][j] += 1;
                    barbeam_total_intensity[i][j] += t_event.current;
                }
            }
        }

    }
    cout << " DONE ]" << endl;
    t_event_file->Close();
    t_beam_intensity_file->Close();

    // calculate intensity and rate of each bar
    TMatrixF bar_rate(25, 64);
    TMatrixF bar_intensity(25, 64);
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            if (end_time_mat(i, j) - begin_time_mat(i, j) > 0 && barbeam_total_counts[i][j] > 0) {
                bar_rate(i, j) = barbeam_total_counts[i][j] / (end_time_mat(i, j) - begin_time_mat(i, j));
                bar_intensity(i, j) = barbeam_total_intensity[i][j] / barbeam_total_counts[i][j];
            } else {
                bar_rate(i, j) = -1;
                bar_intensity(i, j) = -1;
            }
        }
    }

    t_file_out->cd();
    bar_rate.Write("bar_rate");
    bar_intensity.Write("bar_intensity");
    t_file_out->Close();

    return 0;
}
