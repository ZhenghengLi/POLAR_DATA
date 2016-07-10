#include "ComptonEdgeCalc.hpp"

using namespace std;

ComptonEdgeCalc::ComptonEdgeCalc() {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            spec_func_[i][j] = NULL;
            spec_hist_[i][j] = NULL;
        }
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

}

void ComptonEdgeCalc::set_source_type(string type_str) {
    if (type_str == "na22" || type_str == "Na22") {
        source_type_ = "Na22";
    } else if (type_str == "cs137" || type_str == "Cs137") {
        source_type_ = "Cs137";
    } else {
        cerr << "WARNING: only two source types are supported, they are Na22 and Cs137." << endl;
        cerr << "WARNING: the source type is set to the default one, Na22." << endl;
    }
}

void ComptonEdgeCalc::gen_energy_adc_vector_(EventIterator& eventIter) {
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
}

void ComptonEdgeCalc::fill_spec_data(EventIterator& eventIter,
                                     SpecDataFile& spec_data_file) {
    if (spec_data_file.get_mode() != 'w')
        return;
    if (!is_ped_mean_read_) {
        cerr << "WARNING: pedestal mean vectors are not read yet. " << endl;
    }
    if (!is_xtalk_matrix_read_) {
        cerr << "WARNING: crosstalk matrixes are not read yet. " << endl;
    }
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
        spec_data_file.clear_cur_entry();
        spec_data_file.t_source_event.type = eventIter.t_trigger.type;
        spec_data_file.t_source_event.trigger_n = eventIter.t_trigger.trigger_n;
        copy(eventIter.t_trigger.trig_accepted, eventIter.t_trigger.trig_accepted + 25,
             spec_data_file.t_source_event.trig_accepted);
        while (eventIter.phy_modules_next_packet()) {
            int idx = eventIter.t_modules.ct_num - 1;
            gen_energy_adc_vector_(eventIter);
            copy(energy_adc_vector_.GetMatrixArray(), energy_adc_vector_.GetMatrixArray() + 64,
                 &spec_data_file.t_source_event.energy_adc[idx * 64]);
            copy(eventIter.t_modules.trigger_bit, eventIter.t_modules.trigger_bit + 64,
                 &spec_data_file.t_source_event.trigger_bit[idx * 64]);
            spec_data_file.t_source_event.multiplicity[idx] = eventIter.t_modules.multiplicity;
        }
        if (source_type_ == "Na22" && check_na22_event_(spec_data_file.t_source_event)) {
            spec_data_file.event_fill();
        } else if (source_type_ == "Cs137" && check_cs137_event_(spec_data_file.t_source_event)) {
            spec_data_file.event_fill();
        }
    }
    cout << " DONE ]" << endl;
}

bool ComptonEdgeCalc::check_na22_event_(const SpecDataFile::SourceEvent_T source_event) {
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

bool ComptonEdgeCalc::check_cs137_event_(const SpecDataFile::SourceEvent_T source_event) {

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
