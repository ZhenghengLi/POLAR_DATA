#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include "HkFileL1Conv.hpp"
#include "OptionsManager.hpp"

using namespace std;

class Processor {
private:
    OptionsManager* cur_options_mgr_;
        
public:
    Processor(OptionsManager* my_options_mgr);
    ~Processor();

    void convert_data(HkFileL1Conv& hkfile_l1_conv);
    void write_meta_info(HkFileL1Conv& hkfile_l1_conv);
    
};

#endif
