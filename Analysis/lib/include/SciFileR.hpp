#ifndef SCIFILER_H
#define SCIFILER_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "RootInc.hpp"
#include "SciType.hpp"

class SciFileR: private SciType {
private:
    bool cur_is_1P_;
    
public:
    SciFileR();
    ~SciFileR();

};

#endif
