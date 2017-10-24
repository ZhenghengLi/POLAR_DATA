#include "CommonCanvas.hpp"

using namespace std;

CommonCanvas::CommonCanvas(int ct_n) {
    ct_num_ = ct_n;
    xtalk_matrix_.ResizeTo(64, 64);
    xtalk_map_mod_2d_ = new TH2F("xtalk_map_mod_2d",
            Form("Crosstalk Matrix 2D Map of CT_%02d", ct_num_),
            64, 0, 64, 64, 0, 64);
    xtalk_map_mod_2d_->SetDirectory(NULL);
    xtalk_map_mod_2d_->GetXaxis()->SetNdivisions(64);
    xtalk_map_mod_2d_->GetYaxis()->SetNdivisions(64);
    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0) {
            xtalk_map_mod_2d_->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            xtalk_map_mod_2d_->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    xtalk_map_mod_3d_ = new TH2F("xtalk_map_mod_3d",
            Form("Crosstalk Matrix 3D Map of CT_%02d", ct_num_),
            64, 0, 64, 64, 0, 64);
    xtalk_map_mod_3d_->SetDirectory(NULL);
    xtalk_map_mod_3d_->GetXaxis()->SetNdivisions(64);
    xtalk_map_mod_3d_->GetYaxis()->SetNdivisions(64);
    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0) {
            xtalk_map_mod_3d_->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            xtalk_map_mod_3d_->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
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
    canvas_xtalk_matrix_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_xtalk_matrix"));
    if (canvas_xtalk_matrix_ == NULL)
        return;
    if (TString(selected->GetName()) == "xtalk_map_mod_2d") {
        int x = static_cast<int>(canvas_xtalk_matrix_->GetPad(1)->AbsPixeltoX(px));
        int y = static_cast<int>(canvas_xtalk_matrix_->GetPad(1)->AbsPixeltoY(py));
        draw_xtalk_fitting(x, 63 - y);
    }

}

void CommonCanvas::init(TMatrixF xtalk_mat, TH2F* xtalk_h[64][64], TF1* xtalk_l[64][64]) {
    xtalk_matrix_ = xtalk_mat;
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            xtalk_hist_[jx][jy] = xtalk_h[jx][jy];
            xtalk_line_[jx][jy] = xtalk_l[jx][jy];
        }
    }
}

void CommonCanvas::draw_xtalk_matrix() {
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            xtalk_map_mod_2d_->SetBinContent(jy + 1, 64 - jx, xtalk_matrix_(jx, jy));
            xtalk_map_mod_3d_->SetBinContent(jy + 1, 64 - jx, xtalk_matrix_(jx, jy));
        }
    }
    canvas_xtalk_matrix_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_xtalk_matrix"));
    if (canvas_xtalk_matrix_ == NULL) {
        canvas_xtalk_matrix_ = new TCanvas("canvas_xtalk_matrix", "Crosstalk Matrix Map of one Module", 1200, 600);
        canvas_xtalk_matrix_->SetGrid();
        canvas_xtalk_matrix_->ToggleEventStatus();
        canvas_xtalk_matrix_->Connect("Closed()", "CommonCanvas", this, "CloseWindow()");
        canvas_xtalk_matrix_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "CommonCanvas",
                                     this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
        canvas_xtalk_matrix_->Divide(2, 1);
        canvas_xtalk_matrix_->GetPad(1)->SetGrid();
    }
    canvas_xtalk_matrix_->SetTitle(Form("Crosstalk Matrix Map of CT_%02d", ct_num_));
    gStyle->SetOptStat(0);
    canvas_xtalk_matrix_->cd(1);
    xtalk_map_mod_2d_->Draw("colz");
    canvas_xtalk_matrix_->cd(2);
    xtalk_map_mod_3d_->Draw("lego2");
    canvas_xtalk_matrix_->Update();
}

void CommonCanvas::draw_xtalk_fitting(int jx, int jy) {
    if (jx == jy) return;
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);
    canvas_xtalk_fitting_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_xtalk_fitting_"));
    if (canvas_xtalk_fitting_ == NULL) {
        canvas_xtalk_fitting_ = new TCanvas("canvas_xtalk_fitting_", "Crosstalk fitting of CT_i_jx => CT_i_jy", 800, 600);
        canvas_xtalk_fitting_->SetGridy();
    }
    canvas_xtalk_fitting_->SetTitle(Form("Crosstalk fitting of CT_%02d_%02d => CT_%02d_%02d",
                ct_num_, jx + 1, ct_num_, jy + 1));
    canvas_xtalk_fitting_->cd();
    xtalk_hist_[jx][jy]->Draw("colz");
    xtalk_hist_[jx][jy]->Fit(xtalk_line_[jx][jy], "RQ");
    canvas_xtalk_fitting_->Update();
}
