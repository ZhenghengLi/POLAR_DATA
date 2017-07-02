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
	canvas_ = static_cast<TCanvas*>(gROOT->FindObject("angle_quicklook"));
	if (canvas_ == NULL) {
		canvas_ = new TCanvas("angle_quicklook", "angle_quicklook", 900, 900);
		canvas_->Divide(2, 2);
		canvas_->Connect("Closed()", "CommonCanvas", this, "CloseWindow()");
	}
	canvas_->cd(i);
}

