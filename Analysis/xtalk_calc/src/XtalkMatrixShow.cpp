#include "XtalkMatrixShow.hpp"

using namespace std;

XtalkMatrixShow::XtalkMatrixShow() {
    cur_xtalk_matrix_calc_ = NULL;
}

XtalkMatrixShow::~XtalkMatrixShow() {

}

void XtalkMatrixShow::CloseWindow() {
    if (cur_xtalk_matrix_calc_ != NULL) {
        cur_xtalk_matrix_calc_->delete_xtalk_hist();
        cur_xtalk_matrix_calc_ = NULL;
    }
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void XtalkMatrixShow::ProcessAction1(Int_t event, Int_t px, Int_t py, TObject* selected) {
    if (event != kButton1Down)
        return;
    canvas_mod_map_cur_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_mod_map_cur"));
    if (canvas_mod_map_cur_ == NULL)
        return;
    if (TString(selected->GetName()) == "xtalk_map_mod_2d") {
        int x = static_cast<int>(canvas_mod_map_cur_->GetPad(1)->AbsPixeltoX(px));
        int y = static_cast<int>(canvas_mod_map_cur_->GetPad(1)->AbsPixeltoY(py));
        show_jx_jy_line(x, 63 - y);
    }
}

void XtalkMatrixShow::ProcessAction2(Int_t event, Int_t px, Int_t py, TObject* selected) {
    if (event != kButton1Down)
        return;
    canvas_mod_map_all_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_mod_map_all"));
    if (canvas_mod_map_all_ == NULL)
        return;
    int x = canvas_mod_map_all_->AbsPixeltoX(px);
    int y = canvas_mod_map_all_->AbsPixeltoY(py);
    show_mod_map_sel(x / 64 * 5 + 4 - y / 64);
}

void XtalkMatrixShow::show_mod_map_cur(XtalkMatrixCalc& xtalk_matrix_calc) {
    cur_xtalk_matrix_calc_ = &xtalk_matrix_calc;
    gStyle->SetOptStat(0);
    canvas_mod_map_cur_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_mod_map_cur"));
    if (canvas_mod_map_cur_ == NULL) {
        canvas_mod_map_cur_ = new TCanvas("canvas_mod_map_cur", "Crosstalk Matrix Map of one Module", 1200, 600);
        canvas_mod_map_cur_->SetGrid();
        canvas_mod_map_cur_->ToggleEventStatus();
        canvas_mod_map_cur_->Connect("Closed()", "XtalkMatrixShow", this, "CloseWindow()");
        canvas_mod_map_cur_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "XtalkMatrixShow",
                                     this, "ProcessAction1(Int_t, Int_t, Int_t, TObject*)");
        canvas_mod_map_cur_->Divide(2, 1);
        canvas_mod_map_cur_->GetPad(1)->SetGrid();
    }
    canvas_mod_map_cur_->SetTitle(Form("Crosstalk Matrix Map of CT_%02d",
                                       xtalk_matrix_calc.get_current_ct_idx() + 1));
    canvas_mod_map_cur_->cd(1);
    xtalk_matrix_calc.draw_xtalk_map_cur_mod_2d();
    canvas_mod_map_cur_->cd(2);
    xtalk_matrix_calc.draw_xtalk_map_cur_mod_3d();
    canvas_mod_map_cur_->Update();
}

void XtalkMatrixShow::show_jx_jy_line(int jx, int jy) {
    if (cur_xtalk_matrix_calc_ == NULL)
        return;
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);
    canvas_jx_jy_line_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_jx_jy_line"));
    if (canvas_jx_jy_line_ == NULL) {
        canvas_jx_jy_line_ = new TCanvas("canvas_jx_jy_line", "Crosstalk fitting of CT_i_jx => CT_i_jy", 800, 600);
        canvas_jx_jy_line_->SetGridy();
    }
    canvas_jx_jy_line_->SetTitle(Form("Crosstalk fitting of CT_%02d_%02d => CT_%02d_%02d",
                                      cur_xtalk_matrix_calc_->get_current_ct_idx() + 1,
                                      jx + 1,
                                      cur_xtalk_matrix_calc_->get_current_ct_idx() + 1,
                                      jy + 1));
    canvas_jx_jy_line_->cd();
    cur_xtalk_matrix_calc_->draw_xtalk_line(jx, jy);
    canvas_jx_jy_line_->Update();
}

void XtalkMatrixShow::show_mod_map_all(XtalkMatrixCalc& xtalk_matrix_calc) {
    cur_xtalk_matrix_calc_ = &xtalk_matrix_calc;
    gStyle->SetOptStat(0);
    canvas_mod_map_all_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_mod_map_all"));
    if (canvas_mod_map_all_ == NULL) {
        canvas_mod_map_all_ = new TCanvas("canvas_mod_map_all", "Crosstalk Matrix Map of All Modules", 1000, 1000);
        canvas_mod_map_all_->SetGrid();
        canvas_mod_map_all_->ToggleEventStatus();
        canvas_mod_map_all_->Connect("Closed()", "XtalkMatrixShow", this, "CloseWindow()");
        canvas_mod_map_all_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "XtalkMatrixShow",
                                     this, "ProcessAction2(Int_t, Int_t, Int_t, TObject*)");
        canvas_mod_map_cur_->SetGrid();
    }
    canvas_mod_map_all_->cd();
    xtalk_matrix_calc.draw_xtalk_map_all();
    canvas_mod_map_cur_->Update();
}

void XtalkMatrixShow::show_mod_map_sel(int ct_idx) {
    if (cur_xtalk_matrix_calc_ == NULL)
        return;
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);
    canvas_mod_map_sel_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_mod_map_sel"));
    if (canvas_mod_map_sel_ == NULL) {
        canvas_mod_map_sel_ = new TCanvas("canvas_mod_map_sel", "Crosstalk Matrix Map of One Module", 800, 800);
        canvas_mod_map_sel_->SetGrid();
    }
    canvas_mod_map_sel_->SetTitle(Form("Crosstalk Matrix Map of CT_%02d", ct_idx + 1));
    canvas_mod_map_sel_->cd();
    cur_xtalk_matrix_calc_->draw_xtalk_map_sel_mod(ct_idx);
    canvas_mod_map_sel_->Update();
}
