#ifndef PEDESTAL_H
#define PEDESTAL_H 1

#include <iostream>
#include "RootInc.hpp"

using namespace std;

class Pedestal {
private:
    TVectorF ped_mean_vec_CT_[25];

private:

public:
    TVectorF ped_vec_CT[25];

public:
    Pedestal();
    ~Pedestal();

    bool read_pedestal(const char* ped_filename);
    void gen_pedestal();
    void gen_pedestal(int ct_num, double temp);

};

#endif
