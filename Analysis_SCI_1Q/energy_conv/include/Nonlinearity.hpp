#ifndef Nonlinearity_H
#define Nonlinearity_H 1

#include <iostream>
#include "RootInc.hpp"

using namespace std;

class Nonlinearity {
private:
    TVectorF nonlin_norm_fun_p0_[25];
    TVectorF nonlin_norm_fun_p1_[25];
    TVectorF nonlin_norm_fun_p2_[25];

public:
    TF1* nonlin_norm_fun[25][64];

public:
    Nonlinearity();
    ~Nonlinearity();

    bool read_nonlin_par(const char* nonlin_filename);
    void gen_nonlin_fun();

};

#endif
