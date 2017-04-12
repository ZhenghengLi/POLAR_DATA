#include <iostream>
#include "RootInc.hpp"
#include "CommonCanvas.hpp"
#include "CooConv.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <ped_res.root>" << endl;
        return 2;
    }

    TVectorF ped_mean_vec_ct[25];
    TVectorF ped_shift_mean_vec_ct[25];
    TVectorF ped_shift_sigma_vec_ct[25];
    for (int i = 0; i < 25; i++) {
        ped_mean_vec_ct[i].ResizeTo(64);
        ped_shift_mean_vec_ct[i].ResizeTo(64);
        ped_shift_sigma_vec_ct[i].ResizeTo(64);
    }
    TVectorF common_noise_vec(25);

    // open ped_res_file
    TFile* ped_res_file = new TFile(argv[1], "read");
    if (ped_res_file->IsZombie()) {
        cout << "ped_result file open failed: " << argv[1] << endl;
        return 1;
    }
    TVectorF* tmp_vec;
    // read ped result
    for (int i = 0; i < 25; i++) {
        tmp_vec = static_cast<TVectorF*>(ped_res_file->Get(Form("ped_mean_vec_ct_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("ped_mean_vec_ct_%02d", i + 1) << endl;
            return 1;
        } else {
            ped_mean_vec_ct[i] = *tmp_vec;
        }
        tmp_vec = static_cast<TVectorF*>(ped_res_file->Get(Form("ped_shift_mean_vec_ct_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("ped_shift_mean_vec_ct_%02d", i + 1) << endl;
            return 1;
        } else {
            ped_shift_mean_vec_ct[i] = *tmp_vec;
        }
        tmp_vec = static_cast<TVectorF*>(ped_res_file->Get(Form("ped_shift_sigma_vec_ct_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("ped_shift_sigma_vec_ct_%02d", i + 1) << endl;
            return 1;
        } else {
            ped_shift_sigma_vec_ct[i] = *tmp_vec;
        }
    }
    tmp_vec = static_cast<TVectorF*>(ped_res_file->Get("common_noise_vec"));
    if (tmp_vec == NULL) {
        cout << "cannot find TVectorF common_noise_vec" << endl;
        return 1;
    } else {
        common_noise_vec = *tmp_vec;
    }
    ped_res_file->Close();
    delete ped_res_file;
    ped_res_file = NULL;

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);
    CommonCanvas canvas_noise;
    gStyle->SetOptStat(0);
    TLine* line_h[4];
    TLine* line_v[4];
    for (int i = 0; i < 4; i++) {
        line_h[i] = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
        line_h[i]->SetLineColor(kWhite);
        line_v[i] = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
        line_v[i]->SetLineColor(kWhite);
    }
    // pedestal mean
    TH2D* ped_map = new TH2D("ped_map", "Pedestal Map of 1600 Channels", 40, 0, 40, 40, 0, 40);
    ped_map->SetDirectory(NULL);
    ped_map->GetXaxis()->SetNdivisions(40);
    ped_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            ped_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            ped_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ped_map->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, ped_mean_vec_ct[i](j));
        }
    }
    canvas_noise.cd(1);
    canvas_noise.get_canvas()->GetPad(1)->SetGrid();
    ped_map->DrawCopy("COLZ");
    for (int i = 0; i < 4; i++) {
        line_h[i]->Draw();
        line_v[i]->Draw();
    }
    // common_noise
    TH2D* common_noise_map = new TH2D("common_noise_map", "Common Noise Map of 25 Modules", 5, 0, 5, 5, 0, 5);
    common_noise_map->SetDirectory(NULL);
    common_noise_map->GetXaxis()->SetNdivisions(5);
    common_noise_map->GetYaxis()->SetNdivisions(5);
    for (int i = 0; i < 25; i++) {
        common_noise_map->SetBinContent(itox(i) + 1, itoy(i) + 1, common_noise_vec(i));
    }
    canvas_noise.cd(2);
    canvas_noise.get_canvas()->GetPad(2)->SetGrid();
    common_noise_map->DrawCopy("COLZ");
    // ped_shit
    TH2D* ped_shift_map = new TH2D("ped_shift_map", "Pedestal Shift Map of 1600 Channels After Common Noise Subtracted", 40, 0, 40, 40, 0, 40);
    ped_shift_map->SetDirectory(NULL);
    ped_shift_map->GetXaxis()->SetNdivisions(40);
    ped_shift_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            ped_shift_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            ped_shift_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ped_shift_map->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, ped_shift_mean_vec_ct[i](j));
        }
    }
    canvas_noise.cd(3);
    canvas_noise.get_canvas()->GetPad(3)->SetGrid();
    ped_shift_map->DrawCopy("COLZ");
    for (int i = 0; i < 4; i++) {
        line_h[i]->Draw();
        line_v[i]->Draw();
    }
    // intrinsic noise
    TH2D* intrinsic_noise_map = new TH2D("intrinsic_noise_map", "Intrinsic Noise Map of 1600 Channels", 40, 0, 40, 40, 0, 40);
    intrinsic_noise_map->SetDirectory(NULL);
    intrinsic_noise_map->GetXaxis()->SetNdivisions(40);
    intrinsic_noise_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            intrinsic_noise_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            intrinsic_noise_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            intrinsic_noise_map->SetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1, ped_shift_sigma_vec_ct[i](j));
        }
    }
    canvas_noise.cd(4);
    canvas_noise.get_canvas()->GetPad(4)->SetGrid();
    intrinsic_noise_map->DrawCopy("COLZ");
    for (int i = 0; i < 4; i++) {
        line_h[i]->Draw();
        line_v[i]->Draw();
    }

    rootapp->Run();
    return 0;
}
