#include "SpectrumShow.hpp"

using namespace std;

SpectrumShow::SpectrumShow() {
    cur_compton_edge_calc_ = NULL;
}

SpectrumShow::~SpectrumShow() {

}

void SpectrumShow::CloseWindow() {
    if (true) {

    }
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void SpectrumShow::ProcessAction(Int_t event, Int_t px, Int_t py, TObject* selected) {
    if (event != kButton1Down)
        return;
    canvas_map_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_map"));
    if (canvas_map_ == NULL)
        return;
    int x = static_cast<int>(canvas_map_->AbsPixeltoX(px));
    int y = static_cast<int>(canvas_map_->AbsPixeltoY(py));
    show_mod(xytoi(x, y));
}

void SpectrumShow::show_map(ComptonEdgeCalc& compton_edge_calc) {
    cur_compton_edge_calc_ = &compton_edge_calc;
    gStyle->SetOptStat(0);
    canvas_map_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_map"));
    if (canvas_map_ == NULL) {
        canvas_map_ = new TCanvas("canvas_map", "Source Event Count Map", 800, 800);
        canvas_map_->SetGrid();
        canvas_map_->ToggleEventStatus();
        canvas_map_->Connect("Closed()", "SpectrumShow", this, "CloseWindow()");
        canvas_map_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "SpectrumShow",
                             this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    canvas_map_->cd();
    cur_compton_edge_calc_->draw_spec_count_map();
}

void SpectrumShow::show_mod(Int_t ct_idx) {
    if (cur_compton_edge_calc_ == NULL)
        return;
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);
    canvas_mod_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_mod"));
    if (canvas_mod_ == NULL) {
        canvas_mod_ = new TCanvas("canvas_mod", "Spectrum of one mod", 800, 800);
        canvas_mod_->Divide(8, 8);
    }
    canvas_mod_->SetTitle(Form("Spectrum of CT %02d", ct_idx + 1));
    for (int j = 0; j < 64; j++) {
        canvas_mod_->cd(jtoc(j));
        canvas_mod_->GetPad(jtoc(j))->SetLogy();
        cur_compton_edge_calc_->draw_spec_hist(ct_idx, j);
    }
    canvas_mod_->Update();
}
