#ifndef PEDMEANCALC_H
#define PEDMEANCALC_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include "RootInc.hpp"
#include "SciIterator.hpp"
#include "PedDataFile.hpp"
#include "CooConv.hpp"
#include "Constants.hpp"

#define PED_BINS 128
#define PED_MAX 1024

using namespace std;

class PedMeanCalc {
private:
    TF1*  ped_gaus_[25][64];
    TH1F* ped_hist_[25][64];
    TH2F* ped_map_;
    TH2F* ped_mean_;
    TH2F* ped_sigma_;

    bool is_all_created_;
    bool is_all_filled_;
    bool is_all_fitted_;
    bool is_all_read_;

    int max_bars_;

public:
    Float_t mean[25][64];
    Float_t sigma[25][64];
    Float_t mean_0[25][64];
    Float_t sigma_0[25][64];
    
public:
    PedMeanCalc();
    ~PedMeanCalc();

    // write
    void set_max_bars(int bars);
    void fill_ped_data(SciIterator& sciIter,
                       PedDataFile& ped_data_file);

    // read
    void create_ped_hist();
    void delete_ped_hist();
    void fill_ped_hist(PedDataFile& ped_data_file);
    void fit_ped_hist();
    void draw_ped_map();
    void draw_ped_mean();
    void draw_ped_sigma();
    void draw_ped_hist(int ct_i, int ch_j);
    bool write_ped_vector(const char* filename,
                          PedDataFile& ped_data_file);
    bool read_ped_vector(const char* filename);

};

#endif
