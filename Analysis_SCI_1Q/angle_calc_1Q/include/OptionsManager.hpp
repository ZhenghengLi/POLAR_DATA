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
    TString deadtime_filename;      // -d
    TString weight_filename;        // -w
    TString output_filename;        // -o
    TString bar_mask_filename;      // -k
    bool    no_deadtime;

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
