#include "Processor.hpp"

using namespace std;

Processor::Processor() {
    initialize();
}

Processor::~Processor() {
    if (os_logfile_.is_open())
        os_logfile_.close();
}

void Processor::initialize() {
    phy_error_count_ = 0;
    ped_error_count_ = 0;

    phy_first_valid_found_     = false;
    phy_first_valid_index_     = -1;
    phy_first_valid_week_      = -1;
    phy_first_valid_second_    = -1;
    phy_last_valid_index_      = -1;
    phy_last_valid_week_       = -1;
    phy_last_valid_second_     = -1;
    phy_total_valid_count_     = 0;
    ped_first_valid_found_     = false;
    ped_first_valid_index_     = -1;
    ped_first_valid_week_      = -1;
    ped_first_valid_second_    = -1;
    ped_last_valid_index_      = -1;
    ped_last_valid_week_       = -1;
    ped_last_valid_second_     = -1;
    ped_total_valid_count_     = 0;
    
}

bool Processor::logfile_open(const char* filename) {
    os_logfile_.open(filename);
    return os_logfile_.is_open();
}

void Processor::logfile_close() {
    os_logfile_.close();
}

void Processor::set_log(bool flag) {
    log_flag_ = flag;
}

bool Processor::can_log() {
    return log_flag_ && os_logfile_.is_open();
}

bool Processor::check_sci_hk_match(const SciTransfer& scitran, const HkGPSIterator& hkgpsiter) {
    bool phy_first_match = (hkgpsiter.first_gps_sync.first - scitran.phy_first_gps < MAX_OFFSET);
    bool ped_first_match = (hkgpsiter.first_gps_sync.first - scitran.ped_first_gps < MAX_OFFSET);
    bool phy_last_match = (scitran.phy_last_gps - hkgpsiter.last_gps_sync.first < MAX_OFFSET);
    bool ped_last_match = (scitran.ped_last_gps - hkgpsiter.last_gps_sync.first < MAX_OFFSET);

    return phy_first_match && ped_first_match && phy_last_match && ped_last_match;
}

GPSTime Processor::calc_abs_time_(const pair<GPSTime, uint32_t>& gps_sync,
                                  const GPSTime&  trig_frm_gps,
                                  const uint32_t& trig_timestamp, 
                                  const double&   ticks_per_second) {
    GPSTime ret_gps_time = gps_sync.first;
    double gps_diff = trig_frm_gps - gps_sync.first;
    if (gps_diff > MAX_OFFSET) {
        ret_gps_time.week = -1;
        ret_gps_time.second = -1;
        return ret_gps_time;
    }
    double second_diff = (static_cast<double>(trig_timestamp)
                          - static_cast<double>(gps_sync.second)) / ticks_per_second;
    double diff_of_diff = gps_diff - second_diff;
    
    const double cycle = 4294967296 / ticks_per_second;
    if (diff_of_diff > MAX_DIFF) {
        if (abs(diff_of_diff - cycle) < MAX_DIFF) {
            ret_gps_time += second_diff + cycle;
            return ret_gps_time;
        } else {
            ret_gps_time.week = -1;
            ret_gps_time.second = -1;
            return ret_gps_time;
        }
    } else if (diff_of_diff < -1 * MAX_DIFF) {
        if (abs(diff_of_diff + cycle) < MAX_DIFF) {
            ret_gps_time += second_diff - cycle;
            return ret_gps_time;
        } else {
            ret_gps_time.week = -1;
            ret_gps_time.second = -1;
            return ret_gps_time;
        }
    } else {
        ret_gps_time += second_diff;
        return ret_gps_time;
    }
}

void Processor::copy_modules(SciTransfer& scitran) {
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Copying physical modules data ..." << endl;
    cout << "[ " << flush;
    while (scitran.modules_next()) {
        cur_percent = static_cast<int>(100 * scitran.get_modules_cur_index()
                                        / scitran.get_modules_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        scitran.modules_fill();
    }
    cout << " DONE ] " << endl;
}

void Processor::copy_ped_modules(SciTransfer& scitran) {
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Copying pedestal modules data ..." << endl;
    cout << "[ " << flush;
    while (scitran.ped_modules_next()) {
        cur_percent = static_cast<int>(100 * scitran.get_ped_modules_cur_index()
                                       / scitran.get_ped_modules_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        scitran.ped_modules_fill();
    }
    cout << " DONE ] " << endl;
}

void Processor::calc_time_trigger(SciTransfer& scitran, HkGPSIterator& hkgpsiter) {
    hkgpsiter.initialize();
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Calculating time and copying physical trigger data ..." << endl;
    cout << "[ " << flush;
    while (scitran.trigger_next()) {
        cur_percent = static_cast<int>(100 * scitran.get_trigger_cur_index()
                                       / scitran.get_trigger_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }

        if (scitran.t_trigger.is_bad != 0) {
            scitran.t_trigger.abs_gps_week = -1;
            scitran.t_trigger.abs_gps_second = -1;
            scitran.t_trigger.abs_gps_valid = false;
            scitran.trigger_fill();
            continue;
        }

        // start calculating absolute time
        while (!hkgpsiter.is_passed_last() && scitran.phy_cur_gps > hkgpsiter.after_gps_sync.first) {
            hkgpsiter.next_minute();
        }
        GPSTime cur_abs_gps, cur_abs_gps_before, cur_abs_gps_after;
        cur_abs_gps_before = calc_abs_time_(hkgpsiter.before_gps_sync,
                                            scitran.phy_cur_gps,
                                            scitran.phy_cur_timestamp,
                                            hkgpsiter.cur_ticks_per_second);
        if (!hkgpsiter.is_passed_last()) { // not passed last
            cur_abs_gps_after = calc_abs_time_(hkgpsiter.after_gps_sync,
                                               scitran.phy_cur_gps,
                                               scitran.phy_cur_timestamp,
                                               hkgpsiter.cur_ticks_per_second);
            if (cur_abs_gps_before.week < 0) {
                phy_error_count_++;
                if (cur_abs_gps_after.week < 0) {
                    if (can_log()) {
                        os_logfile_ << "phy " << scitran.get_trigger_cur_index() << " : before after" << endl;
                    }
                    scitran.t_trigger.abs_gps_week = -1;
                    scitran.t_trigger.abs_gps_second = -1;
                    scitran.t_trigger.abs_gps_valid = hkgpsiter.is_before_valid() && hkgpsiter.is_after_valid();
                } else {
                    if (can_log()) {
                        os_logfile_ << "phy " << scitran.get_trigger_cur_index() << " : before" << endl;
                    }
                    scitran.t_trigger.abs_gps_week = cur_abs_gps_after.week;
                    scitran.t_trigger.abs_gps_second = cur_abs_gps_after.second;
                    scitran.t_trigger.abs_gps_valid = hkgpsiter.is_after_valid();
                }
            } else if (cur_abs_gps_after.week < 0) {
                phy_error_count_++;
                if (cur_abs_gps_before.week < 0) {
                    if (can_log()) {
                        os_logfile_ << "phy " << scitran.get_trigger_cur_index() << " : before after" << endl;
                    }
                    scitran.t_trigger.abs_gps_week = -1;
                    scitran.t_trigger.abs_gps_second = -1;
                    scitran.t_trigger.abs_gps_valid = hkgpsiter.is_before_valid() && hkgpsiter.is_after_valid();
                } else {
                    if (can_log()) {
                        os_logfile_ << "phy " << scitran.get_trigger_cur_index() << " : after" << endl;
                    }
                    scitran.t_trigger.abs_gps_week = cur_abs_gps_before.week;
                    scitran.t_trigger.abs_gps_second = cur_abs_gps_before.second;
                    scitran.t_trigger.abs_gps_valid = hkgpsiter.is_before_valid();
                }
            } else {
                double second_diff = cur_abs_gps_after - cur_abs_gps_before;
                if (abs(second_diff) > MAX_ABS_DIFF) {
                    phy_error_count_++;
                    if (can_log()) {
                        os_logfile_ << "phy " << scitran.get_trigger_cur_index() << " : too different" << endl;
                    }
                    scitran.t_trigger.abs_gps_week = -1;
                    scitran.t_trigger.abs_gps_second = -1;
                    scitran.t_trigger.abs_gps_valid = hkgpsiter.is_before_valid() && hkgpsiter.is_after_valid();
                } else {
                    cur_abs_gps = cur_abs_gps_before + second_diff / 2;
                    scitran.t_trigger.abs_gps_week = cur_abs_gps.week;
                    scitran.t_trigger.abs_gps_second = cur_abs_gps.second;
                    scitran.t_trigger.abs_gps_valid = hkgpsiter.is_before_valid() && hkgpsiter.is_after_valid();
                }
            }
        } else { // passed last
            if (cur_abs_gps_before.week < 0) {
                phy_error_count_++;
                if (can_log()) {
                    os_logfile_ << "phy " << scitran.get_trigger_cur_index() << " : before at last" << endl;
                }
                scitran.t_trigger.abs_gps_week = -1;
                scitran.t_trigger.abs_gps_second = -1;
                scitran.t_trigger.abs_gps_valid = hkgpsiter.is_before_valid();
            } else {
                scitran.t_trigger.abs_gps_week = cur_abs_gps_before.week;
                scitran.t_trigger.abs_gps_second = cur_abs_gps_before.second;
                scitran.t_trigger.abs_gps_valid = hkgpsiter.is_before_valid();
            }
        }
        // finish calculating absolute time

        if (scitran.t_trigger.abs_gps_week > 0 && scitran.t_trigger.abs_gps_week < 1024) {
            if (scitran.t_trigger.abs_gps_week > 629) {
                scitran.t_trigger.abs_gps_week += 1024;
            } else {
                scitran.t_trigger.abs_gps_week += 2048;
            }
        }
        scitran.trigger_fill();
        
        if (scitran.t_trigger.abs_gps_week >= 0 &&
            scitran.t_trigger.abs_gps_second >= 0 &&
            scitran.t_trigger.abs_gps_valid) {
            phy_total_valid_count_++;
            if (!phy_first_valid_found_) {
                phy_first_valid_found_  = true;
                phy_first_valid_index_  = scitran.get_trigger_cur_index();
                phy_first_valid_week_   = scitran.t_trigger.abs_gps_week;
                phy_first_valid_second_ = scitran.t_trigger.abs_gps_second;
            }
            phy_last_valid_index_  = scitran.get_trigger_cur_index();
            phy_last_valid_week_   = scitran.t_trigger.abs_gps_week;
            phy_last_valid_second_ = scitran.t_trigger.abs_gps_second;
        }
    }
    cout << " DONE ] " << endl;
    char str_buffer[200];
    sprintf(str_buffer, "%d:%d[%ld] => %d:%d[%ld]; %ld/%ld",
            static_cast<int>(phy_first_valid_week_),
            static_cast<int>(phy_first_valid_second_),
            static_cast<long int>(phy_first_valid_index_),
            static_cast<int>(phy_last_valid_week_),
            static_cast<int>(phy_last_valid_second_),
            static_cast<long int>(phy_last_valid_index_),
            static_cast<long int>(phy_total_valid_count_),
            static_cast<long int>(scitran.get_trigger_tot_entries()));
    phy_gps_result_str_.assign(str_buffer);
}

void Processor::calc_time_ped_trigger(SciTransfer& scitran, HkGPSIterator& hkgpsiter) {
    hkgpsiter.initialize();
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Calculating time and copying pedestal trigger data ..." << endl;
    cout << "[ " << flush;
    while (scitran.ped_trigger_next()) {
        cur_percent = static_cast<int>(100 * scitran.get_ped_trigger_cur_index()
                                       / scitran.get_ped_trigger_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }

        if (scitran.t_ped_trigger.is_bad != 0) {
            scitran.t_ped_trigger.abs_gps_week = -1;
            scitran.t_ped_trigger.abs_gps_second = -1;
            scitran.t_ped_trigger.abs_gps_valid = false;
            scitran.ped_trigger_fill();
            continue;
        }

        // start calculating absolute time
        while (!hkgpsiter.is_passed_last() && scitran.ped_cur_gps > hkgpsiter.after_gps_sync.first) {
            hkgpsiter.next_minute();
        }
        GPSTime cur_abs_gps, cur_abs_gps_before, cur_abs_gps_after;
        cur_abs_gps_before = calc_abs_time_(hkgpsiter.before_gps_sync,
                                            scitran.ped_cur_gps,
                                            scitran.ped_cur_timestamp,
                                            hkgpsiter.cur_ticks_per_second);
        if (!hkgpsiter.is_passed_last()) { // not passed last
            cur_abs_gps_after = calc_abs_time_(hkgpsiter.after_gps_sync,
                                               scitran.ped_cur_gps,
                                               scitran.ped_cur_timestamp,
                                               hkgpsiter.cur_ticks_per_second);
            if (cur_abs_gps_before.week < 0) {
                ped_error_count_++;
                if (cur_abs_gps_after.week < 0) {
                    if (can_log()) {
                        os_logfile_ << "ped " << scitran.get_ped_trigger_cur_index() << " : before after" << endl;
                    }
                    scitran.t_ped_trigger.abs_gps_week = -1;
                    scitran.t_ped_trigger.abs_gps_second = -1;
                    scitran.t_ped_trigger.abs_gps_valid = hkgpsiter.is_before_valid() && hkgpsiter.is_after_valid();
                } else {
                    if (can_log()) {
                        os_logfile_ << "ped " << scitran.get_ped_trigger_cur_index() << " : before" << endl;
                    }
                    scitran.t_ped_trigger.abs_gps_week = cur_abs_gps_after.week;
                    scitran.t_ped_trigger.abs_gps_second = cur_abs_gps_after.second;
                    scitran.t_ped_trigger.abs_gps_valid = hkgpsiter.is_after_valid();
                }
            } else if (cur_abs_gps_after.week < 0) {
                ped_error_count_++;
                if (cur_abs_gps_before.week < 0) {
                    if (can_log()) {
                        os_logfile_ << "ped " << scitran.get_ped_trigger_cur_index() << " : before after" << endl;
                    }
                    scitran.t_ped_trigger.abs_gps_week = -1;
                    scitran.t_ped_trigger.abs_gps_second = -1;
                    scitran.t_ped_trigger.abs_gps_valid = hkgpsiter.is_before_valid() && hkgpsiter.is_after_valid();
                } else {
                    if (can_log()) {
                        os_logfile_ << "ped " << scitran.get_ped_trigger_cur_index() << " : after" << endl;
                    }
                    scitran.t_ped_trigger.abs_gps_week = cur_abs_gps_before.week;
                    scitran.t_ped_trigger.abs_gps_second = cur_abs_gps_before.second;
                    scitran.t_ped_trigger.abs_gps_valid = hkgpsiter.is_before_valid();
                }
            } else {
                double second_diff = cur_abs_gps_after - cur_abs_gps_before;
                if (abs(second_diff) > MAX_ABS_DIFF) {
                    ped_error_count_++;
                    if (can_log()) {
                        os_logfile_ << "ped " << scitran.get_ped_trigger_cur_index() << " : too different" << endl;
                    }
                    scitran.t_ped_trigger.abs_gps_week = -1;
                    scitran.t_ped_trigger.abs_gps_second = -1;
                    scitran.t_ped_trigger.abs_gps_valid = hkgpsiter.is_before_valid() && hkgpsiter.is_after_valid();
                } else {
                    cur_abs_gps = cur_abs_gps_before + second_diff / 2;
                    scitran.t_ped_trigger.abs_gps_week = cur_abs_gps.week;
                    scitran.t_ped_trigger.abs_gps_second = cur_abs_gps.second;
                    scitran.t_ped_trigger.abs_gps_valid = hkgpsiter.is_before_valid() && hkgpsiter.is_after_valid();
                }
            }
        } else { // passed last
            if (cur_abs_gps_before.week < 0) {
                ped_error_count_++;
                if (can_log()) {
                    os_logfile_ << "ped " << scitran.get_ped_trigger_cur_index() << " : before at last" << endl;
                }
                scitran.t_ped_trigger.abs_gps_week = -1;
                scitran.t_ped_trigger.abs_gps_second = -1;
                scitran.t_ped_trigger.abs_gps_valid = hkgpsiter.is_before_valid();
            } else {
                scitran.t_ped_trigger.abs_gps_week = cur_abs_gps_before.week;
                scitran.t_ped_trigger.abs_gps_second = cur_abs_gps_before.second;
                scitran.t_ped_trigger.abs_gps_valid = hkgpsiter.is_before_valid();
            }
        }
        // finish calculating absolute time

        if (scitran.t_ped_trigger.abs_gps_week > 0 && scitran.t_ped_trigger.abs_gps_week < 1024) {
            if (scitran.t_ped_trigger.abs_gps_week > 629) {
                scitran.t_ped_trigger.abs_gps_week += 1024;
            } else {
                scitran.t_ped_trigger.abs_gps_week += 2048;
            }
        }
        scitran.ped_trigger_fill();
        
        if (scitran.t_ped_trigger.abs_gps_week >= 0 &&
            scitran.t_ped_trigger.abs_gps_second >= 0 &&
            scitran.t_ped_trigger.abs_gps_valid) {
            ped_total_valid_count_++;
            if (!ped_first_valid_found_) {
                ped_first_valid_found_  = true;
                ped_first_valid_index_  = scitran.get_ped_trigger_cur_index();
                ped_first_valid_week_   = scitran.t_ped_trigger.abs_gps_week;
                ped_first_valid_second_ = scitran.t_ped_trigger.abs_gps_second;
            }
            ped_last_valid_index_  = scitran.get_ped_trigger_cur_index();
            ped_last_valid_week_   = scitran.t_ped_trigger.abs_gps_week;
            ped_last_valid_second_ = scitran.t_ped_trigger.abs_gps_second;
        }
    }
    cout << " DONE ] " << endl;
    char str_buffer[200];
    sprintf(str_buffer, "%d:%d[%ld] => %d:%d[%ld]; %ld/%ld",
            static_cast<int>(ped_first_valid_week_),
            static_cast<int>(ped_first_valid_second_),
            static_cast<long int>(ped_first_valid_index_),
            static_cast<int>(ped_last_valid_week_),
            static_cast<int>(ped_last_valid_second_),
            static_cast<long int>(ped_last_valid_index_),
            static_cast<long int>(ped_total_valid_count_),
            static_cast<long int>(scitran.get_ped_trigger_tot_entries()));
    ped_gps_result_str_.assign(str_buffer);
}

void Processor::write_meta_info(SciTransfer& scitran) {
    string meta_key;
    string meta_value;

    // dattype
    meta_key = scitran.get_dattype()->GetName();
    meta_value = scitran.get_dattype()->GetTitle();
    meta_value += ", ABS TIME ADDED";
    scitran.write_meta(meta_key.c_str(), meta_value.c_str());

    // m_version
    meta_key = scitran.get_version()->GetName();
    meta_value = scitran.get_version()->GetTitle();
    meta_value += "; ";
    meta_value += SW_NAME + " " + SW_VERSION;
    scitran.write_meta(meta_key.c_str(), meta_value.c_str());

    // gentime
    meta_key = scitran.get_gentime()->GetName();
    meta_value = scitran.get_gentime()->GetTitle();
    TTimeStamp* cur_time = new TTimeStamp();
    meta_value += "; ";
    meta_value += cur_time->AsString("lc");
    delete cur_time;
    cur_time = NULL;
    scitran.write_meta(meta_key.c_str(), meta_value.c_str());

    // rawfile
    meta_key = scitran.get_rawfile()->GetName();
    meta_value = scitran.get_rawfile()->GetTitle();
    scitran.write_meta(meta_key.c_str(), meta_value.c_str());

    // dcdinfo
    meta_key = scitran.get_dcdinfo()->GetName();
    meta_value = scitran.get_dcdinfo()->GetTitle();
    scitran.write_meta(meta_key.c_str(), meta_value.c_str());

    // gps_info
    scitran.write_meta("m_phy_gps", phy_gps_result_str_.c_str());
    scitran.write_meta("m_ped_gps", ped_gps_result_str_.c_str());
}

void Processor::print_error_count(const SciTransfer& scitran) {
    char phy_buffer[100];
    sprintf(phy_buffer, "%ld / %lld", phy_error_count_, scitran.get_trigger_tot_entries());
    char ped_buffer[100];
    sprintf(ped_buffer, "%ld / %lld", ped_error_count_, scitran.get_ped_trigger_tot_entries());
    cout << "================================================================================" << endl;
    cout << left
         << setw(17) << "phy_error_count:" << setw(20) << phy_buffer
         << setw(17) << "ped_error_count:" << setw(20) << ped_buffer
         << right << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    cout << "phy_gps: { " << phy_gps_result_str_ << " }" << endl;
    cout << "ped_gps: { " << ped_gps_result_str_ << " }" << endl;
    cout << "================================================================================" << endl;
}
