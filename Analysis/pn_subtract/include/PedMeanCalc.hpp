#ifndef PEDMEANCALC_H
#define PEDMEANCALC_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "SciIterator.hpp"
#include "PedDataFile.hpp"

using namespace std;

class PedMeanCalc {
private:

public:
    PedMeanCalc();
    ~PedMeanCalc();

    // write
    void fill_ped_data(SciIterator& sciIter,
                       PedDataFile& ped_data_file);
    // read
    void fill_ped_hist(PedDataFile& ped_data_file);
    void fit_ped_hist(bool draw_flag, int ct_i, int ch_j);

};

#endif
