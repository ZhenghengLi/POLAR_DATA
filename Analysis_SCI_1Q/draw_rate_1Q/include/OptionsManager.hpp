#ifndef OPTIONSMANAGER_H
#define OPTIONSMANAGER_H

#include <iostream>
#include <vector>
#include "TString.h"
#include "Constants.hpp"

using namespace std;

class OptionsManager {
private:
    bool version_flag_;

public:
    TString pol_event_filename;
    double  begin_met_time;
    double  end_met_time;
    TString output_filename;
    double  binwidth;
    int     phase;
    bool    tout1_flag;
    int     max_bars;
    int     min_bars;
    TString bar_mask_filename;
    int     niter;
    double  min_signif;
    TString weight_filename;
    double  grb_ra;
    double  grb_dec;
    bool    align_hist;

public:
    OptionsManager();
    ~OptionsManager();

    bool parse(int argc_par, char** argv_par);
    void print_help();
    void print_version();
    void print_options();
    void init();
    bool get_version_flag();

};

#endif
