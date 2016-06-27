#ifndef PEDMEANCALC_H
#define PEDMEANCALC_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "SciIterator.hpp"
#include "PedDataFile.hpp"

#define PED_BINS 128
#define PED_MAX 1024

using namespace std;

class PedMeanCalc {
private:
    TF1*  ped_gaus_[25][64];
    TH1F* ped_hist_[25][64];
    TH2F* ped_map_;

    bool is_all_created_;

public:
    Float_t mean[25][64];
    Float_t sigma[25][64];
    Float_t mean_0[25][64];
    Float_t sigma_0[25][64];
    
public:
    PedMeanCalc();
    ~PedMeanCalc();

    // write
    void fill_ped_data(SciIterator& sciIter,
                       PedDataFile& ped_data_file);

    // read
    void create_ped_hist();
    void delete_ped_hist();
    void fill_ped_hist(PedDataFile& ped_data_file);
    void fit_ped_hist();
    void draw_ped_map();
    void draw_ped_hist(int ct_i, int ch_j);
    void write_ped_vector(const char* filename,
                          PedDataFile& ped_data_file);

};

#endif
