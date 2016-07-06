#ifndef COMPTONEDGECALC_H
#define COMPTONEDGECALC_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <queue>
#include <algorithm>
#include "RootInc.hpp"
#include "EventIterator.hpp"
#include "SpecDataFile.hpp"
#include "CooConv.hpp"
#include "Constants.hpp"

using namespace std;

class ComptonEdgeCalc {
private:
    TVectorF ped_mean_vector_[25];
    TMatrixF xtalk_matrix_inv_[25];
    TVectorF energy_adc_vector_;

    TF1*  spec_func_[25][64];
    TH1F* spec_hist_[25][64];

    bool is_all_created_;
    bool is_all_filled_;
    bool is_all_fitted_;
    bool is_all_read_;

    bool is_ped_mean_read_;
    bool is_xtalk_matrix_read_;

    string source_type_; // "Na22" | "Cs137"

public:
    // fitting parameters here

private:
    void gen_energy_adc_vector_(EventIterator& eventIter);
    
public:
    ComptonEdgeCalc();
    ~ComptonEdgeCalc();

    void set_source_type(string type_str);
    // write
    void fill_spec_data(EventIterator& eventIter,
                        SpecDataFile& spec_data_file);
};

#endif
