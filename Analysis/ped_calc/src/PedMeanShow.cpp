#include "PedMeanShow.hpp"

using namespace std;

PedMeanShow::PedMeanShow() {
    cur_ped_mean_calc_ = NULL;
}

PedMeanShow::~PedMeanShow() {

}

void PedMeanShow::show_map(PedMeanCalc& ped_mean_calc) {
    cur_ped_mean_calc_ = &ped_mean_calc;    
    gStyle->SetOptStat(0);
    canvas_map_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_map"));
    if (canvas_map_ == NULL) {
        canvas_map_ = new TCanvas("canvas_map", "Pedestal Map of 1600 Channels", 800, 800);
        canvas_map_->SetGrid();
        canvas_map_->ToggleEventStatus();
        canvas_map_->Connect("Closed()", "PedMeanShow", this, "CloseWindow()");
        canvas_map_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "PedMeanShow",
                             this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    canvas_map_->cd();
    cur_ped_mean_calc_->draw_ped_map();
}

void PedMeanShow::show_mod(Int_t ct_idx) {
    if (cur_ped_mean_calc_ == NULL)
        return;
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);
    canvas_mod_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_mod"));
    if (canvas_mod_ == NULL) {
        canvas_mod_ = new TCanvas("canvas_mod", "Pedestal of one mod", 800, 600);
        canvas_mod_->Divide(8, 8);
    }
    canvas_mod_->SetTitle(Form("Pedestal of CT %02d", ct_idx + 1));
    for (int j = 0; j < 64; j++) {
        canvas_mod_->cd(jtoc(j));
        cur_ped_mean_calc_->draw_ped_hist(ct_idx, j);
    }
    canvas_mod_->Update();
}

void PedMeanShow::show_mean_sigma(PedMeanCalc& ped_mean_calc_) {
    gStyle->SetOptStat(0);
    canvas_mean_sigma_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_mean_sigma"));
    if (canvas_mean_sigma_ == NULL) {
        canvas_mean_sigma_ = new TCanvas("canvas_mean_sigma", "Pedestal Mean and Sigma of 1600 Channels", 1200, 600);
        canvas_mean_sigma_->Connect("Closed()", "PedMeanShow", this, "CloseWindow()");
        canvas_mean_sigma_->ToggleEventStatus();
        canvas_mean_sigma_->Divide(2, 1);
        canvas_mean_sigma_->GetPad(1)->SetGrid();
        canvas_mean_sigma_->GetPad(2)->SetGrid();
    }
    canvas_mean_sigma_->cd(1);
    ped_mean_calc_.draw_ped_mean();
    canvas_mean_sigma_->cd(2);
    ped_mean_calc_.draw_ped_sigma();
}

void PedMeanShow::CloseWindow() {
    if (cur_ped_mean_calc_ != NULL) {
        cur_ped_mean_calc_->delete_ped_hist();
        cur_ped_mean_calc_ = NULL;
    }
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void PedMeanShow::ProcessAction(Int_t event, Int_t px, Int_t py, TObject* selected) {
    if (event != kButton1Down)
        return;
    canvas_map_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_map"));
    if (canvas_map_ == NULL)
        return;
    int x = static_cast<int>(canvas_map_->AbsPixeltoX(px));
    int y = static_cast<int>(canvas_map_->AbsPixeltoY(py));
    show_mod(xytoi(x, y));
}

