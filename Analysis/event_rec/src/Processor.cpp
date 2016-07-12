#include "Processor.hpp"

using namespace std;

Processor::Processor(OptionsManager* my_options_mgr, TApplication* my_rootapp) {
    cur_options_mgr_ = my_options_mgr;
    cur_rootapp_     = my_rootapp;

    for (int i = 0; i < 25; i++) {
        ped_mean_vector_[i].ResizeTo(64);
        xtalk_matrix_inv_[i].ResizeTo(64, 64);
        adc_per_kev_vector_[i].ResizeTo(64);
    }
    energy_dep_vector_.ResizeTo(64);
}

Processor::~Processor() {

}

int Processor::start_process() {
    cout << "ped_vector_filename => " << cur_options_mgr_->ped_vector_filename.Data() << endl;
    cout << "xtalk_matrix_filename => " << cur_options_mgr_->xtalk_matrix_filename.Data() << endl;
    cout << "adc_per_kev_filename => " << cur_options_mgr_->adc_per_kev_filename.Data() << endl;
    cout << "decoded_data_filename => " << cur_options_mgr_->decoded_data_filename.Data() << endl;
    cout << "begin_gps => " << cur_options_mgr_->begin_gps.Data() << endl;
    cout << "end_gps => " << cur_options_mgr_->end_gps.Data() << endl;
    cout << "rec_event_data_filename => " << cur_options_mgr_->rec_event_data_filename.Data() << endl;
    return 0;
}

bool Processor::read_ped_mean_vector_(const char* filename) {
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
    return true;
}

bool Processor::read_xtalk_matrix_inv_(const char* filename) {
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
    return true;
}

bool Processor::read_adc_per_kev_vector_(const char* filename) {
    TFile* adc_per_kev_file = new TFile(filename, "READ");
    if (adc_per_kev_file->IsZombie())
        return false;
    TVectorF* adc_per_kev_vec;
    for (int i = 0; i < 25; i++) {
        adc_per_kev_vec  = static_cast<TVectorF*>(adc_per_kev_file->Get(Form("adc_per_kev_vec_ct_%02d", i + 1)));
        if (adc_per_kev_vec == NULL)
            return false;
        adc_per_kev_vector_[i] = (*adc_per_kev_vec);
        delete adc_per_kev_vec;
        adc_per_kev_vec = NULL;
    }
    adc_per_kev_file->Close();
    delete adc_per_kev_file;
    adc_per_kev_file = NULL;
    return true;
}

bool Processor::gen_energy_vector_(EventIterator& eventIter) {
    for (int j = 0; j < 64; j++) {
        if (eventIter.t_modules.trigger_bit[j] && eventIter.t_modules.energy_adc[j] == 4095) {
            return false;
        }
    }
    copy(eventIter.t_modules.energy_adc, eventIter.t_modules.energy_adc + 64,
         energy_dep_vector_.GetMatrixArray());
    // substract pedestal and common noise
    int   idx = eventIter.t_modules.ct_num - 1;
    float cur_common_sum   = 0;
    int   cur_common_n     = 0;
    float cur_common_noise = 0;
    if (eventIter.t_modules.compress != 3) {
        for (int j = 0; j < 64; j++) {
            if (energy_dep_vector_(j) < 4096) {
                energy_dep_vector_(j) -= ped_mean_vector_[idx](j);
            }
            if (!eventIter.t_modules.trigger_bit[j]) {
                cur_common_sum += energy_dep_vector_(j);
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
        if (energy_dep_vector_(j) < 4096) {
            energy_dep_vector_(j) -= cur_common_noise;
        } else {
            energy_dep_vector_(j) = gRandom->Uniform(-1, 1);
        }
    }
    // crosstalk correction
    energy_dep_vector_ = xtalk_matrix_inv_[idx] * energy_dep_vector_;
    // deposited energy reconstruction
    for (int j = 0; j < 64; j++) {
        if (adc_per_kev_vector_[idx](j) > 0) {
            energy_dep_vector_(j) = energy_dep_vector_(j) / adc_per_kev_vector_[idx](j);
        } else {
            energy_dep_vector_(j) = -1;
        }
    }
    return true;
}

void Processor::reconstruct_all_events_(EventIterator& eventIter, RecEventDataFile& rec_event_data_file) {

}
