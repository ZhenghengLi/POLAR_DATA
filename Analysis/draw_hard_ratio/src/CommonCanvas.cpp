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

void CommonCanvas::cd_spec(int i) {
	canvas_spec_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_spec"));
	if (canvas_spec_ == NULL) {
		canvas_spec_ = new TCanvas("canvas_spec", "deposited energy spectrum", 800, 600);
		canvas_spec_->Divide(1, 2);
		canvas_spec_->Connect("Closed()", "CommonCanvas", this, "CloseWindow()");
	}
	canvas_spec_->cd(i);
}

