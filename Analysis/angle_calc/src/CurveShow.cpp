#include "CurveShow.hpp"

using namespace std;

CurveShow::CurveShow() {
    cur_curve_hist_ = NULL;
    cur_hit_map_    = NULL;
}

CurveShow::~CurveShow() {

}

void CurveShow::CloseWindow() {
    if (cur_curve_hist_ != NULL) {
        delete cur_curve_hist_;
        cur_curve_hist_ = NULL;
    }
    if (cur_hit_map_ != NULL) {
        delete cur_hit_map_;
        cur_hit_map_ = NULL;
    }
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void CurveShow::set_curve_hist(TH1F* curve_hist) {
    cur_curve_hist_ = curve_hist;
}

void CurveShow::set_hit_map(TH2F* hit_map) {
    cur_hit_map_ = hit_map;
}

void CurveShow::show_curve() {
    if (cur_curve_hist_ == NULL)
        return;
    if (cur_hit_map_ == NULL)
        return;
    
}
