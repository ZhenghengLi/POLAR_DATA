#ifndef PEDMEANSHOW_H
#define PEDMEANSHOW_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#if !(defined(__ROOTCLING__) || defined(__CINT__))
#include "PedMeanCalc.hpp"
#include "CooConv.hpp"
#include "RootInc.hpp"
#endif /* __ROOTCLING__ || __CINT __ */
#include "RQ_OBJECT.h"

using namespace std;

class PedMeanShow {
    RQ_OBJECT("PedMeanShow");

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
    TCanvas* canvas_mean_sigma_;
    PedMeanCalc* cur_ped_mean_calc_;
#endif /* __ROOTCLING__ || __CINT __ */
    
public:
    PedMeanShow();
    virtual ~PedMeanShow();
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void show_map(PedMeanCalc& ped_mean_calc);
    void show_mod(Int_t ct_idx);
    void show_mean_sigma(PedMeanCalc& ped_mean_calc);
#endif /* __ROOTCLING__ || __CINT __ */
    
};

#endif
