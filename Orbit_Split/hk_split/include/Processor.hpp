#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include "RootInc.hpp"
#include "HkFileR.hpp"
#include "HkFileW.hpp"
#include "OptionsManager.hpp"
#include "Constants.hpp"

#define GPS_DIFF_MAX 150

using namespace std;

class Processor {
private:
    OptionsManager* cur_options_mgr_;
    HkFileR* hkfile_r_arr_;
    int      hkfile_r_len_;
    int      cur_index_;
    bool     reach_end_;
    
public:
    Processor(OptionsManager* my_options_mgr);
    ~Processor();
    bool open_and_check();
    void set_start();
    bool next_file(HkFileW& hkfile_w);
    void write_the_file(HkFileW& hkfile_w);
    void write_meta_info(HkFileW& hkfile_w);

};

#endif
