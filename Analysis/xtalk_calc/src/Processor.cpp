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
    cout << "action 1" << endl;
    cout << "ped_vec: " << cur_options_mgr_->ped_vector_filename << endl;
    cout << "decoded: " << cur_options_mgr_->decoded_data_filename << endl;
    cout << "xtalk_d: " << cur_options_mgr_->xtalk_data_filename << endl;
    cout << "rw_mode: " << cur_options_mgr_->rw_mode << endl;
    cout << "begin_g: " << cur_options_mgr_->begin_gps << endl;
    cout << "end_gps: " << cur_options_mgr_->end_gps << endl;
    return 0;
}

int Processor::do_action_2_() {
    cout << "action 2" << endl;
    cout << "xtalk_d: " << cur_options_mgr_->xtalk_data_filename << endl;
    cout << "rw_mode: " << cur_options_mgr_->rw_mode << endl;
    cout << "cr_num:  " << cur_options_mgr_->ct_num << endl;
    cout << "show_fl: " << cur_options_mgr_->show_flag;
    return 0;
}

int Processor::do_action_3_() {
    cout << "action 3" << endl;
    cout << "xtalk_d: " << cur_options_mgr_->xtalk_data_filename << endl;
    cout << "rw_mode: " << cur_options_mgr_->rw_mode << endl;
    cout << "xtalk_m: " << cur_options_mgr_->xtalk_matrix_filename << endl;
    cout << "xtalk_r: " << cur_options_mgr_->xtalk_matrix_read_flag << endl;
    return 0;
}

int Processor::do_action_4_() {
    cout << "action 4" << endl;
    cout << "xtalk_m: " << cur_options_mgr_->xtalk_matrix_filename << endl;
    cout << "xtalk_r: " << cur_options_mgr_->xtalk_matrix_read_flag << endl;
    return 0;
}
