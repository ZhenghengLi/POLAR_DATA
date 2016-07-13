#include "CurveShow.hpp"

using namespace std;

CurveShow::CurveShow() {
    cur_curve_hist_   = NULL;
    cur_hit_map_hist_ = NULL;
    for (int i = 0; i < 4; i++) {
        line_v_[i] = NULL;
        line_h_[i] = NULL;
    }
}

CurveShow::~CurveShow() {

}

void CurveShow::CloseWindow() {
    if (cur_curve_hist_ != NULL) {
        delete cur_curve_hist_;
        cur_curve_hist_ = NULL;
    }
    if (cur_hit_map_hist_ != NULL) {
        delete cur_hit_map_hist_;
        cur_hit_map_hist_ = NULL;
    }
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void CurveShow::set_curve_hist(TH1F* curve_hist) {
    cur_curve_hist_ = curve_hist;
}

void CurveShow::set_hit_map_hist(TH2F* hit_map_hist) {
    cur_hit_map_hist_ = hit_map_hist;
}

void CurveShow::show_curve() {
    if (cur_curve_hist_ == NULL)
        return;
    if (cur_hit_map_hist_ == NULL)
        return;
    gStyle->SetOptStat(0);
    canvas_curve_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_curve"));
    if (canvas_curve_ == NULL) {
        canvas_curve_ = new TCanvas("canvas_curve", "Modulation Curve and Hit Map", 1200, 600);
        canvas_curve_->Connect("Closed()", "CurveShow", this, "CloseWindow()");
        canvas_curve_->ToggleEventStatus();
        canvas_curve_->Divide(2, 1);
        canvas_curve_->GetPad(1)->SetGridy();
        canvas_curve_->GetPad(2)->SetGrid();
    }
    canvas_curve_->cd(1);
    cur_curve_hist_->Draw();
    canvas_curve_->cd(2);
    cur_hit_map_hist_->Draw("COLZ");
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
