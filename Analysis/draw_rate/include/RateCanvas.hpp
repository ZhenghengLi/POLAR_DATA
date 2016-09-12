#ifndef RATECANVAS_H
#define RATECANVAS_H

#include <iostream>
#include <iomanip>
#if !(defined(__ROOTCLING__) || defined(__CINT__))
#include "RootInc.hpp"
#include "CooConv.hpp"
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
    int      start_gps_week_;
    double   start_gps_second_;
    int      select_count_;
#endif /* __ROOTCLING__ || __CINT __ */
    
public:
    RateCanvas(int week, double second);
    virtual ~RateCanvas();
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void cd_trigger();
    void cd_modules(int i);
#endif /* __ROOTCLING__ || __CINT __ */
    
};

#endif
