#include "Processor.hpp"

using namespace std;

Processor::Processor(OptionsManager* my_options_mgr) {
    cur_options_mgr_ = my_options_mgr;
    scifile_r_len_ = cur_options_mgr_->raw_file_vector.size();
    scifile_r_arr_ = new SciFileR[scifile_r_len_];
}

Processor::~Processor() {
    delete [] scifile_r_arr_;
}

bool Processor::open_and_check() {
    if (scifile_r_len_ == 1) {
        cout << "Opening file: " << cur_options_mgr_->raw_file_vector[0].Data() << " ... " << endl;
        if (scifile_r_arr_[0].open(cur_options_mgr_->raw_file_vector[0].Data(),
                                   cur_options_mgr_->gps_begin.Data(),
                                   cur_options_mgr_->gps_end.Data())) {
            scifile_r_arr_[0].print_file_info();
            return true;
        } else {
            cout << "Error: file open failed." << endl;
            scifile_r_arr_[0].close();
            return false;
        }
    } else {
        bool open_result;
        for (int i = 0; i < scifile_r_len_; i++) {
            cout << "Opening file: " << cur_options_mgr_->raw_file_vector[i].Data() << " ... " << endl;
            if (i == 0) {
                open_result = scifile_r_arr_[i].open(cur_options_mgr_->raw_file_vector[i].Data(),
                                                     cur_options_mgr_->gps_begin.Data(),
                                                     "end");
            } else if (i == scifile_r_len_ - 1) {
                open_result = scifile_r_arr_[i].open(cur_options_mgr_->raw_file_vector[i].Data(),
                                                     "begin",
                                                     cur_options_mgr_->gps_end.Data());
            } else {
                open_result = scifile_r_arr_[i].open(cur_options_mgr_->raw_file_vector[i].Data(),
                                                     "begin",
                                                     "end");
            }
            if (open_result) {
                scifile_r_arr_[i].print_file_info();
                if (i > 0) {
                    if (scifile_r_arr_[i].get_gps_value_first() - scifile_r_arr_[i - 1].get_gps_value_last() > GPS_DIFF_MAX) {
                        cout << "Error: two files cannot connect in GPS time." << endl;
                        for (int j = 0; j <= i; j++) {
                            scifile_r_arr_[j].close();
                        }
                        return false;
                    } else if (scifile_r_arr_[i].get_gps_value_first() - scifile_r_arr_[i - 1].get_gps_value_last() <= -1) {
                        cout << "Error: two files have overlap in GPS time." << endl;
                        for (int j = 0; j <= i; j++) {
                            scifile_r_arr_[j].close();
                        }
                        return false;
                    }
                }
            } else {
                cout << "Error: file open failed." << endl;
                for (int j = 0; j <= i; j++) {
                    scifile_r_arr_[j].close();
                }
                return false;
            }
        }
        return true;
    }
}

void Processor::set_start() {
    cur_index_ = -1;
    reach_end_ = false;
}

bool Processor::next_file(SciFileW& scifile_w) {
    if (reach_end_)
        return false;
    cur_index_++;
    if (cur_index_ < scifile_r_len_) {
        scifile_w.set_scifile_r(&scifile_r_arr_[cur_index_]);
        return true;
    } else {
        reach_end_ = true;
        return false;
    }
}

void Processor::write_the_file(SciFileW& scifile_w) {
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "Processing file: " << cur_options_mgr_->raw_file_vector[cur_index_] << " ... " << endl;
    scifile_w.write_phy_modules();
    scifile_w.write_phy_trigger();
    scifile_w.write_ped_modules();
    scifile_w.write_ped_trigger();
}

void Processor::write_meta_info(SciFileW& scifile_w) {

}
