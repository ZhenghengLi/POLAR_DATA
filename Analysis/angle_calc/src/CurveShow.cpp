#include "CurveShow.hpp"

using namespace std;

CurveShow::CurveShow() {
    cur_curve_hist_ = NULL;
}

CurveShow::~CurveShow() {

}

void CurveShow::CloseWindow() {
    if (cur_curve_hist_ != NULL) {
        delete cur_curve_hist_;
        cur_curve_hist_ = NULL;
    }
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void CurveShow::set_curve_hist(TH1F* curve_hist) {
    cur_curve_hist_ = curve_hist;
}

void CurveShow::show_curve() {
    if (cur_curve_hist_ == NULL)
        return;
    
}
