#ifndef PEDMEANSHOW_H
#define PEDMEANSHOW_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#ifndef __CINT__
#include "PedMeanCalc.hpp"
#endif /* __CINT __ */
#include "RootInc.hpp"

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
    TCanvas* canvas_map_;
    TCanvas* canvas_mod_;
    
public:
    PedMeanShow();
    virtual ~PedMeanShow();
#ifndef __CINT__
    void show_map(PedMeanCalc& ped_mean_calc);
    void show_mod(PedMeanCalc& ped_mean_calc,
                  Int_t ct_idx);
#endif /* __CINT __ */
    
};

#endif
