#include "CommonCanvas.hpp"

using namespace std;

CommonCanvas::CommonCanvas() {
    for (int i = 0; i < 25; i++) {
        xtalk_mat_ct_[i].ResizeTo(64, 64);
    }
    for (int i = 0; i < 4; i++) {
        line_h_[i] = NULL;
        line_v_[i] = NULL;
    }
    xtalk_map_all_ = new TH2F("xtalk_map_all", "Crosstalk Matrix of All Modules",
            320, 0, 320, 320, 0, 320);
    xtalk_map_all_->SetDirectory(NULL);
    for (int i = 0; i < 320; i++) {
        if (i % 64 == 0) {
            xtalk_map_all_->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            xtalk_map_all_->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    xtalk_map_mod_ = new TH2F("xtalk_map_mod", "Crosstalk Matrix Map of One Module",
                              64, 0, 64, 64, 0, 64);
    xtalk_map_mod_->SetDirectory(NULL);
    xtalk_map_mod_->GetXaxis()->SetNdivisions(64);
    xtalk_map_mod_->GetYaxis()->SetNdivisions(64);
    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0) {
            xtalk_map_mod_->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            xtalk_map_mod_->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
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
    canvas_xtalk_matrx_all_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_xtalk_matrx_all"));
    if (canvas_xtalk_matrx_all_ == NULL)
        return;
    int x = canvas_xtalk_matrx_all_->AbsPixeltoX(px);
    int y = canvas_xtalk_matrx_all_->AbsPixeltoY(py);
    draw_xtalk_map_sel_mod(x / 64 * 5 + 4 - y / 64);
}

void CommonCanvas::init(TMatrixF xtalk_mat[25]) {
    for (int i = 0; i < 25; i++) {
        xtalk_mat_ct_[i] = xtalk_mat[i];
    }
}

void CommonCanvas::draw_xtalk_map_all() {
    canvas_xtalk_matrx_all_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_xtalk_matrx_all"));
    if (canvas_xtalk_matrx_all_ == NULL) {
        canvas_xtalk_matrx_all_ = new TCanvas("canvas_xtalk_matrx_all", "Crosstalk Matrix Map of All Modules", 800, 800);
        canvas_xtalk_matrx_all_->ToggleEventStatus();
        canvas_xtalk_matrx_all_->Connect("Closed()", "CommonCanvas", this, "CloseWindow()");
        canvas_xtalk_matrx_all_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "CommonCanvas",
                                     this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    canvas_xtalk_matrx_all_->cd();
    gStyle->SetOptStat(0);
    for (int i = 0; i < 25; i++) {
        for (int jx = 0; jx < 64; jx++) {
            for (int jy = 0; jy < 64; jy++) {
                xtalk_map_all_->SetBinContent((i / 5) * 64 + jy + 1,
                                              (4 - i % 5) * 64 + (63 - jx) + 1,
                                              xtalk_mat_ct_[i](jx, jy));
            }
        }
    }
    xtalk_map_all_->Draw("COLZ");
    for (int i = 0; i < 4; i++) {
        if (line_h_[i] != NULL)
            delete line_h_[i];
        line_h_[i] = new TLine(0, (i + 1) * 64, 320, (i + 1) * 64);
        line_h_[i]->SetLineColor(kWhite);
        line_h_[i]->Draw("SAME");
        if (line_v_[i] != NULL)
            delete line_v_[i];
        line_v_[i] = new TLine((i + 1) * 64, 0, (i + 1) * 64, 320);
        line_v_[i]->SetLineColor(kWhite);
        line_v_[i]->Draw("SAME");
    }
    canvas_xtalk_matrx_all_->Update();
}

void CommonCanvas::draw_xtalk_map_sel_mod(int ct_idx) {
    canvas_xtalk_matrx_sel_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_xtalk_matrx_sel"));
    if (canvas_xtalk_matrx_sel_ == NULL) {
        canvas_xtalk_matrx_sel_ = new TCanvas("canvas_xtalk_matrx_sel", "Crosstalk Matrix Map of One Module", 800, 800);
        canvas_xtalk_matrx_sel_->SetGrid();
    }
    canvas_xtalk_matrx_sel_->SetTitle(Form("Crosstalk Matrix Map of CT_%02d", ct_idx + 1));
    canvas_xtalk_matrx_sel_->cd();
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            xtalk_map_mod_->SetBinContent(jy + 1, 64 - jx, xtalk_mat_ct_[ct_idx](jx, jy));
        }
    }
    xtalk_map_mod_->Draw("LEGO2");
    canvas_xtalk_matrx_sel_->Update();
}

