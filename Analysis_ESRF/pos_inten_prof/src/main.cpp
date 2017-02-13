#include <iostream>
#include "RootInc.hpp"
#include "Na22Info.hpp"
#include "BarPos.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <inten_filename.root> <pos_inten_prof.root>" << endl;
        return 2;
    }
    string inten_filename = argv[1];
    string pos_inten_prof_filename = argv[2];

    struct {
        double ct_time_second_3;
        double current;
        double motor_x;
        double motor_y;
    } t_beam_intensity;

    TFile* inten_file = new TFile(inten_filename.c_str(), "read");
    if (inten_file->IsZombie()) {
        cout << "inten_file open failed." << endl;
        return 1;
    }
    TTree* t_beam_intensity_tree = static_cast<TTree*>(inten_file->Get("t_beam_intensity"));
    if (t_beam_intensity_tree == NULL) {
        cout << "cannot find TTree t_beam_intensity" << endl;
        return 1;
    }

    t_beam_intensity_tree->SetBranchAddress("ct_time_second_3",  &t_beam_intensity.ct_time_second_3   );
    t_beam_intensity_tree->SetBranchAddress("current",           &t_beam_intensity.current            );
    t_beam_intensity_tree->SetBranchAddress("motor_x",           &t_beam_intensity.motor_x            );
    t_beam_intensity_tree->SetBranchAddress("motor_y",           &t_beam_intensity.motor_y            );

    // get the first and last time
    t_beam_intensity_tree->GetEntry(0);
    double ct_time_second_first = t_beam_intensity.ct_time_second_3;
    t_beam_intensity_tree->GetEntry(t_beam_intensity_tree->GetEntries() - 1);
    double ct_time_second_last  = t_beam_intensity.ct_time_second_3;

    // prepare profile
    double binw = 0.1;
    int nbins = static_cast<int>((ct_time_second_last - ct_time_second_first) / binw);
    TProfile* intensity_prof;
    intensity_prof = new TProfile("intensity_prof", "intensity_prof", nbins,
            ct_time_second_first, ct_time_second_last, 10, 60);
    intensity_prof->SetDirectory(NULL);
    TProfile* distance_prof[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            distance_prof[i][j] = new TProfile(Form("distance_prof_CT_%02d_%02d", i + 1, j),
                    Form("distance_prof_CT_%02d_%02d", i + 1, j), nbins,
                    ct_time_second_first, ct_time_second_last, 0, 500);
            distance_prof[i][j]->SetDirectory(NULL);
        }
    }

    cout << "reading data ..." << endl;
    for (Long64_t q = 0; q < t_beam_intensity_tree->GetEntries(); q++) {
        if (q % 10000 == 0) {
            cout << q << endl;
        }
        t_beam_intensity_tree->GetEntry(q);
        intensity_prof->Fill(t_beam_intensity.ct_time_second_3, t_beam_intensity.current);
        for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 64; j++) {
                double cur_x = ijtox(i, j) / 8 * ModD + ijtox(i, j) % 8 * BarD;
                double cur_y = ijtoy(i, j) / 8 * ModD + ijtoy(i, j) % 8 * BarD;
                double delta_x = cur_x - t_beam_intensity.motor_x;
                double delta_y = cur_y - t_beam_intensity.motor_y;
                double cur_dist = TMath::Sqrt(delta_x * delta_x + delta_y * delta_y);
                distance_prof[i][j]->Fill(t_beam_intensity.ct_time_second_3, cur_dist);
            }
        }
    }
    cout << "done." << endl;
    inten_file->Close();

    cout << "saving historgram ..." << endl;
    TFile* t_file_out = new TFile(pos_inten_prof_filename.c_str(), "recreate");
    if (t_file_out->IsZombie()) {
        cout << "pos_inten_prof file open failed." << endl;
        return 1;
    }
    for (int i = 0; i < 25; i++) {
        t_file_out->mkdir(Form("CT_%02d", i + 1))->cd();
        for (int j = 0; j < 64; j++) {
            distance_prof[i][j]->Write();
        }
    }
    t_file_out->cd();
    intensity_prof->Write();
    t_file_out->Close();

    return 0;
}
