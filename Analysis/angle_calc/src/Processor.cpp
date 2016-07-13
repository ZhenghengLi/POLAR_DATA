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
    if (!rec_event_data_file_.open(cur_options_mgr_->rec_event_data_filename.Data(), 'r')) {
        cerr << "root file open failed: " << cur_options_mgr_->rec_event_data_filename.Data() << endl;
        return 1;
    }
    if (!angle_data_file_.open(cur_options_mgr_->angle_data_filename.Data(), 'w')) {
        cerr << "root file open failed: " << cur_options_mgr_->angle_data_filename.Data() << endl;
        return 1;
    }
    event_filter_.set_low_energy_thr(cur_options_mgr_->low_energy_thr);
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Calculating Scattering Angle ..." << endl;
    cout << "[ " << flush;
    rec_event_data_file_.event_set_start();
    while (rec_event_data_file_.event_next()) {
        cur_percent = static_cast<int>(100 * rec_event_data_file_.event_get_cur_entry() / rec_event_data_file_.event_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        if (!event_filter_.find_first_two_bars(rec_event_data_file_.t_rec_event)) {
            continue;
        }
        if (event_filter_.check(rec_event_data_file_.t_rec_event)) {
            angle_data_file_.t_angle.first_ij[0]  = event_filter_.first_pos.i;
            angle_data_file_.t_angle.first_ij[1]  = event_filter_.first_pos.j;
            angle_data_file_.t_angle.second_ij[0] = event_filter_.second_pos.i;
            angle_data_file_.t_angle.second_ij[1] = event_filter_.second_pos.j;
            angle_data_file_.t_angle.rand_angle   = event_filter_.first_pos.angle_to(event_filter_.second_pos);
            angle_data_file_.angle_fill();
        }
    }
    cout << " DONE ]" << endl;
    angle_data_file_.write_all_tree();
    angle_data_file_.write_fromfile(rec_event_data_file_.get_fromfile_str().c_str());
    angle_data_file_.write_gps_span(rec_event_data_file_.get_gps_span_str().c_str());
    angle_data_file_.write_lasttime();
    angle_data_file_.close();
    rec_event_data_file_.close();
    
    return 0;
}

int Processor::do_action_2_() {
    cout << cur_options_mgr_->angle_data_filename.Data() << endl;
    cout << "rw_mode => " << cur_options_mgr_->rw_mode << endl;
    cout << "show_flag => " << cur_options_mgr_->show_flag << endl;
    return 0;
}
