#ifndef RATECANVAS_H
#define RATECANVAS_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#if !(defined(__ROOTCLING__) || defined(__CINT__))
#include "RootInc.hpp"
#include "CooConv.hpp"
#include "TGaxis.h"
#endif /* __ROOTCLING__ || __CINT __ */
#include "RQ_OBJECT.h"

using namespace std;

class RateCanvas {
    RQ_OBJECT("RateCanvas");

public: // slots
    virtual void CloseWindow();
    virtual void ProcessAction(Int_t event,
                               Int_t px,
                               Int_t py,
                               TObject* selected);
    
private:
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    TCanvas* canvas_trigger_;
    TCanvas* canvas_modules_;
    TH1D*    cur_hist_int_;
    TLine*   line_obj_[5];
    TLine*   line_t90_[2];
    int      line_cnt_;
    int      start_gps_week_;
    double   start_gps_second_;
    int      select_count_;
    double   select_x_[5];
    int      select_i_[5];
    double   cur_scale_;
    TF1*     fun_before_;
    TF1*     fun_after_;
    TLine*   line_before_;
    TLine*   line_before_05_;
    TLine*   line_after_;
    TLine*   line_after_05_;
    bool     is_fitted_;
#endif /* __ROOTCLING__ || __CINT __ */
    
public:
    RateCanvas(int week, double second);
    virtual ~RateCanvas();
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void cd_trigger();
    void cd_modules(int i);
    void draw_hist_int(TH1D* hist_int);
#endif /* __ROOTCLING__ || __CINT __ */
    
};

#endif
