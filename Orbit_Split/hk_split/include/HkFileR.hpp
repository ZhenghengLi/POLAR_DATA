#ifndef HKFILER_H
#define HKFILER_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include "HkType1M.hpp"

using namespace std;

class HkFileR: private HkType1M {
private:

public:
    HkFileR();
    ~HkFileR();

};

#endif
