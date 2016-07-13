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
#endif /* __ROOTCLING__ || __CINT __ */

public:
    CurveShow();
    virtual ~CurveShow();

#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void set_curve_hist(TH1F* curve_hist);
    void show_curve();
#endif /* __ROOTCLING__ || __CINT __ */
    
};

#endif
