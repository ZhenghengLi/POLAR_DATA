#include "ComptonEdgeCalc.hpp"

using namespace std;

ComptonEdgeCalc::ComptonEdgeCalc() {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            spec_func_[i][j] = NULL;
            spec_hist_[i][j] = NULL;
        }
    }
    spec_count_map_ = NULL;
    for (int i = 0; i < 8; i++) {
        line_h_[i] = NULL;
        line_v_[i] = NULL;
    }
    
    is_all_created_ = false;
    is_all_filled_  = false;
    is_all_fitted_  = false;
    is_all_read_    = false;

    is_ped_mean_read_     = false;
    is_xtalk_matrix_read_ = false;
    
    for (int i = 0; i < 25; i++) {
        ped_mean_vector_[i].ResizeTo(64);
        ped_mean_vector_[i].Zero();
        xtalk_matrix_inv_[i].ResizeTo(64, 64);
        xtalk_matrix_inv_[i].Zero();
        for (int jx = 0; jx < 64; jx++) {
            for (int jy = 0; jy < 64; jy++) {
                if (jx == jy) {
                    xtalk_matrix_inv_[i](jx, jy) = 1;
                }
            }
        }
    }
    energy_adc_vector_.ResizeTo(64);

    source_type_ = "Na22";
    
}

ComptonEdgeCalc::~ComptonEdgeCalc() {
    delete_spec_hist();
}

void ComptonEdgeCalc::set_source_type(string type_str) {
    if (type_str == "na22" || type_str == "Na22") {
        source_type_ = "Na22";
    } else if (type_str == "cs137" || type_str == "Cs137") {
        source_type_ = "Cs137";
    } else {
        cerr << "WARNING: only two source types are supported, they are Na22 and Cs137." << endl;
        cerr << "WARNING: the source type is set to the default one, Na22." << endl;
        source_type_ = "Na22";
    }
}

bool ComptonEdgeCalc::gen_energy_adc_vector_(EventIterator& eventIter) {
    for (int j = 0; j < 64; j++) {
        if (eventIter.t_modules.trigger_bit[j] && eventIter.t_modules.energy_adc[j] >= 4095) {
            return false;
        }
    }
    copy(eventIter.t_modules.energy_adc, eventIter.t_modules.energy_adc + 64,
         energy_adc_vector_.GetMatrixArray());
    // subtract pedestal and common noise
    int   idx = eventIter.t_modules.ct_num - 1;
    float cur_common_sum   = 0;
    int   cur_common_n     = 0;
    float cur_common_noise = 0;
    if (eventIter.t_modules.compress != 3) {
        for (int j = 0; j < 64; j++) {
            if (energy_adc_vector_(j) < 4096) {
                energy_adc_vector_(j) -= ped_mean_vector_[idx](j);
            }
            if (!eventIter.t_modules.trigger_bit[j]) {
                cur_common_sum += energy_adc_vector_(j);
                cur_common_n++;
            }
        }
    }
    if (eventIter.t_modules.compress == 0 || eventIter.t_modules.compress == 2) {
        cur_common_noise = (cur_common_n > 0 ? cur_common_sum / cur_common_n : 0);
    } else if (eventIter.t_modules.compress == 3) {
        cur_common_noise = eventIter.t_modules.common_noise;
    } else {
        cur_common_noise = 0;
    }
    for (int j = 0; j < 64; j++) {
        if (energy_adc_vector_(j) < 4096) {
            energy_adc_vector_(j) -= cur_common_noise;
        } else {
            energy_adc_vector_(j) = gRandom->Uniform(-1, 1);
        }
    }
    // crosstalk correction
    energy_adc_vector_ = xtalk_matrix_inv_[idx] * energy_adc_vector_;
    return true;
}

void ComptonEdgeCalc::fill_source_data(EventIterator& eventIter,
                                       SourceDataFile& source_data_file) {
    if (source_data_file.get_mode() != 'w')
        return;
    if (!is_ped_mean_read_) {
        cerr << "WARNING: pedestal mean vectors are not read yet. " << endl;
    }
    if (!is_xtalk_matrix_read_) {
        cerr << "WARNING: crosstalk matrixes are not read yet. " << endl;
    }
    bool overflow_flag;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Selecting and Filling Source Events Data ... " << endl;
    cout << "[ " << flush;
    eventIter.phy_trigger_set_start();
    while (eventIter.phy_trigger_next_event()) {
        cur_percent = static_cast<int>(100 * eventIter.phy_trigger_get_cur_entry() / eventIter.phy_trigger_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        if (eventIter.t_trigger.is_bad > 0 || eventIter.t_trigger.lost_count > 0) {
            continue;
        }
        overflow_flag = false;
        source_data_file.clear_cur_entry();
        source_data_file.t_source_event.type = eventIter.t_trigger.type;
        source_data_file.t_source_event.trigger_n = eventIter.t_trigger.trigger_n;
        copy(eventIter.t_trigger.trig_accepted, eventIter.t_trigger.trig_accepted + 25,
             source_data_file.t_source_event.trig_accepted);
        while (eventIter.phy_modules_next_packet()) {
            int idx = eventIter.t_modules.ct_num - 1;
            if (!gen_energy_adc_vector_(eventIter)) {
                overflow_flag = true;
                break;
            }
            copy(energy_adc_vector_.GetMatrixArray(), energy_adc_vector_.GetMatrixArray() + 64,
                 &source_data_file.t_source_event.energy_adc[idx * 64]);
            copy(eventIter.t_modules.trigger_bit, eventIter.t_modules.trigger_bit + 64,
                 &source_data_file.t_source_event.trigger_bit[idx * 64]);
            source_data_file.t_source_event.multiplicity[idx] = eventIter.t_modules.multiplicity;
        }
        if (overflow_flag) {
            continue;
        }
        if (source_type_ == "Na22" && check_na22_event_(source_data_file.t_source_event)) {
            source_data_file.event_fill();
        } else if (source_type_ == "Cs137" && check_cs137_event_(source_data_file.t_source_event)) {
            source_data_file.event_fill();
        }
    }
    cout << " DONE ]" << endl;
}

bool ComptonEdgeCalc::check_na22_event_(const SourceDataFile::SourceEvent_T source_event) {
    Bar first_bar;
    Pos first_pos;
    Bar second_bar;
    Pos second_pos;
    priority_queue<Bar> bar_queue;
    for (int i = 0; i < 25; i++) {
        if (source_event.trig_accepted[i]) {
            for (int j = 0; j < 64; j++) {
                if (source_event.trigger_bit[i * 64 + j]) {
                    bar_queue.push(Bar(source_event.energy_adc[i * 64 + j], i, j));
                }
            }
        }
    }
    if (bar_queue.empty()) {
        return false;
    }
    first_bar = bar_queue.top();
    bar_queue.pop();
    first_pos.randomize(first_bar.i, first_bar.j);
    bool found_not_adjacent = false;
    while (!bar_queue.empty()) {
        second_bar = bar_queue.top();
        bar_queue.pop();
        second_pos.randomize(second_bar.i, second_bar.j);
        if (!first_pos.is_adjacent_to(second_pos)) {
            found_not_adjacent = true;
            break;
        }
    }
    if (!found_not_adjacent) {
        return false;
    }

    // now first_pos and second_pos are ready,
    // following code should check if is Na22 event according to these two positions

    // calculate angle and distance to the 4 Na22 sources
    double src_angle[4];
    double src_distance[4];
    for (int i = 0; i < 4; i++) {
        src_angle[i] = angle_of_3_points_(SourcePos[i][0],  SourcePos[i][1],
                                          first_pos.abs_x,  first_pos.abs_y,
                                          second_pos.abs_x, second_pos.abs_y);
        src_distance[i] = distance_of_3_points_(SourcePos[i][0],  SourcePos[i][1],
                                                first_pos.abs_x,  first_pos.abs_y,
                                                second_pos.abs_x, second_pos.abs_y);
    }
    // find the largest angle
    double largest_angle = 0;
    int    largest_index = 0;
    for (int i = 0; i < 4; i++) {
        if (src_angle[i] > largest_angle) {
            largest_angle = src_angle[i];
            largest_index = i;
        }
    }
    if (largest_angle < AngleMin) {
        return false;
    }
    if (src_distance[largest_index] > DistanceMax) {
        return false;
    } else {
        return true;
    }
}

bool ComptonEdgeCalc::check_cs137_event_(const SourceDataFile::SourceEvent_T source_event) {
    if (check_na22_event_(source_event))
        return false;
    return true;
}

double ComptonEdgeCalc::angle_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2) {
    double vec1[2] = {x1 - x0, y1 - y0};
    double vec2[2] = {x2 - x0, y2 - y0};
    double norm_vec1 = TMath::Sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1]);
    double norm_vec2 = TMath::Sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1]);
    double product12 = vec1[0] * vec2[0] + vec1[1] * vec2[1];
    return TMath::ACos(product12 / (norm_vec1 * norm_vec2));
}

double ComptonEdgeCalc::distance_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2) {
    double vec01[2] = {x1 - x0, y1 - y0};
    double vec12[2] = {x2 - x1, y2 - y1};
    double norm_vec01 = TMath::Sqrt(vec01[0] * vec01[0] + vec01[1] * vec01[1]);
    double norm_vec12 = TMath::Sqrt(vec12[0] * vec12[0] + vec12[1] * vec12[1]);
    double product_01_12 = vec01[0] * vec12[0] + vec01[1] * vec12[1];
    double angle_01_12 = TMath::ACos(product_01_12 / (norm_vec01 * norm_vec12));
    return (norm_vec01 * TMath::Sin(angle_01_12));
}

bool ComptonEdgeCalc::read_ped_mean_vector(const char* filename) {
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

bool ComptonEdgeCalc::read_xtalk_matrix_inv(const char* filename) {
    TFile* xtalk_matrix_file = new TFile(filename, "READ");
    if (xtalk_matrix_file->IsZombie())
        return false;
    TMatrixF* xtalk_mat_inv;
    for (int i = 0; i < 25; i++) {
        xtalk_mat_inv = static_cast<TMatrixF*>(xtalk_matrix_file->Get(Form("xtalk_mat_inv_ct_%02d", i + 1)));
        if (xtalk_mat_inv == NULL)
            return false;
        xtalk_matrix_inv_[i] = (*xtalk_mat_inv);
        delete xtalk_mat_inv;
        xtalk_mat_inv = NULL;
    }
    xtalk_matrix_file->Close();
    delete xtalk_matrix_file;
    xtalk_matrix_file = NULL;
    is_xtalk_matrix_read_ = true;
    return true;
}

void ComptonEdgeCalc::create_spec_hist() {
    if (is_all_created_)
        return;
    delete_spec_hist();
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            spec_hist_[i][j] = new TH1F(Form("spec_hist_%02d_%02d", i + 1, j + 1),
                                        Form("Spectrum of CH %02d_%02d", i + 1, j + 1),
                                        SPEC_BINS, 0, 4096);
            spec_hist_[i][j]->SetDirectory(NULL);
            spec_func_[i][j] = new TF1(Form("spec_func_%02d_%02d", i + 1, j + 1),
                                       "[0]+[1]*TMath::Erfc((x-[2])/[3])", FUNC_MIN, FUNC_MAX);
            spec_func_[i][j]->SetParName(2, "CE");
            spec_func_[i][j]->SetParName(3, "Sigma");
        }
    }
    is_all_created_ = true;
    is_all_filled_  = false;
    is_all_fitted_  = false;
}

void ComptonEdgeCalc::delete_spec_hist() {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            if (spec_hist_[i][j] != NULL) {
                delete spec_hist_[i][j];
                spec_hist_[i][j] = NULL;
            }
            if (spec_func_[i][j] != NULL) {
                delete spec_func_[i][j];
                spec_func_[i][j] = NULL;
            }
        }
    }
    is_all_created_ = false;
    is_all_filled_  = false;
    is_all_fitted_  = false;
}

void ComptonEdgeCalc::fill_spec_hist(SourceDataFile& source_data_file) {
    if (source_data_file.get_mode() != 'r')
        return;
    if (!is_all_created_)
        return;
    if (is_all_filled_)
        return;
    Bar first_bar;
    Bar second_bar;
    priority_queue<Bar> bar_queue;
    source_data_file.event_set_start();
    while (source_data_file.event_next()) {
        while (!bar_queue.empty()) {
            bar_queue.pop();
        }
        for (int i = 0; i < 25; i++) {
            if (!source_data_file.t_source_event.trig_accepted[i])
                continue;
            for (int j = 0; j < 64; j++) {
                if (!source_data_file.t_source_event.trigger_bit[i * 64 + j])
                    continue;
                bar_queue.push(Bar(source_data_file.t_source_event.energy_adc[i * 64 + j], i, j));
            }
        }
        if (bar_queue.empty())
            continue;
        first_bar = bar_queue.top();
        if (source_type_ != "Na22") {
            spec_hist_[first_bar.i][first_bar.j]->Fill(first_bar.adc);
            continue;
        }
        bar_queue.pop();
        if (bar_queue.empty())
            continue;
        second_bar = bar_queue.top();
        spec_hist_[first_bar.i][first_bar.j]->Fill(first_bar.adc);
        spec_hist_[second_bar.i][second_bar.j]->Fill(second_bar.adc);
    }
    is_all_filled_ = true;
}

void ComptonEdgeCalc::fit_spec_hist() {
    if (!is_all_filled_)
        return;
    if (is_all_fitted_)
        return;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            // fit 1 => 1700 500
            spec_func_[i][j]->SetParameters(5, 50, 1700, 500);
            spec_hist_[i][j]->Fit(spec_func_[i][j], "RNQ");
            spec_func_[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 2 => 1900 500
            spec_func_[i][j]->SetParameters(5, 50, 1900, 500);
            spec_hist_[i][j]->Fit(spec_func_[i][j], "RNQ");
            spec_func_[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 3 => 2100 500
            spec_func_[i][j]->SetParameters(5, 50, 2100, 500);
            spec_hist_[i][j]->Fit(spec_func_[i][j], "RNQ");
            spec_func_[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 4 => 2300 500
            spec_func_[i][j]->SetParameters(5, 50, 2300, 500);
            spec_hist_[i][j]->Fit(spec_func_[i][j], "RNQ");
            spec_func_[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 5 => 1700 600
            spec_func_[i][j]->SetParameters(5, 50, 1700, 600);
            spec_hist_[i][j]->Fit(spec_func_[i][j], "RNQ");
            spec_func_[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 6 => 1900 600
            spec_func_[i][j]->SetParameters(5, 50, 1900, 600);
            spec_hist_[i][j]->Fit(spec_func_[i][j], "RNQ");
            spec_func_[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 7 => 2100 600
            spec_func_[i][j]->SetParameters(5, 50, 2100, 600);
            spec_hist_[i][j]->Fit(spec_func_[i][j], "RNQ");
            spec_func_[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 8 => 2300 600
            spec_func_[i][j]->SetParameters(5, 50, 2300, 600);
            spec_hist_[i][j]->Fit(spec_func_[i][j], "RNQ");
            spec_func_[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 9 => 1700 700
            spec_func_[i][j]->SetParameters(5, 50, 1700, 700);
            spec_hist_[i][j]->Fit(spec_func_[i][j], "RNQ");
            spec_func_[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 10 => 1900 700
            spec_func_[i][j]->SetParameters(5, 50, 1900, 700);
            spec_hist_[i][j]->Fit(spec_func_[i][j], "RNQ");
            spec_func_[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 11 => 2100 700
            spec_func_[i][j]->SetParameters(5, 50, 2100, 700);
            spec_hist_[i][j]->Fit(spec_func_[i][j], "RNQ");
            spec_func_[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 12 => 2300 700
            spec_func_[i][j]->SetParameters(5, 50, 2300, 700);
            spec_hist_[i][j]->Fit(spec_func_[i][j], "RNQ");
            spec_func_[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000) {
                if (source_type_ == "Na22") {
                    adc_per_kev[i][j] = erfc_p[i][j][2] / CE_Na22;
                } else if (source_type_ == "Cs137") {
                    adc_per_kev[i][j] = erfc_p[i][j][2] / CE_Cs137;
                }
                ce_adc_sigma[i][j] = erfc_p[i][j][3];
            } else {
                adc_per_kev[i][j] = 0;
                ce_adc_sigma[i][j] = 0;
            }
        }
    }
    is_all_fitted_ = true;
    
}

void ComptonEdgeCalc::draw_spec_count_map() {
    if (!is_all_filled_)
        return;
    spec_count_map_ = static_cast<TH2F*>(gROOT->FindObject("spec_count_map"));
    if (spec_count_map_ == NULL) {
        spec_count_map_ = new TH2F("spec_count_map", "Source Event Count Map", 40, 0, 40, 40, 0, 40);
        spec_count_map_->SetDirectory(NULL);
        spec_count_map_->GetXaxis()->SetNdivisions(40);
        spec_count_map_->GetYaxis()->SetNdivisions(40);
        for (int i = 0; i < 40; i++) {
            if (i % 8 == 0) {
                spec_count_map_->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
                spec_count_map_->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
            }
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            spec_count_map_->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, spec_hist_[i][j]->GetEntries());
        }
    }
    spec_count_map_->Draw("COLZ");
    for (int i = 0; i < 4; i++) {
        if (line_h_[i] != NULL)
            delete line_h_[i];
        line_h_[i] = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
        line_h_[i]->SetLineColor(kWhite);
        line_h_[i]->Draw("SAME");
        if (line_v_[i] != NULL)
            delete line_v_[i];
        line_v_[i] = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
        line_v_[i]->SetLineColor(kWhite);
        line_v_[i]->Draw("SAME");
    }

}

void ComptonEdgeCalc::draw_adc_per_kev() {
    if (!is_all_read_)
        return;
    adc_per_kev_map_ = static_cast<TH2F*>(gROOT->FindObject("adc_per_kev_map"));
    if (adc_per_kev_map_ == NULL) {
        adc_per_kev_map_ = new TH2F("adc_per_kev_map", "ADC/KeV of 1600 Channels", 40, 0, 40, 40, 0, 40);
        adc_per_kev_map_->SetDirectory(NULL);
        adc_per_kev_map_->GetXaxis()->SetNdivisions(40);
        adc_per_kev_map_->GetYaxis()->SetNdivisions(40);
        for (int i = 0; i < 40; i++) {
            if (i % 8 == 0) {
                adc_per_kev_map_->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
                adc_per_kev_map_->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
            }
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            adc_per_kev_map_->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, adc_per_kev[i][j]);
        }
    }
    adc_per_kev_map_->Draw("COLZ");
    for (int i = 0; i < 4; i++) {
        if (line_h_[i] != NULL)
            delete line_h_[i];
        line_h_[i] = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
        line_h_[i]->SetLineColor(kWhite);
        line_h_[i]->Draw("SAME");
        if (line_v_[i] != NULL)
            delete line_v_[i];
        line_v_[i] = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
        line_v_[i]->SetLineColor(kWhite);
        line_v_[i]->Draw("SAME");
    }
}

void ComptonEdgeCalc::draw_ce_adc_sigma() {
    if (!is_all_read_)
        return;
    ce_adc_sigma_map_ = static_cast<TH2F*>(gROOT->FindObject("ce_adc_sigma_map"));
    if (ce_adc_sigma_map_ == NULL) {
        ce_adc_sigma_map_ = new TH2F("ce_adc_sigma_map", "Sigma of CE ADC of 1600 Channels", 40, 0, 40, 40, 0, 40);
        ce_adc_sigma_map_->SetDirectory(NULL);
        ce_adc_sigma_map_->GetXaxis()->SetNdivisions(40);
        ce_adc_sigma_map_->GetYaxis()->SetNdivisions(40);
        for (int i = 0; i < 40; i++) {
            if (i % 8 == 0) {
                ce_adc_sigma_map_->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
                ce_adc_sigma_map_->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
            }
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            ce_adc_sigma_map_->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, ce_adc_sigma[i][j]);
        }
    }
    ce_adc_sigma_map_->Draw("COLZ");
    for (int i = 0; i < 4; i++) {
        if (line_h_[i + 4] != NULL)
            delete line_h_[i + 4];
        line_h_[i + 4] = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
        line_h_[i + 4]->SetLineColor(kWhite);
        line_h_[i + 4]->Draw("SAME");
        if (line_v_[i + 4] != NULL)
            delete line_v_[i + 4];
        line_v_[i + 4] = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
        line_v_[i + 4]->SetLineColor(kWhite);
        line_v_[i + 4]->Draw("SAME");
    }
}

void ComptonEdgeCalc::draw_spec_hist(int ct_i, int ch_j) {
    if (!is_all_filled_)
        return;
    if (ct_i < 0 || ct_i > 24)
        return;
    if (ch_j < 0 || ch_j > 63)
        return;
    if (is_all_fitted_) {
        spec_hist_[ct_i][ch_j]->Fit(spec_func_[ct_i][ch_j], "RQ");
    } else {
        spec_hist_[ct_i][ch_j]->Draw();
    }
}

bool ComptonEdgeCalc::write_adc_per_kev_vector(const char* filename, SourceDataFile& source_data_file) {
    if (!is_all_fitted_)
        return false;
    TFile* adc_per_kev_file = new TFile(filename, "RECREATE");
    if (adc_per_kev_file->IsZombie())
        return false;
    TVectorF* tmp_vec;
    for (int i = 0; i < 25; i++) {
        tmp_vec = new TVectorF(64, adc_per_kev[i]);
        tmp_vec->Write(Form("adc_per_kev_vec_ct_%02d", i + 1));
        delete tmp_vec;
        tmp_vec = new TVectorF(64, ce_adc_sigma[i]);
        tmp_vec->Write(Form("ce_adc_sgm_vec_ct_%02d", i + 1));
        delete tmp_vec;
        tmp_vec = NULL;
    }
    TNamed* tmp_meta;
    // m_dattype
    tmp_meta = new TNamed("m_dattype", "POLAR ADC/KeV VECTOR");
    tmp_meta->Write();
    delete tmp_meta;
    // m_version
    tmp_meta = new TNamed("m_version",  (SW_NAME + " " + SW_VERSION).c_str());
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
    tmp_meta = new TNamed("m_fromfile", source_data_file.get_fromfile_str().c_str());
    tmp_meta->Write();
    delete tmp_meta;
    // m_gps_span
    tmp_meta = new TNamed("m_gps_span", source_data_file.get_gps_span_str().c_str());
    tmp_meta->Write();
    delete tmp_meta;
    tmp_meta = NULL;
    adc_per_kev_file->Close();
    delete adc_per_kev_file;
    adc_per_kev_file = NULL;
    return true;
}

bool ComptonEdgeCalc::read_adc_per_kev_vector(const char* filename) {
    TFile* adc_per_kev_file = new TFile(filename, "READ");
    if (adc_per_kev_file->IsZombie())
        return false;
    TVectorF* adc_per_kev_vec;
    TVectorF* ce_adc_sigma_vec;
    for (int i = 0; i < 25; i++) {
        adc_per_kev_vec  = static_cast<TVectorF*>(adc_per_kev_file->Get(Form("adc_per_kev_vec_ct_%02d", i + 1)));
        ce_adc_sigma_vec = static_cast<TVectorF*>(adc_per_kev_file->Get(Form("ce_adc_sgm_vec_ct_%02d", i + 1)));
        if (adc_per_kev_vec == NULL || ce_adc_sigma_vec == NULL)
            return false;
        for (int j = 0; j < 64; j++) {
            adc_per_kev[i][j]  = (*adc_per_kev_vec)(j);
            ce_adc_sigma[i][j] = (*ce_adc_sigma_vec)(j);
        }
        delete adc_per_kev_vec;
        adc_per_kev_vec = NULL;
        delete ce_adc_sigma_vec;
        ce_adc_sigma_vec = NULL;
    }
    adc_per_kev_file->Close();
    delete adc_per_kev_file;
    adc_per_kev_file = NULL;
    is_all_read_ = true;
    return true;
}
