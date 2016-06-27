#include "PedMeanShow.hpp"

using namespace std;

PedMeanShow::PedMeanShow() {

}

PedMeanShow::~PedMeanShow() {
    if (canvas_map_ != NULL) {
        delete canvas_map_;
        canvas_map_ = NULL;
    }
    if (canvas_mod_ != NULL) {
        delete canvas_mod_;
        canvas_mod_ = NULL;
    }
}

void PedMeanShow::show_map(PedMeanCalc& ped_mean_calc) {
    gStyle->SetOptStat(0);
    canvas_map_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_map"));
    if (canvas_map_ == NULL) {
        
    }
}

void PedMeanShow::show_mod(PedMeanCalc& ped_mean_calc, Int_t ct_idx) {

}

void PedMeanShow::CloseWindow() {

}

void PedMeanShow::ProcessAction(Int_t event, Int_t px, Int_t py, TObject* selected) {

}
