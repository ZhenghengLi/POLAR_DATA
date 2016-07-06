#ifndef XTALKMATRIXCALC_H
#define XTALKMATRIXCALC_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include "RootInc.hpp"
#include "SciIterator.hpp"
#include "XtalkDataFile.hpp"
#include "CooConv.hpp"
#include "Constants.hpp"

using namespace std;

class XtalkMatrixCalc {
private:
    TVectorF ped_mean_vector_[25];
    TVectorF energy_adc_vector_;

    TF1*   xtalk_line_[64][64];
    TH2F*  xtalk_hist_[64][64];
    TH2F*  xtalk_map_mod_;
    TH2F*  xtalk_map_mod_2d_;
    TH2F*  xtalk_map_mod_3d_;
    TH2F*  xtalk_map_all_;
    TLine* line_h_[4];
    TLine* line_v_[4];

    int   current_ct_idx_;

    bool is_all_created_;
    bool is_all_filled_;
    bool is_all_fitted_;
    bool is_all_read_;
    bool is_ped_mean_read_;

    char name_[80];
    char title_[80];

public:
    TMatrixF xtalk_matrix[25];
    TMatrixF xtalk_matrix_inv[25];

private:
    void gen_energy_adc_vector_(SciIterator& sciIter);

public:
    XtalkMatrixCalc();
    ~XtalkMatrixCalc();

    // write
    void fill_xtalk_data(SciIterator& sciIter,
                         XtalkDataFile& xtalk_data_file);
    bool write_xtalk_matrix(const char* filename,
                            XtalkDataFile& xtalk_data_file);

    // read
    bool read_ped_mean_vector(const char* filename);
    
    void create_xtalk_hist();
    void delete_xtalk_hist();
    void reset_xtalk_hist();
    void fill_xtalk_hist(int ct_idx,
                         XtalkDataFile& xtalk_data_file);
    void fit_xtalk_hist();
    void draw_xtalk_map_cur_mod_2d();
    void draw_xtalk_map_cur_mod_3d();
    void draw_xtalk_line(int jx, int jy);
    
    bool read_xtalk_matrix(const char* filename);
    void draw_xtalk_map_all();
    void draw_xtalk_map_sel_mod(int ct_idx);    

    int get_current_ct_idx() {
        return current_ct_idx_;
    }
    
};

#endif
