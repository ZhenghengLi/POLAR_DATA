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
    TString begin_gps;
    TString end_gps;
    TString ped_vector_filename;
    TString decoded_data_filename;
    TString xtalk_data_filename;
    TString xtalk_matrix_filename;
    bool    xtalk_matrix_read_flag;
    bool    show_flag;
    char    rw_mode;
    int     action;
    int     ct_num;
    int     max_bars;
    
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
