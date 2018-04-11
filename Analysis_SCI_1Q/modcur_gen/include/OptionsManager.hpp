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
    TString angle_filename;
    bool    subbkg_flag;            // -S
    double  bkg_before_start;       // -a
    double  bkg_before_stop;        // -b
    double  bkg_after_start;        // -c
    double  bkg_after_stop;         // -d
    double  grb_start;              // -x
    double  grb_stop;               // -y
    TString output_filename;        // -o
    int     nbins;                  // -n
    double  energy_thr;             // -t
    double  rate_binw;              // -w

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
