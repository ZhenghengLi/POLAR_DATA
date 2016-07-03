#ifndef XTALKMATRIXSHOW_H
#define XTALKMATRIXSHOW_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#if !(defined(__ROOTCLING__) || defined(__CINT__))
#include "XtalkMatrixCalc.hpp"
#include "CooConv.hpp"
#include "RootInc.hpp"
#endif /* __ROOTCLING__ || __CINT __ */
#include "RQ_OBJECT.h"

using namespace std;

class XtalkMatrixShow {
    RQ_OBJECT("XtalkMatrixShow");

public: // slots
    virtual void CloseWindow();
    virtual void ProcessAction1(Int_t event,
                                Int_t px,
                                Int_t py,
                                TObject* selected);
    virtual void ProcessAction2(Int_t event,
                                Int_t px,
                                Int_t py,
                                TObject* selected);
private:
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    TCanvas* canvas_mod_map_cur_;
    TCanvas* canvas_jx_jy_line_;
    TCanvas* canvas_mod_map_all_;
    TCanvas* canvas_mod_map_sel_;
    XtalkMatrixCalc* cur_xtalk_matrix_calc_;
#endif /* __ROOTCLING__ || __CINT __ */

public:
    XtalkMatrixShow();
    virtual ~XtalkMatrixShow();

#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void show_mod_map_cur(XtalkMatrixCalc& xtalk_matrix_calc);
    void show_jx_jy_line(int jx, int jy);
    void show_mod_map_all(XtalkMatrixCalc& xtalk_matrix_calc);
    void show_mod_map_sel(int ct_idx);
#endif /* __ROOTCLING__ || __CINT __ */
    
    
};


#endif
