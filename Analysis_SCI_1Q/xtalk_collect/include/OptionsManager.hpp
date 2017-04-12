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
    double  begin_met_time;         // -B
    double  end_met_time;           // -E
    TString xtalk_data_filename;      // -F

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
