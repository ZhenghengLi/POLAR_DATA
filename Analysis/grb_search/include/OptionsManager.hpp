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
    TString begin_gps;                        // -B
    TString end_gps;                          // -E
    TString decoded_data_filename;
    double  bw_start;                         // -a
    double  bw_stop;                          // -b
    int     bw_len;                           // -n
    double  min_prob;                         // -p
    int     bkg_distance;                     // -d
    int     bkg_nbins;                        // -m

    
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
