#ifndef Gain_HH
#define Gain_HH

#include <iostream>
#include "RootInc.hpp"

using namespace std;

class Gain {
private:
    TMatrixF gain_vs_hv_p0_mat_;
    TMatrixF gain_vs_hv_p1_mat_;
    TVectorF adc_per_kev_CT_[25];

public:
    TMatrixF bad_calib_mat;
    TVectorF gain_vec_CT[25];

public:
    Gain();
    ~Gain();

    bool read_gain_vec(const char* gain_vec_filename);
    bool read_gain_vs_hv(const char* gain_vs_hv_filename);
    void gen_gain();
    void gen_gain(int ct_num, double hv);

};


#endif
