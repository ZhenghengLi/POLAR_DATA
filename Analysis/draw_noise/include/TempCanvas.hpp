#ifndef TempCanvas_H
#define TempCanvas_H

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

class TempCanvas {
    RQ_OBJECT("TempCanvas");

public: // slots
    virtual void CloseWindow();
    virtual void ProcessAction(Int_t event,
                               Int_t px,
                               Int_t py,
                               TObject* selected);

private:
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    TCanvas* canvas_single_;
    TCanvas* canvas_25_mod_;
    int      start_gps_week_;
    double   start_gps_second_;
#endif /* __ROOTCLING__ || __CINT __ */

public:
    TempCanvas(int week, double second);
    virtual ~TempCanvas();
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void cd_single();
    void cd_25_mod(int i);
#endif /* __ROOTCLING__ || __CINT __ */

};

#endif
