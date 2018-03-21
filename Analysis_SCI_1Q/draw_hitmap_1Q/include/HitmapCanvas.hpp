#ifndef HitmapCanvas_H
#define HitmapCanvas_H

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

class HitmapCanvas {
    RQ_OBJECT("HitmapCanvas");

public: // slots
    virtual void CloseWindow();
    virtual void ProcessAction(Int_t event,
                               Int_t px,
                               Int_t py,
                               TObject* selected);

private:
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    TCanvas* canvas_count_;
    TH2D* count_map_;
    TLine* line_h_[4];
    TLine* line_v_[4];
#endif /* __ROOTCLING__ || __CINT __ */

public:
    HitmapCanvas();
    virtual ~HitmapCanvas();
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void set_count_map(int ch_count[25][64]);
    void draw_count_map();
#endif /* __ROOTCLING__ || __CINT __ */

};

#endif
