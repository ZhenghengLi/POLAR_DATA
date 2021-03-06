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
    TString gain_vec_filename;      // -g
    TString gain_vs_hv_filename;    // -G
    bool    gain_hv_flag;
    TString gain_temp_filename;     // -t
    bool    gain_temp_flag;
    TString output_filename;        // -o

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
