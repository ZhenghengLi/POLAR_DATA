#ifndef RATECANVAS_H
#define RATECANVAS_H

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

class CommonCanvas {
    RQ_OBJECT("CommonCanvas");

public: // slots
    virtual void CloseWindow();
    virtual void ProcessAction(Int_t event,
                               Int_t px,
                               Int_t py,
                               TObject* selected);
    
private:
#if !(defined(__ROOTCLING__) || defined(__CINT__))
	TCanvas* canvas_spec_;
	TCanvas* canvas_ratio_;
#endif /* __ROOTCLING__ || __CINT __ */
    
public:
    CommonCanvas();
    virtual ~CommonCanvas();
#if !(defined(__ROOTCLING__) || defined(__CINT__))
	void cd_spec(int i);
#endif /* __ROOTCLING__ || __CINT __ */
    
};

#endif
