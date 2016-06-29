#include "XtalkMatrixCalc.hpp"

using namespace std;

XtalkMatrixCalc::XtalkMatrixCalc() {
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            xtalk_line_[jx][jy] = NULL;
            xtalk_hist_[jx][jy] = NULL;
        }
    }
    xtalk_map_mod_ = NULL;

    is_all_created_ = false;
    is_all_filled_  = false;
    is_all_fitted_  = false;
    is_all_read_    = false;

    for (int i = 0; i < 25; i++) {
        xtalk_matrix[i].ResizeTo(64, 64);
        xtalk_matrix_inv[i].ResizeTo(64, 64);
        
    }
}

XtalkMatrixCalc::~XtalkMatrixCalc() {

}
