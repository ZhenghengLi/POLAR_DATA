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

    return true;
}

bool Processor::read_xtalk_matrix_inv_(const char* filename) {

    return true;
}

bool Processor::read_adc_per_kev_vector_(const char* filename) {

    return true;
}

bool Processor::gen_energy_vector_(EventIterator& eventIter) {

    return true;
}

void Processor::reconstruct_all_events_(EventIterator& eventIter, RecEventDataFile& rec_event_data_file) {

}
