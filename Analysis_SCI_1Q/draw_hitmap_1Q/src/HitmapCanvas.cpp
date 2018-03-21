#include "HitmapCanvas.hpp"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstdio>

using namespace std;

HitmapCanvas::HitmapCanvas() {
    canvas_count_ = NULL;
    count_map_ = new TH2D("count_hist", "Hit Map", 40, 0, 40, 40, 0, 40);
    count_map_->SetDirectory(NULL);
    count_map_->GetXaxis()->SetNdivisions(40);
    count_map_->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            count_map_->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            count_map_->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    for (int i = 0; i < 4; i++) {
        line_h_[i] = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
        line_h_[i]->SetLineColor(kWhite);
        line_v_[i] = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
        line_v_[i]->SetLineColor(kWhite);
    }
}


void HitmapCanvas::set_count_map(int ch_count[25][64]) {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            int x = ijtox(i, j) + 1;
            int y = ijtoy(i, j) + 1;
            count_map_->SetBinContent(x, y, ch_count[i][j]);
        }
    }
}

HitmapCanvas::~HitmapCanvas() {
    delete count_map_;
    count_map_ = NULL;
}

void HitmapCanvas::CloseWindow() {
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void HitmapCanvas::ProcessAction(Int_t event, Int_t px, Int_t py, TObject* selected) {
    if (event != kButton1Down)
        return;
}

void HitmapCanvas::draw_count_map() {
    canvas_count_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_count"));
    if (canvas_count_ == NULL) {
        canvas_count_ = new TCanvas("canvas_count", "total trigger counts of 25 modules", 600, 600);
        canvas_count_->SetGrid();
        canvas_count_->Connect("Closed()", "HitmapCanvas", this, "CloseWindow()");
        // canvas_count_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "HitmapCanvas",
        //                        this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    canvas_count_->cd();
    count_map_->Draw("colz");
    for (int i = 0; i < 4; i++) {
        line_h_[i]->Draw("same");
        line_v_[i]->Draw("same");
    }

}

