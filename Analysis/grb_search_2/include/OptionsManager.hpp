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
    TString pdf_filename;                     // -o
    double  min_signif;                       // -s
    int     min_bars;                         // -y
    int     max_bars;                         // -z
    TString bar_mask_filename;                // -k

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
