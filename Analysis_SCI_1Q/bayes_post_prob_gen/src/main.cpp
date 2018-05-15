#include <iostream>
#include "RootInc.hpp"

using namespace std;

bool read_prob(const char* filename, double& prob_m, int& pa_idx, int& pd_idx) {
    TFile* prob_file = new TFile(filename, "read");
    if (prob_file->IsZombie()) {
        cout << "prob_file open failed: " << filename << endl;
        return false;
    }
    TNamed* tmp_tnamed = NULL;
    // pa_idx
    tmp_tnamed = static_cast<TNamed*>(prob_file->Get("pa_idx"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed pa_idx" << endl;
        return false;
    } else {
        pa_idx = TString(tmp_tnamed->GetTitle()).Atoi();
    }
    // pd_idx
    tmp_tnamed = static_cast<TNamed*>(prob_file->Get("pd_idx"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed pd_idx" << endl;
        return false;
    } else {
        pd_idx = TString(tmp_tnamed->GetTitle()).Atoi();
    }
    // prob_m
    tmp_tnamed = static_cast<TNamed*>(prob_file->Get("prob_m"));
    if (tmp_tnamed == NULL) {
        cout << "cannot find TNamed prob_m" << endl;
        return false;
    } else {
        prob_m = TString(tmp_tnamed->GetTitle()).Atof();
    }
    // close file
    prob_file->Close();
    delete prob_file;

    return true;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <post_prob.root> <prob_1.root> <prob_2.root> ..." << endl;
        return 2;
    }

    bool read_flag[90][50];
    double prob_m_array[90][50];
    for (int i = 0; i < 90; i++) {
        for (int j = 0; j < 50; j++) {
            read_flag[i][j] = false;
            prob_m_array[i][j] = 0;
        }
    }

    cout << "Reading probability at measurement ..." << endl;
    int pa_idx;
    int pd_idx;
    double prob_m;
    double prob_m_sum = 0;
    for (int q = 2; q < argc; q++) {
        if (read_prob(argv[q], prob_m, pa_idx, pd_idx)) {
            read_flag[pa_idx][pd_idx] = true;
            prob_m_sum += prob_m;
            prob_m_array[pa_idx][pd_idx] = prob_m;
        } else {
            cout << "read prob file failed: " << argv[q] << endl;
            return 1;
        }
    }

    bool all_read = true;
    for (int i = 0; i < 90; i++) {
        for (int j = 0; j < 50; j++) {
            if (!read_flag[i][j]) {
                all_read = false;
                cout << Form("PA_%02d_PD_%02d", i * 2 + 1, j * 2 + 1) << " not read" << endl;
            }
        }
    }

    if (all_read) {
        cout << "All prob values are read" << endl;
    } else {
        cout << "Not all prob values are read" << endl;
        return 1;
    }

    cout << "Calculating and writing posterior probability map ..." << endl;
    TFile* output_file = new TFile(argv[1], "recreate");
    if (output_file->IsZombie()) {
        cout << "output_file open failed: " << argv[1] << endl;
        return 1;
    }

    TH2D* prob_m_map = new TH2D("prob_m_map", "prob_m_map", 50, 0, 100, 90, 0, 180);
    for (int i = 0; i < 90; i++) {
        for (int j = 0; j < 50; j++) {
            prob_m_map->SetBinContent(j + 1, i + 1, prob_m_array[i][j] / prob_m_sum);
        }
    }
    output_file->cd();
    prob_m_map->Write();

    return 0;
}
