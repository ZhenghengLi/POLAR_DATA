#include "Processor.hpp"

using namespace std;

Processor::Processor(OptionsManager* my_options_mgr, TApplication* my_rootapp) {
    cur_options_mgr_ = my_options_mgr;
    cur_rootapp_     = my_rootapp;
}

Processor::~Processor() {
    
}

int Processor::start_process() {
    switch (cur_options_mgr_->action) {
    case 1:
        return do_action_1_();
    case 2:
        return do_action_2_();
    case 3:
        return do_action_3_();
    case 4:
        return do_action_4_();
    default:
        return 1;
    }
}

int Processor::do_action_1_() {
    cout << "action 1: " << endl;
    cout << "begin_gps => " << cur_options_mgr_->begin_gps.Data() << endl;
    cout << "end_gps => " << cur_options_mgr_->end_gps.Data() << endl;
    cout << "decoded_data_filename => " << cur_options_mgr_->decoded_data_filename.Data() << endl;
    cout << "rw_mode => " << cur_options_mgr_->rw_mode << endl;
    cout << "ped_vector_filename => " << cur_options_mgr_->ped_vector_filename.Data() << endl;
    cout << "xtalk_matrix_filename => " << cur_options_mgr_->xtalk_matrix_filename.Data() << endl;
    cout << "source_type => " << cur_options_mgr_->source_type.Data() << endl;
    cout << "spec_data_filename => " << cur_options_mgr_->spec_data_filename.Data() << endl;
    return 0;
}

int Processor::do_action_2_() {
    cout << "action 2: " << endl;
    cout << "spec_data_filename => " << cur_options_mgr_->spec_data_filename.Data() << endl;
    cout << "rw_mode => " <<  cur_options_mgr_->rw_mode << endl;
    cout << "show_flag => " << cur_options_mgr_->show_flag << endl;
    return 0;
}

int Processor::do_action_3_() {
    cout << "action 3: " << endl;
    cout << "spec_data_filename => " << cur_options_mgr_->spec_data_filename.Data() << endl;
    cout << "rw_mode => " <<  cur_options_mgr_->rw_mode << endl;
    cout << "adc_per_kev_filename => " << cur_options_mgr_->adc_per_kev_filename.Data() << endl;
    cout << "adc_per_kev_read_flag => " << cur_options_mgr_->adc_per_kev_read_flag << endl;
    return 0;
}

int Processor::do_action_4_() {
    cout << "action 4: " << endl;
    cout << "adc_per_kev_filename => " << cur_options_mgr_->adc_per_kev_filename.Data() << endl;
    cout << "adc_per_kev_read_flag => " << cur_options_mgr_->adc_per_kev_read_flag << endl;
    return 0;
}
