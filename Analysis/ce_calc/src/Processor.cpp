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

    compton_edge_calc_.set_source_type(cur_options_mgr_->source_type.Data());
    if (!compton_edge_calc_.read_ped_mean_vector(cur_options_mgr_->ped_vector_filename.Data())) {
        cerr << "read pedestal vector file failed: " << cur_options_mgr_->ped_vector_filename.Data() << endl;
        return 1;
    }
    if (!cur_options_mgr_->xtalk_matrix_filename.IsNull()) {
        if (!compton_edge_calc_.read_xtalk_matrix_inv(cur_options_mgr_->xtalk_matrix_filename.Data())) {
            cerr << "read crosstalk matrix file failed: " << cur_options_mgr_->xtalk_matrix_filename.Data() << endl;
            return 1;
        }
    }
    if (!eventIter_.open(cur_options_mgr_->decoded_data_filename.Data(),
                         cur_options_mgr_->begin_gps.Data(), cur_options_mgr_->end_gps.Data())) {
        cerr << "root file open failed: " << cur_options_mgr_->decoded_data_filename.Data() << endl;
        return 1;
    }
    if (!spec_data_file_.open(cur_options_mgr_->spec_data_filename.Data(), 'w')) {
        cerr << "root file open failed: " << cur_options_mgr_->spec_data_filename.Data() << endl;
    }
    eventIter_.print_file_info();
    cout << "----------------------------------------------------------" << endl;
    compton_edge_calc_.fill_spec_data(eventIter_, spec_data_file_);
    spec_data_file_.write_all_tree();
    spec_data_file_.write_fromfile(eventIter_.get_filename().c_str());
    spec_data_file_.write_gps_span(eventIter_.get_phy_first_gps().c_str(),
                                   eventIter_.get_phy_last_gps().c_str());
    spec_data_file_.write_lasttime();
    spec_data_file_.close();
    eventIter_.close();
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
