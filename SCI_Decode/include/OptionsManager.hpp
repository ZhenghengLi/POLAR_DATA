#ifndef OPTIONSMANAGER_H
#define OPTIONSMANAGER_H

#include <iostream>
#include <queue>
#include "TString.h"
#include "Constants.hpp"

using namespace std;

class OptionsManager {
private:
    bool version_flag_;
    
public:
    TString list_file;
    queue<TString> raw_file_queue;
    TString out_file;
    TString log_file;
    
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
