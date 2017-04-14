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
    TString na22_data_filename;
    int     ct_num;                  // -n
    float   low_temp;                // -a
    float   high_temp;               // -b
    float   low_hv;                  // -x
    float   high_hv;                 // -y
    TString ce_result_filename;      // -o

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
