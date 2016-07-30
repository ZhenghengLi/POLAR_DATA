#include "Processor.hpp"

using namespace std;

Processor::Processor(OptionsManager* my_options_mgr) {
    cur_options_mgr_ = my_options_mgr;
    hkfile_r_len_ = cur_options_mgr_->raw_file_vector.size();
    hkfile_r_arr_ = new HkFileR[hkfile_r_len_];
}

Processor::~Processor() {
    delete [] hkfile_r_arr_;
}

bool Processor::open_and_check() {
    if (hkfile_r_len_ == 1) {
        cout << "Opening file: " << cur_options_mgr_->raw_file_vector[0].Data() << " ... " << endl;
        if (hkfile_r_arr_[0].open(cur_options_mgr_->raw_file_vector[0].Data(),
                                   cur_options_mgr_->gps_begin.Data(),
                                   cur_options_mgr_->gps_end.Data())) {
            hkfile_r_arr_[0].print_file_info();
            return true;
        } else {
            cout << "Error: file open failed." << endl;
            hkfile_r_arr_[0].close();
            return false;
        }
    } else {
        bool open_result;
        for (int i = 0; i < hkfile_r_len_; i++) {
            cout << "Opening file: " << cur_options_mgr_->raw_file_vector[i].Data() << " ... " << endl;
            if (i == 0) {
                open_result = hkfile_r_arr_[i].open(cur_options_mgr_->raw_file_vector[i].Data(),
                                                    cur_options_mgr_->gps_begin.Data(),
                                                    "end");
            } else if (i == hkfile_r_len_ - 1) {
                open_result = hkfile_r_arr_[i].open(cur_options_mgr_->raw_file_vector[i].Data(),
                                                    "begin",
                                                    cur_options_mgr_->gps_end.Data());
            } else {
                open_result = hkfile_r_arr_[i].open(cur_options_mgr_->raw_file_vector[i].Data(),
                                                    "begin",
                                                    "end");
            }
            if (open_result) {
                hkfile_r_arr_[i].print_file_info();
                if (i > 0) {
                    if (hkfile_r_arr_[i].get_gps_value_first() - hkfile_r_arr_[i - 1].get_gps_value_last() > GPS_DIFF_MAX) {
                        cout << "Error: two files cannot connect in GPS time." << endl;
                        for (int j = 0; j <= i; j++) {
                            hkfile_r_arr_[j].close();
                        }
                        return false;
                    } else if (hkfile_r_arr_[i].get_gps_value_first() - hkfile_r_arr_[i - 1].get_gps_value_last() <= -1) {
                        cout << "Error: two files have overlap in GPS time." << endl;
                        for (int j = 0; j <= i; j++) {
                            hkfile_r_arr_[j].close();
                        }
                        return false;
                    }
                }
            } else {
                cout << "Error: file open failed." << endl;
                for (int j = 0; j <= i; j++) {
                    hkfile_r_arr_[j].close();
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

bool Processor::next_file(HkFileW& hkfile_w) {
    if (reach_end_)
        return false;
    cur_index_++;
    if (cur_index_ < hkfile_r_len_) {
        hkfile_w.set_hkfile_r(&hkfile_r_arr_[cur_index_]);
        return true;
    } else {
        reach_end_ = true;
        return false;
    }
}

void Processor::write_the_file(HkFileW& hkfile_w) {
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "Processing file: " << cur_options_mgr_->raw_file_vector[cur_index_] << " ... " << endl;
    hkfile_w.write_hk_obox();
    hkfile_w.write_hk_ibox();
    // close current reading file
    hkfile_r_arr_[cur_index_].close();
}

void Processor::write_meta_info(HkFileW& hkfile_w) {
    // dattype
    hkfile_w.write_meta("m_dattype", "POLAR 1R LEVEL HK DECODED DATA");
    // version
    hkfile_w.write_meta("m_version", (SW_NAME + " " + SW_VERSION).c_str());
    // gentime
    TTimeStamp* cur_time = new TTimeStamp();
    hkfile_w.write_meta("m_gentime", cur_time->AsString("lc"));
    delete cur_time;
    cur_time = NULL;
    // dcdfile
    TSystem sys;
    string dcd_file_list = sys.BaseName(cur_options_mgr_->raw_file_vector[0].Data());
    for (int i = 1; i < hkfile_r_len_; i++) {
        dcd_file_list += "; ";
        dcd_file_list += sys.BaseName(cur_options_mgr_->raw_file_vector[i].Data());
    }
    hkfile_w.write_meta("m_dcdfile", dcd_file_list.c_str());
    // gps span
    hkfile_w.write_gps_span();
}
