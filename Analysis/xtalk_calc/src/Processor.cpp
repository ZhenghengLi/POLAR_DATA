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
    if (!xtalk_matrix_calc_.read_ped_mean_vector(cur_options_mgr_->ped_vector_filename.Data())) {
        cerr << "read pedestal vector file failed: " << cur_options_mgr_->ped_vector_filename.Data() << endl;
        return 1;
    }
    if (!sciIter_.open(cur_options_mgr_->decoded_data_filename.Data(),
                       cur_options_mgr_->begin_gps.Data(), cur_options_mgr_->end_gps.Data())) {
        cerr << "root file open failed: " << cur_options_mgr_->decoded_data_filename.Data() << endl;
        return 1;
    }
    if (!xtalk_data_file_.open(cur_options_mgr_->xtalk_data_filename.Data(), 'w')) {
        cerr << "root file open failed: " << cur_options_mgr_->xtalk_data_filename.Data() << endl;
        return 1;
    }
    sciIter_.print_file_info();
    cout << "----------------------------------------------------------" << endl;
    xtalk_matrix_calc_.fill_xtalk_data(sciIter_, xtalk_data_file_);
    xtalk_data_file_.write_all_tree();
    xtalk_data_file_.write_fromfile(sciIter_.get_filename().c_str());
    xtalk_data_file_.write_gps_span(sciIter_.get_phy_first_gps().c_str(),
                                    sciIter_.get_phy_last_gps().c_str());
    xtalk_data_file_.write_lasttime();
    xtalk_data_file_.close();
    return 0;
}

int Processor::do_action_2_() {
    if (!xtalk_data_file_.open(cur_options_mgr_->xtalk_data_filename.Data(), 'r')) {
        cerr << "root file open failed: " << cur_options_mgr_->xtalk_data_filename.Data() << endl;
        return 1;
    }
    xtalk_matrix_calc_.create_xtalk_hist();
    cout << "Filling crosstalk histogram ..." << endl;
    xtalk_matrix_calc_.fill_xtalk_hist(cur_options_mgr_->ct_num - 1,
                                       xtalk_data_file_);
    cout << "Fitting crosstalk histogram ..." << endl;
    xtalk_matrix_calc_.fit_xtalk_hist();
    cout << "Show crosstalk matrix map ..." << endl;
    xtalk_matrix_show_.show_mod_map_cur(xtalk_matrix_calc_);
    cur_rootapp_->Run();
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
