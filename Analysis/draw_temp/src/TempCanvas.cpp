#include "TempCanvas.hpp"

using namespace std;

TempCanvas::TempCanvas(int week, double second) {
    start_gps_week_   = week;
    start_gps_second_ = second;
}

TempCanvas::~TempCanvas() {

}

void TempCanvas::cd_single() {
    canvas_single_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_single"));
    if (canvas_single_ == NULL) {
        canvas_single_ = new TCanvas("canvas_single", "temperature of seleceted modules", 1000, 600);
        canvas_single_->ToggleEventStatus();
        canvas_single_->SetCrosshair();
        canvas_single_->Connect("Closed()", "TempCanvas", this, "CloseWindow()");
        canvas_single_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "TempCanvas",
                                 this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    canvas_single_->cd();
}

void TempCanvas::cd_25_mod(int i) {
    canvas_25_mod_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_25_mod"));
    if (canvas_25_mod_ == NULL) {
        canvas_25_mod_ = new TCanvas("canvas_25_mod", "temperature of 25 modules", 1500, 1000);
        canvas_25_mod_->ToggleEventStatus();
        canvas_25_mod_->Connect("Closed()", "TempCanvas", this, "CloseWindow()");
        canvas_25_mod_->Divide(5, 5);
    }
    canvas_25_mod_->cd(itoc(i));
}

void TempCanvas::CloseWindow() {
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void TempCanvas::ProcessAction(Int_t event, Int_t px, Int_t py, TObject* selected) {

}
