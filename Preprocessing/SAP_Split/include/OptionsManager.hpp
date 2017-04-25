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
    vector<TString> raw_file_vector;
    TString out_file;
    TString met_begin;
    TString met_end;
    bool    no_check_conn;

public:
    OptionsManager();
    ~OptionsManager();

    bool parse(int argc_par, char** argv_par);
    void print_help();
    void print_version();
    void init();
    bool get_version_flag();

};

#endif
