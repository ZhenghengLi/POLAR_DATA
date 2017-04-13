#include "CommonCanvas.hpp"

using namespace std;

CommonCanvas::CommonCanvas() {

}

CommonCanvas::~CommonCanvas() {

}

void CommonCanvas::CloseWindow() {
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void CommonCanvas::ProcessAction(Int_t event, Int_t px, Int_t py, TObject* selected) {

}

void CommonCanvas::cd(int i) {
	canvas_noise_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_noise"));
	if (canvas_noise_ == NULL) {
		canvas_noise_ = new TCanvas("canvas_noise", "pedestal and noise", 900, 900);
		canvas_noise_->Divide(2, 2);
		canvas_noise_->Connect("Closed()", "CommonCanvas", this, "CloseWindow()");
	}
	canvas_noise_->cd(i);
}

