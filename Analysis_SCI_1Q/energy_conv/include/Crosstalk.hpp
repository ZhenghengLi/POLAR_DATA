#ifndef Crosstalk_H
#define Crosstalk_H 1

#include <iostream>
#include "RootInc.hpp"

using namespace std;

class Crosstalk {
private:
    TMatrixF xtalk_mat_inv_CT_[25];

public:
    TMatrixF xtalk_matrix_inv_CT[25];

public:
    Crosstalk();
    ~Crosstalk();

    bool read_crosstalk(const char* xtalk_filename);
    void gen_crosstalk();

};

#endif
