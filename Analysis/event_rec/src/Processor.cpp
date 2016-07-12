#include "Processor.hpp"

using namespace std;

Processor::Processor(OptionsManager* my_options_mgr, TApplication* my_rootapp) {
    cur_options_mgr_ = my_options_mgr;
    cur_rootapp_     = my_rootapp;
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
    
    return true;
}

void Processor::reconstruct_all_events_(EventIterator& eventIter, RecEventDataFile& rec_event_data_file) {

}
