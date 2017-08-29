#include "SpectrumCanvas.hpp"
#include "CooConv.hpp"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstdio>

using namespace std;

SpectrumCanvas::SpectrumCanvas() {
    canvas_count_ = NULL;
    canvas_spec_ = NULL;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            tri_spec_[i][j] = NULL;
        }
    }
    mod_count_map_ = new TH2D("mod_count_hist", "Module Counts", 5, 0, 5, 5, 0, 5);
    mod_count_map_->SetDirectory(NULL);
    bar_count_map_ = new TH2D("bar_count_hist", "Channel Counts", 40, 0, 40, 40, 0, 40);
    bar_count_map_->SetDirectory(NULL);
}

void SpectrumCanvas::set_tri_spec(TH1D* tri_spec_par[25][64]) {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            tri_spec_[i][j] = tri_spec_par[i][j];
        }
    }
}


void SpectrumCanvas::set_count_map(int mod_count[25], int bar_count[25][64]) {
    for (int i = 0; i < 25; i++) {
        int x = i / 5 + 1;
        int y = 5 - i % 5;
        mod_count_map_->SetBinContent(x, y, mod_count[i]);
        for (int j = 0; j < 64; j++) {
            bar_count_map_->SetBinContent(ijtox(i, j), ijtoy(i, j), bar_count[i][j]);
        }
    }
}

SpectrumCanvas::~SpectrumCanvas() {
    delete mod_count_map_;
    mod_count_map_ = NULL;
    delete bar_count_map_;
    bar_count_map_ = NULL;
}

void SpectrumCanvas::CloseWindow() {
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void SpectrumCanvas::ProcessAction(Int_t event, Int_t px, Int_t py, TObject* selected) {
    if (event != kButton1Down)
        return;
    canvas_count_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_count"));
    if (canvas_count_ == NULL)
        return;
    if (TString(selected->GetName()) == "mod_count_hist") {
        int x = static_cast<int>(canvas_count_->GetPad(1)->AbsPixeltoX(px));
        int y = static_cast<int>(canvas_count_->GetPad(1)->AbsPixeltoY(py));
        int ct_num = x * 5 + 5 - y;
        draw_spec_(ct_num);
    }
}

void SpectrumCanvas::draw_count_map() {
    canvas_count_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_count"));
    if (canvas_count_ == NULL) {
        canvas_count_ = new TCanvas("canvas_count", "total trigger counts of 25 modules", 1200, 600);
        canvas_count_->Divide(2, 1);
        canvas_count_->Connect("Closed()", "SpectrumCanvas", this, "CloseWindow()");
        canvas_count_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "SpectrumCanvas",
                               this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    canvas_count_->cd(1);
    mod_count_map_->Draw("colz");
    canvas_count_->cd(2);
    bar_count_map_->Draw("colz");

}

void SpectrumCanvas::draw_spec_(int ct_num) {
    if (ct_num < 1 || ct_num > 25) return;
    int ct_idx = ct_num - 1;
    canvas_spec_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_spec"));
    if (canvas_spec_ == NULL) {
        canvas_spec_ = new TCanvas("canvas_spec", "Spectrum", 1000, 800);
        canvas_spec_->SetFillColor(kYellow);
        canvas_spec_->Divide(8, 8);
        for (int j = 0; j < 64; j++) {
            // canvas_spec_->GetPad(j + 1)->SetLogy();
            canvas_spec_->GetPad(j + 1)->SetFillColor(kWhite);
        }
    }
    for (int j = 0; j < 64; j++) {
        canvas_spec_->cd(jtoc(j));
        tri_spec_[ct_idx][j]->Draw("h");
    }
    canvas_spec_->SetTitle(Form("Spectrom of CT_%02d", ct_idx + 1));
    canvas_spec_->Update();

}

