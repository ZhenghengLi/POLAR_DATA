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
    default:
        return 1;
    }
}

int Processor::do_action_1_() {
    if (!sciIter_.open(cur_options_mgr_->decoded_data_filename.Data(),
                      cur_options_mgr_->begin_gps.Data(), cur_options_mgr_->end_gps.Data())) {
        cerr << "root file open failed: " << cur_options_mgr_->decoded_data_filename.Data() << endl;
        return 1;
    }
    if (!ped_data_file_.open(cur_options_mgr_->ped_data_filename.Data(), 'w')) {
        cerr << "root file open failed: " << cur_options_mgr_->ped_data_filename.Data() << endl;
        return 1;
    }
    sciIter_.print_file_info();
    cout << "----------------------------------------------------------" << endl;
    ped_mean_calc_.fill_ped_data(sciIter_, ped_data_file_);
    ped_data_file_.write_all_tree();
    ped_data_file_.write_fromfile(sciIter_.get_filename().c_str());
    ped_data_file_.write_gps_span(sciIter_.get_ped_first_gps().c_str(),
                                  sciIter_.get_ped_last_gps().c_str());
    ped_data_file_.write_lasttime();
    ped_data_file_.close();
    sciIter_.close();
    return 0;
}

int Processor::do_action_2_() {
    if (!ped_data_file_.open(cur_options_mgr_->ped_data_filename.Data(), 'r')) {
        cerr << "root file open failed: " << cur_options_mgr_->ped_data_filename.Data() << endl;
        return 1;
    }
    ped_mean_calc_.create_ped_hist();
    cout << "Filling pedestal histogram ..." << endl;
    ped_mean_calc_.fill_ped_hist(ped_data_file_);
    cout << "Fitting pedestal histogram ..." << endl;
    ped_mean_calc_.fit_ped_hist();
    cout << "Showing pedestal mean map ..." << endl;
    ped_mean_show_.show_map(ped_mean_calc_);
    cur_rootapp_->Run();
    return 0;
}

int Processor::do_action_3_() {
    if (!ped_data_file_.open(cur_options_mgr_->ped_data_filename.Data(), 'r')) {
        cerr << "root file open failed: " << cur_options_mgr_->ped_data_filename.Data() << endl;
        return 1;
    }
    ped_mean_calc_.create_ped_hist();
    cout << "Filling pedestal histogram ..." << endl;
    ped_mean_calc_.fill_ped_hist(ped_data_file_);
    cout << "Fitting pedestal histogram ..." << endl;
    ped_mean_calc_.fit_ped_hist();
    cout << "Writting pedestal vector ..." << endl;
    if (ped_mean_calc_.write_ped_vector(cur_options_mgr_->ped_vector_filename.Data(),
                                         ped_data_file_)) {
        cout << "Successfully writted pedestal vector." << endl;
    } else {
        cout << "ERROR: failed to write pedestal vector." << endl;
    }
    return 0;
}
