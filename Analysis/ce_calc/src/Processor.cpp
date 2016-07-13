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
    if (!source_data_file_.open(cur_options_mgr_->source_data_filename.Data(), 'w')) {
        cerr << "root file open failed: " << cur_options_mgr_->source_data_filename.Data() << endl;
    }
    eventIter_.print_file_info();
    cout << "----------------------------------------------------------" << endl;
    compton_edge_calc_.fill_source_data(eventIter_, source_data_file_);
    source_data_file_.write_all_tree();
    source_data_file_.write_meta("m_dattype", "POLAR SOURCE EVENT DATA", false);
    source_data_file_.write_meta("m_version", (SW_NAME + " " + SW_VERSION).c_str(), false);
    source_data_file_.write_fromfile(eventIter_.get_filename().c_str());
    source_data_file_.write_gps_span(eventIter_.get_phy_first_gps().c_str(),
                                   eventIter_.get_phy_last_gps().c_str());
    source_data_file_.write_lasttime();
    source_data_file_.close();
    eventIter_.close();
    return 0;
}

int Processor::do_action_2_() {
    compton_edge_calc_.set_source_type(cur_options_mgr_->source_type.Data());
    if (!source_data_file_.open(cur_options_mgr_->source_data_filename.Data(), 'r')) {
        cerr << "root file open failed: " << cur_options_mgr_->source_data_filename.Data() << endl;
        return 1;
    }
    compton_edge_calc_.create_spec_hist();
    cout << "Filling spectrum histogram ..." << endl;
    compton_edge_calc_.fill_spec_hist(source_data_file_);
    if (cur_options_mgr_->fit_flag) {
        cout << "Fitting spectrum histogram ..." << endl;
        compton_edge_calc_.fit_spec_hist();
    }
    cout << "Showing source event count map ..." << endl;
    spectrum_show_.show_map(compton_edge_calc_);
    cur_rootapp_->Run();
    return 0;
}

int Processor::do_action_3_() {
    if (!source_data_file_.open(cur_options_mgr_->source_data_filename.Data(), 'r')) {
        cerr << "root file open failed: " << cur_options_mgr_->source_data_filename.Data() << endl;
        return 1;
    }
    compton_edge_calc_.create_spec_hist();
    cout << "Filling spectrum histogram ..." << endl;
    compton_edge_calc_.fill_spec_hist(source_data_file_);
    cout << "Fitting spectrum histogram ..." << endl;
    compton_edge_calc_.fit_spec_hist();
    cout << "Writting ADC/Kev vector ..." << endl;
    if (compton_edge_calc_.write_adc_per_kev_vector(cur_options_mgr_->adc_per_kev_filename.Data(), source_data_file_)) {
        cout << "Successfully writted ADC/KeV vector." << endl;
    } else {
        cout << "ERROR: failed to write ADC/KeV vector." << endl;
    }
    return 0;
}

int Processor::do_action_4_() {
    if (!compton_edge_calc_.read_adc_per_kev_vector(cur_options_mgr_->adc_per_kev_filename.Data())) {
        cerr << "read ADC/KeV vector file failed: " << cur_options_mgr_->adc_per_kev_filename.Data() << endl;
        return 1;
    }
    cout << "Showing ADC/KeV and Sigma of CE ADC ... " << endl;
    spectrum_show_.show_adc_per_kev(compton_edge_calc_);
    cur_rootapp_->Run();
    return 0;
}
