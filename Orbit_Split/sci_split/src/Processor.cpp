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
