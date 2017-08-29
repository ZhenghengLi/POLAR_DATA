#ifndef SpectrumCanvas_H
#define SpectrumCanvas_H

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

class SpectrumCanvas {
    RQ_OBJECT("SpectrumCanvas");

public: // slots
    virtual void CloseWindow();
    virtual void ProcessAction(Int_t event,
                               Int_t px,
                               Int_t py,
                               TObject* selected);

private:
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    TCanvas* canvas_count_;
    TCanvas* canvas_spec_;
    TH1D* tri_spec_[25][64];
    TH2D* mod_count_map_;
    TH2D* bar_count_map_;
    // function
    void draw_spec_(int ct_num);
#endif /* __ROOTCLING__ || __CINT __ */

public:
    SpectrumCanvas();
    virtual ~SpectrumCanvas();
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void set_tri_spec(TH1D* tri_spec_par[25][64]);
    void set_count_map(int mod_count[25], int bar_count[25][64]);
    void draw_count_map();
#endif /* __ROOTCLING__ || __CINT __ */

};

#endif
