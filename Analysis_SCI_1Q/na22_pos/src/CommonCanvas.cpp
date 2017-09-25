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

void CommonCanvas::cd() {
	canvas_ = static_cast<TCanvas*>(gROOT->FindObject("canvas"));
	if (canvas_ == NULL) {
		canvas_ = new TCanvas("canvas", "canvas", 900, 900);
		canvas_->Connect("Closed()", "CommonCanvas", this, "CloseWindow()");
	}
	canvas_->cd();
}

