#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include "RootInc.hpp"
#include "SciFileR.hpp"
#include "SciFileW.hpp"
#include "OptionsManager.hpp"

#define GPS_DIFF_MAX 150

using namespace std;

class Processor {
private:
    OptionsManager* cur_options_mgr_;
    SciFileR* scifile_r_arr_;
    int       scifile_r_len_;
    
public:
    Processor(OptionsManager* my_options_mgr);
    ~Processor();
    bool open_and_check();

    

};

#endif
