#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include "SciFile1RR.hpp"
#include "SciFileL1W.hpp"

using namespace std;

class Processor {
private:
        
public:
    Processor();
    ~Processor();

    void convert_data(SciFile1RR& scifile_1r,
                      SciFileL1W& scifile_l1);
    void write_meta_info(SciFile1RR& scifile_1r,
                         SciFileL1W& scifile_l1);
    
};

#endif
