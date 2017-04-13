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
	TCanvas* canvas_xtalk_matrix_;
    TCanvas* canvas_xtalk_fitting_;
    int      ct_num_;
    TMatrixF xtalk_matrix_;
    TF1*     xtalk_line_[64][64];
    TH2F*    xtalk_hist_[64][64];
    TH2F*    xtalk_map_mod_2d_;
    TH2F*    xtalk_map_mod_3d_;

#endif /* __ROOTCLING__ || __CINT __ */

public:
    CommonCanvas(int ct_n = 1);
    virtual ~CommonCanvas();
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void init(TMatrixF xtalk_mat, TH2F* xtalk_h[64][64], TF1* xtalk_l[64][64]);
    void draw_xtalk_matrix();
    void draw_xtalk_fitting(int jx, int jy);
#endif /* __ROOTCLING__ || __CINT __ */

};

#endif
