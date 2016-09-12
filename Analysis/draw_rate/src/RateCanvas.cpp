#include "RateCanvas.hpp"

using namespace std;

RateCanvas::RateCanvas(int week, double second) {
    start_gps_week_   = week;
    start_gps_second_ = second;
    select_count_     = 0;
}

RateCanvas::~RateCanvas() {

}

void RateCanvas::cd_trigger() {
    canvas_trigger_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_trigger"));
    if (canvas_trigger_ == NULL) {
        canvas_trigger_ = new TCanvas("canvas_trigger", "rate of event trigger", 1000, 800);
        canvas_trigger_->ToggleEventStatus();
        canvas_trigger_->SetCrosshair();
        canvas_trigger_->Connect("Closed()", "RateCanvas", this, "CloseWindow()");
        canvas_trigger_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "RateCanvas",
                                 this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    canvas_trigger_->cd();
}

void RateCanvas::cd_modules(int i) {
    canvas_modules_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_modules"));
    if (canvas_modules_ == NULL) {
        canvas_modules_ = new TCanvas("canvas_modules", "rate of 25 modules", 1500, 1000);
        canvas_modules_->ToggleEventStatus();
        canvas_modules_->Divide(5, 5);
    }
    canvas_modules_->cd(itoc(i));
}

void RateCanvas::CloseWindow() {
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void RateCanvas::ProcessAction(Int_t event, Int_t px, Int_t py, TObject* selected) {
    if (event == kKeyPress) {
        cout << " -*-*-*-*-*-*-*-*-*-*-*-*- " << endl;
        return;
    }
    if (event != kButton1Down)
        return;
    canvas_trigger_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_trigger"));
    if (canvas_trigger_ == NULL)
        return;
    int x = static_cast<int>(canvas_trigger_->AbsPixeltoX(px));
    select_count_ += 1;
    int tmp_gps_second = static_cast<int>(start_gps_second_ + x);
    cout << " - " << setw(2) << select_count_ << " -> " << start_gps_week_ + tmp_gps_second / 604800 << ":" << tmp_gps_second % 604800 << endl;
}
