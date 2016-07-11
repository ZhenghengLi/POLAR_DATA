#ifndef SPECTRUMSHOW_H
#define SPECTRUMSHOW_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#if !(defined(__ROOTCLING__) || defined(__CINT__))
#include "ComptonEdgeCalc.hpp"
#include "CooConv.hpp"
#include "RootInc.hpp"
#endif /* __ROOTCLING__ || __CINT __ */
#include "RQ_OBJECT.h"

using namespace std;

class SpectrumShow {
    RQ_OBJECT("SpectrumShow");

public: // slots
    virtual void CloseWindow();
    virtual void ProcessAction(Int_t event,
                               Int_t px,
                               Int_t py,
                               TObject* selected);

private:
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    TCanvas* canvas_map_;
    TCanvas* canvas_mod_;
    TCanvas* canvas_adc_per_kev_;
    ComptonEdgeCalc* cur_compton_edge_calc_;
#endif /* __ROOTCLING__ || __CINT __ */

public:
    SpectrumShow();
    virtual ~SpectrumShow();

#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void show_map(ComptonEdgeCalc& compton_edge_calc);
    void show_mod(Int_t ct_idx);
    void show_adc_per_kev(ComptonEdgeCalc& compton_edge_calc);
#endif /* __ROOTCLING__ || __CINT __ */
    
};

#endif
