#include "CommonCanvas.hpp"

using namespace std;

CommonCanvas::CommonCanvas() {
    for (int i = 0; i < 4; i++) {
        line_h_[i] = NULL;
        line_v_[i] = NULL;
    }
}

CommonCanvas::~CommonCanvas() {

}

void CommonCanvas::CloseWindow() {
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void CommonCanvas::ProcessAction(Int_t event, Int_t px, Int_t py, TObject* selected) {
    if (event != kButton1Down)
        return;
    canvas_na22_hitmap_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_na22_hitmap"));
    if (canvas_na22_hitmap_ == NULL)
        return;
    int x = static_cast<int>(canvas_na22_hitmap_->AbsPixeltoX(px));
    int y = static_cast<int>(canvas_na22_hitmap_->AbsPixeltoY(py));
    draw_ce_fitting(xytoi(x, y));

}

void CommonCanvas::init(TH2F* spec_cnt_map, TH1F* spec_h[25][64], TF1* spec_f[25][64]) {
    spec_count_map_ = spec_cnt_map;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            spec_hist_[i][j] = spec_h[i][j];
            spec_func_[i][j] = spec_f[i][j];
        }
    }
}

void CommonCanvas::draw_na22_hitmap() {
    gStyle->SetOptStat(0);
    canvas_na22_hitmap_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_na22_hitmap"));
    if (canvas_na22_hitmap_ == NULL) {
        canvas_na22_hitmap_ = new TCanvas("canvas_na22_hitmap", "Source Event Count Map", 800, 800);
        canvas_na22_hitmap_->SetGrid();
        canvas_na22_hitmap_->ToggleEventStatus();
        canvas_na22_hitmap_->Connect("Closed()", "CommonCanvas", this, "CloseWindow()");
        canvas_na22_hitmap_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "CommonCanvas",
                this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    canvas_na22_hitmap_->cd();
    spec_count_map_->Draw("COLZ");
    for (int i = 0; i < 4; i++) {
        if (line_h_[i] != NULL)
            delete line_h_[i];
        line_h_[i] = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
        line_h_[i]->SetLineColor(kWhite);
        line_h_[i]->Draw("SAME");
        if (line_v_[i] != NULL)
            delete line_v_[i];
        line_v_[i] = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
        line_v_[i]->SetLineColor(kWhite);
        line_v_[i]->Draw("SAME");
    }
}

void CommonCanvas::draw_ce_fitting(int ct_idx) {
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);
    canvas_ce_fitting_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_ce_fitting"));
    if (canvas_ce_fitting_ == NULL) {
        canvas_ce_fitting_ = new TCanvas("canvas_ce_fitting", "Spectrum of one mod", 800, 600);
		canvas_ce_fitting_->SetFillColor(kYellow);
        canvas_ce_fitting_->Divide(8, 8);
    }
    canvas_ce_fitting_->SetTitle(Form("Spectrum of CT %02d", ct_idx + 1));
    for (int j = 0; j < 64; j++) {
        canvas_ce_fitting_->cd(jtoc(j));
		canvas_ce_fitting_->GetPad(jtoc(j))->SetFillColor(kWhite);
        spec_hist_[ct_idx][j]->Fit(spec_func_[ct_idx][j], "RQ");
    }
    canvas_ce_fitting_->Update();

}
