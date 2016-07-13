#ifndef CURVESHOW_H
#define CURVESHOW_H

#include <iostream>
#if !(defined(__ROOTCLING__) || defined(__CINT__))
#include "RootInc.hpp"
#endif /* __ROOTCLING__ || __CINT __ */
#include "RQ_OBJECT.h"

using namespace std;

class CurveShow {
    RQ_OBJECT("CurveShow");

public: // slots
    virtual void CloseWindow();

private:
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    TCanvas* canvas_curve_;
    TH1F*    cur_curve_hist_;
    TH2F*    cur_hit_map_hist_;
    TLine*   line_h_[4];
    TLine*   line_v_[4];
#endif /* __ROOTCLING__ || __CINT __ */

public:
    CurveShow();
    virtual ~CurveShow();

#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void set_curve_hist(TH1F* curve_hist);
    void set_hit_map_hist(TH2F* hit_map);
    void show_curve();
#endif /* __ROOTCLING__ || __CINT __ */
    
};

#endif
