#include "XtalkMatrixCalc.hpp"

using namespace std;

XtalkMatrixCalc::XtalkMatrixCalc() {
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            xtalk_line_[jx][jy] = NULL;
            xtalk_hist_[jx][jy] = NULL;
        }
    }
    xtalk_map_mod_    = NULL;
    xtalk_map_mod_2d_ = NULL;
    xtalk_map_mod_3d_ = NULL;
    xtalk_map_all_    = NULL;
    current_ct_idx_ = 0;
    for (int i = 0; i < 4; i++) {
        line_h_[i] = NULL;
        line_v_[i] = NULL;
    }

    is_all_created_   = false;
    is_all_filled_    = false;
    is_all_fitted_    = false;
    is_all_read_      = false;
    is_ped_mean_read_ = false;

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

void XtalkMatrixCalc::gen_energy_adc_vector_(SciIterator& sciIter) {
    copy(sciIter.t_modules.energy_adc, sciIter.t_modules.energy_adc + 64,
         energy_adc_vector_.GetMatrixArray());
    // subtract pedestal and common noise
    int   idx = sciIter.t_modules.ct_num - 1;
    float cur_common_sum   = 0;
    int   cur_common_n     = 0;
    float cur_common_noise = 0;
    if (sciIter.t_modules.compress != 3) {
        for (int j = 0; j < 64; j++) {
            if (energy_adc_vector_(j) < 4096) {
                energy_adc_vector_(j) -= ped_mean_vector_[idx](j);
            }
            if (!sciIter.t_modules.trigger_bit[j]) {
                cur_common_sum += energy_adc_vector_(j);
                cur_common_n++;
            }
        }
    }
    if (sciIter.t_modules.compress == 0 || sciIter.t_modules.compress == 2) {
        cur_common_noise = (cur_common_n > 0 ? cur_common_sum / cur_common_n : 0);
    } else if (sciIter.t_modules.compress == 3) {
        cur_common_noise = sciIter.t_modules.common_noise;
    } else {
        cur_common_noise = 0;
    }
    for (int j = 0; j < 64; j++) {
        if (energy_adc_vector_(j) < 4096) {
            energy_adc_vector_(j) -= cur_common_noise;
        }
    }
}

void XtalkMatrixCalc::fill_xtalk_data(SciIterator& sciIter, XtalkDataFile& xtalk_data_file) {
    if (xtalk_data_file.get_mode() != 'w')
        return;
    if (!is_ped_mean_read_) {
        cerr << "WARNING: pedestal mean vectors are not read yet. " << endl;
    }
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Filling crosstalk data of all modules ... " << endl;
    cout << "[ " << flush;
    sciIter.phy_modules_set_start();
    while (sciIter.phy_modules_next()) {
        cur_percent = static_cast<int>(100 * sciIter.phy_modules_get_cur_entry() / sciIter.phy_modules_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        if (sciIter.t_modules.compress == 1)
            continue;
        gen_energy_adc_vector_(sciIter);        
        int idx = sciIter.t_modules.ct_num - 1;
        // select crosstalk data
        for (int jx = 0; jx < 64; jx++) {
            if (!sciIter.t_modules.trigger_bit[jx])
                continue;
            for (int jy = 0; jy < 64; jy++) {
                if ((jx + 1 != jy && jx + 1 <= 63 && sciIter.t_modules.trigger_bit[jx + 1]) ||
                    (jx - 1 != jy && jx - 1 >= 0  && sciIter.t_modules.trigger_bit[jx - 1]) ||
                    (jx + 7 != jy && jx + 7 <= 63 && sciIter.t_modules.trigger_bit[jx + 7]) ||
                    (jx + 8 != jy && jx + 8 <= 63 && sciIter.t_modules.trigger_bit[jx + 8]) ||
                    (jx + 9 != jy && jx + 9 <= 63 && sciIter.t_modules.trigger_bit[jx + 9]) ||
                    (jx - 7 != jy && jx - 7 >= 0  && sciIter.t_modules.trigger_bit[jx - 7]) ||
                    (jx - 8 != jy && jx - 8 >= 0  && sciIter.t_modules.trigger_bit[jx - 8]) ||
                    (jx - 9 != jy && jx - 9 >= 0  && sciIter.t_modules.trigger_bit[jx - 9]))
                    continue;
                if ((jy + 1 != jx && jy + 1 <= 63 && sciIter.t_modules.trigger_bit[jy + 1]) ||
                    (jy - 1 != jx && jy - 1 >= 0  && sciIter.t_modules.trigger_bit[jy - 1]) ||
                    (jy + 7 != jx && jy + 7 <= 63 && sciIter.t_modules.trigger_bit[jy + 7]) ||
                    (jy + 8 != jx && jy + 8 <= 63 && sciIter.t_modules.trigger_bit[jy + 8]) ||
                    (jy + 9 != jx && jy + 9 <= 63 && sciIter.t_modules.trigger_bit[jy + 9]) ||
                    (jy - 7 != jx && jy - 7 >= 0  && sciIter.t_modules.trigger_bit[jy - 7]) ||
                    (jy - 8 != jx && jy - 8 >= 0  && sciIter.t_modules.trigger_bit[jy - 8]) ||
                    (jy - 9 != jx && jy - 9 >= 0  && sciIter.t_modules.trigger_bit[jy - 9]))
                    continue;
                if (jy != jx + 1 && jy != jx - 1 && jy != jx + 7 && jy != jx + 8 &&
                    jy != jx + 9 && jy != jx - 7 && jy != jx - 8 && jy != jx - 9 &&
                    sciIter.t_modules.trigger_bit[jy])
                    continue;
                if (energy_adc_vector_(jy) / energy_adc_vector_(jx) > 0.4)
                    continue;
                if (energy_adc_vector_(jy) < 4096) {
                    xtalk_data_file.t_xtalk_data[idx].jx = jx;
                    xtalk_data_file.t_xtalk_data[idx].x  = energy_adc_vector_(jx);
                    xtalk_data_file.t_xtalk_data[idx].jy = jy;
                    xtalk_data_file.t_xtalk_data[idx].y  = energy_adc_vector_(jy);
                    xtalk_data_file.mod_fill(idx);
                }
            }
        }
    }
    cout << " DONE ]" << endl;
}

void XtalkMatrixCalc::create_xtalk_hist() {
    if (is_all_created_)
        return;
    delete_xtalk_hist();
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
             xtalk_hist_[jx][jy] = new TH2F(Form("xtalk_hist_%02d_%02d", jx + 1, jy + 1),
                                           Form("Crosstalk of %02d => %02d", jx + 1, jy + 1),
                                           256, 0, 4096, 128, 0, 1024);
            xtalk_hist_[jx][jy]->SetDirectory(NULL);
            xtalk_line_[jx][jy] = new TF1(Form("xtalk_line_%02d_%02d", jx + 1, jy + 1),
                                          "[0] * x", 0, 4096);
            xtalk_line_[jx][jy]->SetParameter(0, 0.1);
        }
    }
    is_all_created_ = true;
    is_all_filled_  = false;
    is_all_fitted_  = false;
}

void XtalkMatrixCalc::delete_xtalk_hist() {
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

void XtalkMatrixCalc::fill_xtalk_hist(int ct_idx, XtalkDataFile& xtalk_data_file) {
    if (xtalk_data_file.get_mode() != 'r')
        return;
    if (!is_all_created_)
        return;
    if (is_all_filled_)
        return;
    current_ct_idx_ = ct_idx;
    xtalk_data_file.mod_set_start(current_ct_idx_);
    while (xtalk_data_file.mod_next(current_ct_idx_)) {
        xtalk_hist_[xtalk_data_file.t_xtalk_data[current_ct_idx_].jx]
            [xtalk_data_file.t_xtalk_data[current_ct_idx_].jy]->Fill(
            xtalk_data_file.t_xtalk_data[current_ct_idx_].x,
            xtalk_data_file.t_xtalk_data[current_ct_idx_].y);
    }
    is_all_filled_ = true;
}

void XtalkMatrixCalc::fit_xtalk_hist() {
    if (!is_all_filled_)
        return;
    if (is_all_fitted_)
        return;
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            if (jx == jy) {
                xtalk_matrix[current_ct_idx_](jy, jx) = 1.0;
                continue;
            }
            if (xtalk_hist_[jx][jy]->GetEntries() < 5) {
                cerr << "CT_" << current_ct_idx_ << " : " << jx + 1 << " => " << jy + 1
                     << "    " << "number of entries is too small" << endl;
                xtalk_matrix[current_ct_idx_](jy, jx) = 0.0001;
            } else {
                xtalk_hist_[jx][jy]->Fit(xtalk_line_[jx][jy], "QN");
                xtalk_matrix[current_ct_idx_](jy, jx) = xtalk_line_[jx][jy]->GetParameter(0);
            }
        }
    }
    xtalk_matrix_inv[current_ct_idx_] = xtalk_matrix[current_ct_idx_];
    xtalk_matrix_inv[current_ct_idx_].Invert();
    is_all_fitted_ = true;
}

void XtalkMatrixCalc::draw_xtalk_map_cur_mod_2d() {
    if (!is_all_fitted_)
        return;
    xtalk_map_mod_2d_ = static_cast<TH2F*>(gROOT->FindObject("xtalk_map_mod_2d"));
    if (xtalk_map_mod_2d_ == NULL) {
        xtalk_map_mod_2d_ = new TH2F("xtalk_map_mod_2d",
                                  Form("Crosstalk Matrix 2D Map of CT_%02d", current_ct_idx_ + 1),
                                  64, 0, 64, 64, 0, 64);
        xtalk_map_mod_2d_->SetDirectory(NULL);
        xtalk_map_mod_2d_->GetXaxis()->SetNdivisions(64);
        xtalk_map_mod_2d_->GetYaxis()->SetNdivisions(64);
    }
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            xtalk_map_mod_2d_->SetBinContent(jy + 1, 64 - jx, xtalk_matrix[current_ct_idx_](jx, jy));
        }
    }
    xtalk_map_mod_2d_->Draw("COLZ");
}

void XtalkMatrixCalc::draw_xtalk_map_cur_mod_3d() {
    if (!is_all_fitted_)
    xtalk_map_mod_3d_ = static_cast<TH2F*>(gROOT->FindObject("xtalk_map_mod_3d"));
    if (xtalk_map_mod_3d_ == NULL) {
        xtalk_map_mod_3d_ = new TH2F("xtalk_map_mod_3d",
                                  Form("Crosstalk Matrix 3D Map of CT_%02d", current_ct_idx_ + 1),
                                  64, 0, 64, 64, 0, 64);
        xtalk_map_mod_3d_->SetDirectory(NULL);
        xtalk_map_mod_3d_->GetXaxis()->SetNdivisions(64);
        xtalk_map_mod_3d_->GetYaxis()->SetNdivisions(64);
    }
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            xtalk_map_mod_3d_->SetBinContent(jy + 1, 64 - jx, xtalk_matrix[current_ct_idx_](jx, jy));
        }
    }
    xtalk_map_mod_3d_->Draw("LEGO2");
}

void XtalkMatrixCalc::draw_xtalk_line(int jx, int jy) {
    if (!is_all_fitted_)
        return;
    if (xtalk_hist_[jx][jy]->GetEntries() < 5)
        return;
    xtalk_hist_[jx][jy]->Fit(xtalk_line_[jx][jy], "Q");
}

void XtalkMatrixCalc::draw_xtalk_map_all() {
    if (!is_all_read_)
        return;
    xtalk_map_all_ = static_cast<TH2F*>(gROOT->FindObject("xtalk_map_all"));
    if (xtalk_map_all_ == NULL) {
        xtalk_map_all_ = new TH2F("xtalk_map_all", "Crosstalk Matrix of All Modules",
                                  320, 0, 320, 320, 0, 320);
        xtalk_map_all_->SetDirectory(NULL);
        for (int i = 0; i < 320; i++) {
            if (i % 64 == 0) {
                xtalk_map_all_->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
                xtalk_map_all_->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
            }
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int jx = 0; jx < 64; jx++) {
            for (int jy = 0; jy < 64; jy++) {
                xtalk_map_all_->SetBinContent((i / 5) * 64 + jy + 1,
                                              (4 - i % 5) * 64 + (63 - jx) + 1,
                                              xtalk_matrix[i](jx, jy));
            }
        }
    }
    xtalk_map_all_->Draw("COLZ");
    for (int i = 0; i < 4; i++) {
        if (line_h_[i] != NULL)
            delete line_h_[i];
        line_h_[i] = new TLine(0, (i + 1) * 64, 320, (i + 1) * 64);
        line_h_[i]->SetLineColor(kWhite);
        line_h_[i]->Draw("SAME");
        if (line_v_[i] != NULL)
            delete line_v_[i];
        line_v_[i] = new TLine((i + 1) * 64, 0, (i + 1) * 64, 320);
        line_v_[i]->SetLineColor(kWhite);
        line_v_[i]->Draw("SAME");
    }
}

void XtalkMatrixCalc::draw_xtalk_map_sel_mod(int ct_idx) {
    if (!is_all_read_)
        return;
    xtalk_map_mod_ = static_cast<TH2F*>(gROOT->FindObject("xtalk_map_mod"));
    if (xtalk_map_mod_ == NULL) {
        xtalk_map_mod_ = new TH2F("xtalk_map_mod", "Crosstalk Matrix Map of One Module",
                                  64, 0, 64, 64, 0, 64);
        xtalk_map_mod_->SetDirectory(NULL);
        xtalk_map_mod_->GetXaxis()->SetNdivisions(64);
        xtalk_map_mod_->GetYaxis()->SetNdivisions(64);
    }
    xtalk_map_mod_->SetTitle(Form("Crosstalk Matrix Map of CT_%02d", ct_idx + 1));
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            xtalk_map_mod_->SetBinContent(jy + 1, 64 - jx, xtalk_matrix[ct_idx](jx, jy));
        }
    }
    xtalk_map_mod_->Draw("LEG02");
}

bool XtalkMatrixCalc::write_xtalk_matrix(const char* filename,
                                         XtalkDataFile& xtalk_data_file) {
    if (!is_all_fitted_)
        return false;
    TFile* xtalk_matrix_file = new TFile(filename, "RECREATE");
    if (xtalk_matrix_file->IsZombie())
        return false;
    for (int i = 0; i < 25; i++) {
        xtalk_matrix[i].Write(Form("xtalk_mat_ct_%02d", i + 1));
        xtalk_matrix_inv[i].Write(Form("xtalk_mat_inv_ct_%02d", i + 1));
    }
    TNamed* tmp_meta;
    // m_dattype
    tmp_meta = new TNamed("m_dattype", "POLAR CROSSTALK MATRIX");
    tmp_meta->Write();
    delete tmp_meta;
    // m_version
    tmp_meta = new TNamed("m_version", (SW_NAME + " " + SW_VERSION).c_str());
    tmp_meta->Write();
    delete tmp_meta;
    // m_gentime
    TTimeStamp* cur_time = new TTimeStamp();
    tmp_meta = new TNamed("m_gentime", cur_time->AsString("lc"));
    tmp_meta->Write();
    delete cur_time;
    cur_time = NULL;
    delete tmp_meta;
    // m_fromfile
    tmp_meta = new TNamed("m_fromfile", xtalk_data_file.get_fromfile_str().c_str());
    tmp_meta->Write();
    delete tmp_meta;
    // m_gps_span
    tmp_meta = new TNamed("m_gps_span", xtalk_data_file.get_gps_span_str().c_str());
    tmp_meta->Write();
    delete tmp_meta;
    tmp_meta = NULL;
    xtalk_matrix_file->Close();
    delete xtalk_matrix_file;
    xtalk_matrix_file = NULL;
    return true;
}

bool XtalkMatrixCalc::read_xtalk_matrix(const char* filename) {
    TFile* xtalk_matrix_file = new TFile(filename, "READ");
    if (xtalk_matrix_file->IsZombie())
        return false;
    TMatrixF* xtalk_mat;
    TMatrixF* xtalk_mat_inv;
    for (int i = 0; i < 25; i++) {
        xtalk_mat     = static_cast<TMatrixF*>(xtalk_matrix_file->Get(Form("xtalk_mat_ct_%02d", i + 1)));
        xtalk_mat_inv = static_cast<TMatrixF*>(xtalk_matrix_file->Get(Form("xtalk_mat_inv_ct_%02d", i + 1)));
        if (xtalk_mat == NULL || xtalk_mat_inv == NULL)
            return false;
        xtalk_matrix[i]     = (*xtalk_mat);
        xtalk_matrix_inv[i] = (*xtalk_mat_inv);
        delete xtalk_mat;
        xtalk_mat = NULL;
        delete xtalk_mat_inv;
        xtalk_mat_inv = NULL;
    }
    xtalk_matrix_file->Close();
    delete xtalk_matrix_file;
    xtalk_matrix_file = NULL;
    is_all_read_ = true;
    return true;
}

bool XtalkMatrixCalc::read_ped_mean_vector(const char* filename) {
    TFile* ped_vec_file = new TFile(filename, "READ");
    if (ped_vec_file->IsZombie())
        return false;
    TVectorF* mean_vec;
    for (int i = 0; i < 25; i++) {
        mean_vec = static_cast<TVectorF*>(ped_vec_file->Get(Form("ped_mean_vec_ct_%02d", i + 1)));
        if (mean_vec == NULL)
            return false;
        ped_mean_vector_[i] = (*mean_vec);
        delete mean_vec;
        mean_vec = NULL;
    }
    ped_vec_file->Close();
    delete ped_vec_file;
    ped_vec_file = NULL;
    is_ped_mean_read_ = true;
    return true;
}
