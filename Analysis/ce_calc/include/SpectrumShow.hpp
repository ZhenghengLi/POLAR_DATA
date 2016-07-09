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

private:
#if !(defined(__ROOTCLING__) || defined(__CINT__))

#endif /* __ROOTCLING__ || __CINT __ */

public:
    SpectrumShow();
    virtual ~SpectrumShow();

#if !(defined(__ROOTCLING__) || defined(__CINT__))

#endif /* __ROOTCLING__ || __CINT __ */
    
};

#endif
