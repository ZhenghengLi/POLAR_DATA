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
    default:
        return 1;
    }
}

int Processor::do_action_1_() {
    cout << "rec_event_data_filename => " << cur_options_mgr_->rec_event_data_filename.Data() << endl;
    cout << "angle_data_filename => " << cur_options_mgr_->angle_data_filename.Data() << endl;
    cout << "rw_mode => " << cur_options_mgr_->rw_mode << endl;
    cout << "low_energy_thr => " << cur_options_mgr_->low_energy_thr << endl;
    return 0;
}

int Processor::do_action_2_() {
    cout << cur_options_mgr_->angle_data_filename.Data() << endl;
    cout << "rw_mode => " << cur_options_mgr_->rw_mode << endl;
    cout << "show_flag => " << cur_options_mgr_->show_flag << endl;
    return 0;
}
