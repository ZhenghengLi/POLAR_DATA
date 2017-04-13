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
	TCanvas* canvas_xtalk_matrx_all_;
	TCanvas* canvas_xtalk_matrx_sel_;
    TMatrixF xtalk_mat_ct_[25];
    TH2F*    xtalk_map_all_;
    TH2F*    xtalk_map_mod_;
    TLine*   line_h_[4];
    TLine*   line_v_[4];
#endif /* __ROOTCLING__ || __CINT __ */

public:
    CommonCanvas();
    virtual ~CommonCanvas();
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void init(TMatrixF xtalk_mat[25]);
    void draw_xtalk_map_all();
    void draw_xtalk_map_sel_mod(int ct_idx);
#endif /* __ROOTCLING__ || __CINT __ */

};

#endif
