#include <iostream>
#include "RootInc.hpp"
#include "CooConv.hpp"
#include "CommonCanvas.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE" << argv[0] << " <gain_file.root>" << endl;
        return 2;
    }
    string gain_filename = argv[1];
    TFile* gain_file = new TFile(gain_filename.c_str(), "read");
    if (gain_file->IsZombie()) {
        cout << "gain_file open failed." << endl;
        return 1;
    }
    TVectorF adc_per_kev[25];
    for (int i = 0; i < 25; i++) {
        adc_per_kev[i].ResizeTo(64);
    }
    TVectorF* tmp_vec;
    for (int i = 0; i < 25; i++) {
        tmp_vec = static_cast<TVectorF*>(gain_file->Get(Form("adc_per_kev_ct_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("adc_per_kev_ct_%02d", i + 1) << endl;
            return 1;
        } else {
            adc_per_kev[i] = *tmp_vec;
        }
    }
    gain_file->Close();
    delete gain_file;
    gain_file = NULL;

    TH2D* gain_map = new TH2D("gain_map", "Gain Map of 1600 Channels", 40, 0, 40, 40, 0, 40);
    gain_map->SetDirectory(NULL);
    gain_map->GetXaxis()->SetNdivisions(40);
    gain_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            gain_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            gain_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            gain_map->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, adc_per_kev[i](j));
        }
    }

    TLine* line_h[4];
    TLine* line_v[4];
    for (int i = 0; i < 4; i++) {
        line_h[i] = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
        line_h[i]->SetLineColor(kWhite);
        line_v[i] = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
        line_v[i]->SetLineColor(kWhite);
    }

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);
    CommonCanvas canvas;
    gStyle->SetOptStat(0);
    canvas.cd();
    canvas.get_canvas()->SetGrid();
    gain_map->Draw("colz");
    for (int i = 0; i < 4; i++) {
        line_h[i]->Draw();
        line_v[i]->Draw();
    }

    rootapp->Run();

    return 0;
}
