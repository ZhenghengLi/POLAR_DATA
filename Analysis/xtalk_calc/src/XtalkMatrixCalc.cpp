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
    xtalk_map_all_ = NULL;
    current_ct_idx_ = 0;
    for (int i = 0; i < 4; i++) {
        line_h_[i] = NULL;
        line_v_[i] = NULL;
    }

    is_all_created_ = false;
    is_all_filled_  = false;
    is_all_fitted_  = false;
    is_all_read_    = false;

    for (int i = 0; i < 25; i++) {
        ped_mean_vector_[i].ResizeTo(64);
        ped_mean_vector_[i].Zero();
        xtalk_matrix[i].ResizeTo(64, 64);
        xtalk_matrix_inv[i].ResizeTo(64, 64);
    }
    energy_adc_vector_.ResizeTo(64);
    
}

XtalkMatrixCalc::~XtalkMatrixCalc() {
    delete_xtalk_hist();
}

void XtalkMatrixCalc::fill_xtalk_data(SciIterator& sciIter, XtalkDataFile& xtalk_data_file) {

}

void XtalkMatrixCalc::create_xtalk_hist() {
    if (is_all_created_)
        return;
    delete_xtalk_hist();
    xtalk_map_mod_ = new TH2F("xtalk_map_mod", "Crosstalk Matrix of One Module", 64, 0, 64, 64, 0, 64);
    xtalk_map_mod_->SetDirectory(NULL);
    xtalk_map_all_ = new TH2F("xtalk_map_all", "Crosstalk Matrix of All Modules", 320, 0, 320, 320, 0, 320);
    xtalk_map_mod_->SetDirectory(NULL);
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            xtalk_hist_[jx][jy] = new TH2F(Form("xtalk_hist_%d_%d", jx + 1, jy + 1),
                                           Form("Crosstalk of %d => %d", jx + 1, jy + 1),
                                           256, 0, 4096, 128, 0, 1024);
            xtalk_hist_[jx][jy]->SetDirectory(NULL);
            xtalk_line_[jx][jy] = new TF1(Form("xtalk_line_%d_%d", jx + 1, jy + 1),
                                          "[0] * x", 0, 4096);
            xtalk_line_[jx][jy]->SetParameter(0, 0.1);
        }
    }
    for (int i = 0; i < 4; i++) {
        line_h_[i] = new TLine(0, (i + 1) * 64, 320, (i + 1) * 64);
        line_h_[i]->SetLineColor(kWhite);
        line_v_[i] = new TLine((i + 1) * 64, 0, (i + 1) * 64, 320);
        line_v_[i]->SetLineColor(kWhite);
    }
    is_all_created_ = true;
    is_all_filled_  = false;
    is_all_fitted_  = false;
}

void XtalkMatrixCalc::delete_xtalk_hist() {
    if (xtalk_map_mod_ != NULL) {
        delete xtalk_map_mod_;
        xtalk_map_mod_ = NULL;
    }
    if (xtalk_map_all_ != NULL) {
        delete xtalk_map_all_;
        xtalk_map_all_ = NULL;
    }
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            if (xtalk_hist_[jx][jy] != NULL) {
                delete xtalk_hist_[jx][jy];
                xtalk_hist_[jx][jy] = NULL;
            }
            if (xtalk_line_[jx][jy] != NULL) {
                delete xtalk_line_[jx][jy];
                xtalk_line_[jx][jy] = NULL;
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        if (line_h_[i] != NULL) {
            delete line_h_[i];
            line_h_[i] = NULL;
        }
        if (line_v_[i] != NULL) {
            delete line_v_[i];
            line_v_[i] = NULL;
        }
    }
    is_all_created_ = false;
    is_all_filled_  = false;
    is_all_fitted_  = false;
}

void XtalkMatrixCalc::reset_xtalk_hist() {
    if (!is_all_created_)
        return;
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            xtalk_hist_[jx][jy]->Reset();
            xtalk_line_[jx][jy]->SetParameter(0, 0.1);
        }
    }
    is_all_filled_ = false;
    is_all_fitted_ = false;
}
