#ifndef COMMONCANVAS_H
#define COMMONCANVAS_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#if !(defined(__ROOTCLING__) || defined(__CINT__))
#include "RootInc.hpp"
#include "CooConv.hpp"
#endif /* __ROOTCLING__ || __CINT __ */
#include "RQ_OBJECT.h"

using namespace std;

class CommonCanvas {
    RQ_OBJECT("CommonCanvas");

public: // slots
    virtual void CloseWindow();
    virtual void ProcessAction(Int_t event,
                               Int_t px,
                               Int_t py,
                               TObject* selected);

private:
#if !(defined(__ROOTCLING__) || defined(__CINT__))
	TCanvas* canvas_na22_hitmap_;
    TCanvas* canvas_ce_fitting_;
    TF1*     spec_func_[25][64];
    TH1F*    spec_hist_[25][64];
    TH2F*    spec_count_map_;
    TLine*   line_h_[4];
    TLine*   line_v_[4];

#endif /* __ROOTCLING__ || __CINT __ */

public:
    CommonCanvas();
    virtual ~CommonCanvas();
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void init(TH2F* spec_cnt_map, TH1F* spec_h[25][64], TF1* spec_f[25][64]);
    void draw_na22_hitmap();
    void draw_ce_fitting(int ct_idx);
#endif /* __ROOTCLING__ || __CINT __ */

};

#endif
