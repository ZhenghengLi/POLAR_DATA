#include <iostream>
#include <fstream>
#include <sstream>
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "CooConv.hpp"

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

    cout << "reading bad channels ..." << endl;
    bool bad_channels[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            bad_channels[i][j] = false;
        }
    }
    if (!options_mgr.bad_ch_filename.IsNull()) {
        char line_buffer[100];
        ifstream infile;
        infile.open(options_mgr.bad_ch_filename.Data());
        if (!infile.is_open()) {
            cout << "bad_channels_file open failed." << endl;
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
                cout << "bad channel: ct_" << ct_num << ", ch_" << ch_idx << endl;
                bad_channels[ct_num - 1][ch_idx] = true;
            }
        }
        infile.close();
    }

    cout << "reading nonlinearity function parameters ..." << endl;
    TFile* input_file = new TFile(options_mgr.input_filename.Data(), "read");
    if (input_file->IsZombie()) {
        cout << "input_file open failed: " << options_mgr.input_filename << endl;
        return 1;
    }
    TVectorF nonlin_fun_p0[25];
    TVectorF nonlin_fun_p1[25];
    TVectorF nonlin_fun_p2[25];
    TVectorF* tmp_vec = NULL;
    for (int i = 0; i < 25; i++) {
        tmp_vec = static_cast<TVectorF*>(input_file->Get(Form("nonlin_fun_p0_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("nonlin_fun_p0_%02d", i + 1) << endl;
            return 1;
        } else {
            nonlin_fun_p0[i].ResizeTo(64);
            nonlin_fun_p0[i] = *tmp_vec;
        }
        tmp_vec = static_cast<TVectorF*>(input_file->Get(Form("nonlin_fun_p1_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("nonlin_fun_p1_%02d", i + 1) << endl;
            return 1;
        } else {
            nonlin_fun_p1[i].ResizeTo(64);
            nonlin_fun_p1[i] = *tmp_vec;
        }
        tmp_vec = static_cast<TVectorF*>(input_file->Get(Form("nonlin_fun_p2_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("nonlin_fun_p2_%02d", i + 1) << endl;
            return 1;
        } else {
            nonlin_fun_p2[i].ResizeTo(64);
            nonlin_fun_p2[i] = *tmp_vec;
        }
    }
    input_file->Close();
    delete input_file;
    input_file = NULL;

    cout << "correcting bad channels, normalizing and writing ..." << endl;
    for (int i = 0; i < 25; i++) {
        double p0_sum(0), p0_n(0), p0_mean(0);
        double p1_sum(0), p1_n(0), p1_mean(0);
        double p2_sum(0), p2_n(0), p2_mean(0);
        for (int j = 0; j < 64; j++) {
            if (!bad_channels[i][j]) {
                p0_sum += nonlin_fun_p0[i](j);
                p0_n++;
                p1_sum += nonlin_fun_p1[i](j);
                p1_n++;
                p2_sum += nonlin_fun_p2[i](j);
                p2_n++;
            }
        }
        p0_mean = (p0_n > 0 ? p0_sum / p0_n : 1);
        p1_mean = (p1_n > 0 ? p1_sum / p1_n : 0);
        p2_mean = (p2_n > 0 ? p2_sum / p2_n : 1);
        for (int j = 0; j < 64; j++) {
            if (bad_channels[i][j]) {
                nonlin_fun_p0[i](j) = p0_mean;
                nonlin_fun_p1[i](j) = p1_mean;
                nonlin_fun_p2[i](j) = p2_mean;
            }
        }
        // normarlize
        for (int j = 0; j < 64; j++) {
            double cur_norm = nonlin_fun_p0[i](j) * (1 + nonlin_fun_p1[i](j) * norm_pos[i]) * (1 + TMath::Erf(norm_pos[i] / nonlin_fun_p2[i](j))) / 2.0;
            nonlin_fun_p0[i](j) /= cur_norm;
        }

    }
    TF1* nonlin_fun[25][64];
    TCanvas* canvas_nonlin_fun[25];
    for (int i = 0; i < 25; i++) {
        canvas_nonlin_fun[i] = new TCanvas(Form("canvas_nonlin_norm_fun_%02d", i + 1), Form("canvas_nonlin_norm_fun_%02d", i + 1), 1800, 1500);
        canvas_nonlin_fun[i]->Divide(8, 8);
        canvas_nonlin_fun[i]->SetFillColor(kYellow);
        for (int j = 0; j < 64; j++) {
            nonlin_fun[i][j] = new TF1(Form("nonlin_fun_%02d_%02d", i + 1, j), "[0] * (1 + [1] * x) * (1 + TMath::Erf(x / [2])) / 2", 0, 4096);
            nonlin_fun[i][j]->SetParameters(nonlin_fun_p0[i](j), nonlin_fun_p1[i](j), nonlin_fun_p2[i](j));
            canvas_nonlin_fun[i]->cd(jtoc(j));
            canvas_nonlin_fun[i]->GetPad(jtoc(j))->SetFillColor(kWhite);
            nonlin_fun[i][j]->Draw();

        }
    }
    TFile* output_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (output_file->IsZombie()) {
        cout << "output_file open failed: " << options_mgr.output_filename << endl;
        return 1;
    }
    output_file->cd();
    for (int i = 0; i < 25; i++) {
        nonlin_fun_p0[i].Write(Form("nonlin_norm_fun_p0_%02d", i + 1));
        nonlin_fun_p1[i].Write(Form("nonlin_norm_fun_p1_%02d", i + 1));
        nonlin_fun_p2[i].Write(Form("nonlin_norm_fun_p2_%02d", i + 1));
        canvas_nonlin_fun[i]->Write();
    }
    output_file->Close();
    delete output_file;
    output_file = NULL;

    return 0;
}
