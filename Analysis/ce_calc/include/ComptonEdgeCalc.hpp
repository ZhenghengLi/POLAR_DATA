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

#define SPEC_BINS 64

using namespace std;

class ComptonEdgeCalc {
public:
    struct Bar {
        float adc;
        int   i;
        int   j;

        Bar(float my_adc = 0, int my_i = 0, int my_j = 0) {
            adc = my_adc;
            i   = my_i;
            j   = my_j;
        }
        bool operator<(const Bar& right_bar) const {
            return (adc < right_bar.adc);
        }
        bool operator<=(const Bar& right_bar) const {
            return (adc <= right_bar.adc);
        }
        bool operator>(const Bar& right_bar) const {
            return (adc > right_bar.adc);
        }
        bool operator>=(const Bar& right_bar) const {
            return (adc >= right_bar.adc);
        }
        bool operator==(const Bar& right_bar) const {
            return (adc == right_bar.adc);
        }
    };
    struct Pos {
        int   i;
        int   j;
        float abs_x;
        float abs_y;

        Pos(int my_i = 0, int my_j = 0) {
            randomize(my_i, my_j);
        }
        void randomize(int my_i, int my_j) {
            i = my_i;
            j = my_j;
            abs_x = ijtox(i, j) / 8 * ModD + ijtox(i, j) % 8 * BarD + gRandom->Rndm() * BarW;
            abs_y = ijtoy(i, j) / 8 * ModD + ijtoy(i, j) % 8 * BarD + gRandom->Rndm() * BarW;
        }
        bool is_adjacent_to(const Pos& my_pos) {
            if (i != my_pos.i) {
                return false;
            } else {
                int j_diff = abs(j - my_pos.j);
                if (j_diff == 1 || j_diff == 8 || j_diff == 7 || j_diff == 9) {
                    return true;
                } else {
                    return false;
                }
            }
        }
    };
    
private:
    TVectorF ped_mean_vector_[25];
    TMatrixF xtalk_matrix_inv_[25];
    TVectorF energy_adc_vector_;

    TF1*  spec_func_[25][64];
    TH1F* spec_hist_[25][64];
    TH2F* spec_count_map_;
    TLine* line_h_[4];
    TLine* line_v_[4];

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
    bool gen_energy_adc_vector_(EventIterator& eventIter);
    bool check_na22_event_(const SpecDataFile::SourceEvent_T source_event);
    bool check_cs137_event_(const SpecDataFile::SourceEvent_T source_event);
    double angle_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2);
    double distance_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2);
    
public:
    ComptonEdgeCalc();
    ~ComptonEdgeCalc();

    void set_source_type(string type_str);
    
    // write
    void fill_spec_data(EventIterator& eventIter,
                        SpecDataFile& spec_data_file);

    // read
    bool read_ped_mean_vector(const char* filename);
    bool read_xtalk_matrix_inv(const char* filename);

    void create_spec_hist();
    void delete_spec_hist();
    void fill_spec_hist(SpecDataFile& spec_data_file);
    void fit_spec_hist();
    void draw_spec_count_map();
    void draw_spec_hist(int ct_i, int ch_j);
};

#endif
