#ifndef RATECANVAS_H
#define RATECANVAS_H

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

class RateCanvas {
    RQ_OBJECT("RateCanvas");

public: // slots
    virtual void CloseWindow();
    virtual void CloseLC();
    virtual void ProcessAction(Int_t event,
                               Int_t px,
                               Int_t py,
                               TObject* selected);

private:
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    TCanvas* canvas_trigger_;
    TCanvas* canvas_modules_;
    TCanvas* canvas_modules_tout1_;
	TCanvas* canvas_ch_map_;
    TCanvas* canvas_trigger_subbkg_;
    TCanvas* canvas_theta_pha_;
    TCanvas* canvas_pos_map_;
    TH1D*    cur_trigger_hist_;
    TH1D*    cur_trigger_hist_bkg_;
    TH1D*    cur_trigger_hist_subbkg_;
    double   start_met_time_;
    double   min_signif_;
    int      select_count_;
    double   select_x_[2];
    int      line_cnt_;
    TLine*   line_obj_[2];
    TLine*   line_zero_;
    TLine*   line_t90_left_;
    TLine*   line_t90_right_;
    TLine*   line_peak_;
    TLine*   line_T0_;
    TPaveText* pavetext_;
    bool     keypressed;
    // function
    string met_to_utc_str_(double gps_time);
#endif /* __ROOTCLING__ || __CINT __ */

public:
    RateCanvas(double met_time, double min_s);
    virtual ~RateCanvas();
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    void cd_modules(int i);
    void cd_modules_tout1(int i);
	void cd_ch_map(int i);
    void cd_theta_pha(int i);
    void cd_pos_map();
    void draw_trigger_hist(TH1D* trigger_hist);
    void draw_trigger_hist_bkg(TH1D* trigger_hist_bkg);
#endif /* __ROOTCLING__ || __CINT __ */

};

#endif
