#include "SpectrumCanvas.hpp"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstdio>

using namespace std;

SpectrumCanvas::SpectrumCanvas() {
    canvas_count_ = NULL;
    canvas_spec_ = NULL;
    canvas_eff_ = NULL;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            tri_spec_[i][j] = NULL;
            all_spec_[i][j] = NULL;
            ped_spec_[i][j] = NULL;
            fun_spec_[i][j] = NULL;
            tri_eff_[i][j] = NULL;
        }
    }
    count_map_ = new TH2D("count_hist", "Module Counts", 5, 0, 5, 5, 0, 5);
    count_map_->SetDirectory(NULL);
}

void SpectrumCanvas::set_tri_spec(TH1D* tri_spec_par[25][64]) {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            tri_spec_[i][j] = tri_spec_par[i][j];
        }
    }
}

void SpectrumCanvas::set_all_spec(TH1D* all_spec_par[25][64]) {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            all_spec_[i][j] = all_spec_par[i][j];
        }
    }
}

void SpectrumCanvas::set_ped_spec(TH1D* ped_spec_par[25][64]) {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ped_spec_[i][j] = ped_spec_par[i][j];
        }
    }
}

void SpectrumCanvas::set_fun_spec(TF1* fun_spec_par[25][64]) {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            fun_spec_[i][j] = fun_spec_par[i][j];
        }
    }
}

void SpectrumCanvas::set_tri_eff(TH1D* tri_eff_par[25][64]) {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            tri_eff_[i][j] = tri_eff_par[i][j];
        }
    }
}

void SpectrumCanvas::set_count_map(int mod_count[25]) {
    for (int i = 0; i < 25; i++) {
        int x = i / 5 + 1;
        int y = 5 - i % 5;
        count_map_->SetBinContent(x, y, mod_count[i]);
    }
}

SpectrumCanvas::~SpectrumCanvas() {
    delete count_map_;
    count_map_ = NULL;
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
    int x = static_cast<int>(canvas_count_->AbsPixeltoX(px));
    int y = static_cast<int>(canvas_count_->AbsPixeltoY(py));
    int ct_num = x * 5 + 5 - y;
    draw_spec_(ct_num);
    draw_eff_(ct_num);
}

void SpectrumCanvas::draw_count_map() {
    canvas_count_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_count"));
    if (canvas_count_ == NULL) {
        canvas_count_ = new TCanvas("canvas_count", "total trigger counts of 25 modules", 600, 600);
        canvas_count_->Connect("Closed()", "SpectrumCanvas", this, "CloseWindow()");
        canvas_count_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "SpectrumCanvas",
                               this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    canvas_count_->cd();
    count_map_->Draw("colz");

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
            canvas_spec_->GetPad(j + 1)->SetLogy();
            canvas_spec_->GetPad(j + 1)->SetFillColor(kWhite);
        }
    }
    for (int j = 0; j < 64; j++) {
        canvas_spec_->cd(jtoc(j));
        all_spec_[ct_idx][j]->Draw("h");
        tri_spec_[ct_idx][j]->Draw("h same");
        ped_spec_[ct_idx][j]->Draw("h same");
    }
    canvas_spec_->SetTitle(Form("Spectrom of CT_%02d", ct_idx + 1));
    canvas_spec_->Update();

}

void SpectrumCanvas::draw_eff_(int ct_num) {
    if (ct_num < 1 || ct_num > 25) return;
    int ct_idx = ct_num - 1;
    canvas_eff_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_eff"));
    if (canvas_eff_ == NULL) {
        canvas_eff_ = new TCanvas("canvas_eff", "Efficiency", 1000, 800);
        canvas_eff_->SetFillColor(kYellow);
        canvas_eff_->Divide(8, 8);
        for (int j = 0; j < 64; j++) {
            canvas_eff_->GetPad(j + 1)->SetFillColor(kWhite);
        }
    }
    for (int j = 0; j < 64; j++) {
        canvas_eff_->cd(jtoc(j));
        for (int k = 1; k < tri_eff_[ct_idx][j]->GetNbinsX(); k++) {
            if (tri_eff_[ct_idx][j]->GetBinContent(k) > 0.5) {
                fun_spec_[ct_idx][j]->SetParameter(0, tri_eff_[ct_idx][j]->GetBinCenter(k));
                break;
            }
        }
        tri_eff_[ct_idx][j]->Fit(fun_spec_[ct_idx][j], "RQ");
    }
    canvas_eff_->SetTitle(Form("Efficiency of CT_%02d", ct_idx + 1));
    canvas_eff_->Update();
}

