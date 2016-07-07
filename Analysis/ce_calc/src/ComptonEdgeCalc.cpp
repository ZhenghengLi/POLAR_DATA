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

    return true;
}

bool ComptonEdgeCalc::check_cs137_event_(const SpecDataFile::SourceEvent_T source_event) {

    return true;
}
