#include "OptionsManager.hpp"

using namespace std;

OptionsManager::OptionsManager() {
    init();
}

OptionsManager::~OptionsManager() {

}

bool OptionsManager::parse(int argc_par, char** argv_par) {
    if (argc_par < 2)
        return false;
    TString cur_par_str;
    int idx = 0;
    while (idx < argc_par - 1) {
        cur_par_str = argv_par[++idx];
        if (cur_par_str == "--version") {
            version_flag_ = true;
            return false;
        }
        if (cur_par_str[0] == '-') {
            if (cur_par_str.Length() != 2)
                return false;
            char cur_option = cur_par_str[1];
            switch (cur_option) {
            case 'F':
                if (rw_mode == 'r')
                    return false;
                rw_mode = 'w';
                if (idx < argc_par - 1) {
                    angle_data_filename = argv_par[++idx];
                    if (angle_data_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'f':
                if (rw_mode == 'w')
                    return false;
                rw_mode = 'r';
                if (idx < argc_par - 1) {
                    angle_data_filename = argv_par[++idx];
                    if (angle_data_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'h':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        low_energy_thr = tmp_string.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'm':
                show_flag = true;
                break;
            default:
                return false;
            }
        } else {
            if (rec_event_data_filename.IsNull()) {
                rec_event_data_filename = cur_par_str;
            } else {
                return false;
            }
        }
    }
    if (low_energy_thr < 0) {
        low_energy_thr = 0;
    }
    // judge the action
    if (!rec_event_data_filename.IsNull() && rw_mode == 'w') {
        action = 1;
        return true;
    } else if (rw_mode == 'r' && show_flag) {
        action = 2;
        return true;
    } else {
        return false;
    }
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " <rec_event_data_file.root> -F <angle_data_file.root> [-h <low_erg_thr>]" << endl;
    cout << "  " << SW_NAME << " -f <angle_data_file.root> -m" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -F <angle_data.root>             root file to write that stores scattering angle data" << endl;
    cout << "  -f <angle_data.root>             root file to read that stores scattering angle data" << endl;
    cout << "  -h <log_erg_thr>                 set the low energy threshold " << endl;
    cout << "  -m                               show hit map and modulation curve" << endl;
    cout << endl;
    cout << "  --version                        print version and author information" << endl;
    cout << endl;
}

void OptionsManager::print_version() {
    cout << endl;
    cout << "    " << SW_NAME << " - POLAR Data Analysis Tools" << endl;
    cout << "    " << SW_VERSION << " (" << RELEASE_DATE << ", compiled " << __DATE__ << " " << __TIME__ << ")" << endl;
    cout << endl;
    cout << " Copyright (C) 2015-2016 Zhengheng Li <lizhengheng@ihep.ac.cn>" << endl;
    cout << " Source Code: https://github.com/ZhenghengLi/POLAR_RawData_Decode_Project" << endl;
    cout << endl;
}

void OptionsManager::print_options() {

}

void OptionsManager::init() {
    rec_event_data_filename.Clear();
    angle_data_filename.Clear();
    show_flag = false;
    rw_mode = '0';
    action = 0;
    low_energy_thr = -1;

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
