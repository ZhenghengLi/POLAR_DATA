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
            case 'B':
                if (idx < argc_par - 1) {
                    begin_gps = argv_par[++idx];
                    if (begin_gps[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'E':
                if (idx < argc_par - 1) {
                    end_gps = argv_par[++idx];
                    if (end_gps[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'F':
                if (rw_mode == 'r')
                    return false;
                rw_mode = 'w';
                if (idx < argc_par - 1) {
                    ped_data_filename = argv_par[++idx];
                    if (ped_data_filename[0] == '-') {
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
                    ped_data_filename = argv_par[++idx];
                    if (ped_data_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'm':
                show_flag = true;
                break;
            case 'o':
                if (idx < argc_par - 1) {
                    ped_vector_filename = argv_par[++idx];
                    if (ped_vector_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            default:
                return false;
            }
        } else {
            if (decoded_data_filename.IsNull()) {
                decoded_data_filename = cur_par_str;
            } else {
                return false;
            }
        }
    }
    if (begin_gps.IsNull())
        begin_gps = "begin";
    if (end_gps.IsNull())
        end_gps = "end";
    // judge the action
    if (!decoded_data_filename.IsNull() && rw_mode == 'w') {
        action = 1;
        return true;
    } else if (rw_mode == 'r' && show_flag) {
        action = 2;
        return true;
    } else if (rw_mode == 'r' && !ped_vector_filename.IsNull()) {
        action = 3;
        return true;
    } else {
        return false;
    }
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " <decoded_data_file.root> -B week1:second1 -E week2:second2 -F <ped_data_file.root>" << endl;
    cout << "  " << SW_NAME << " -f <ped_data_file.root> -m" << endl;
    cout << "  " << SW_NAME << " -f <ped_data_file.root> -o <ped_vector_file.root>" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -B week1:second1                 GPS string of beginning" << endl;
    cout << "  -E week2:second2                 GPS string of ending" << endl;
    cout << "  -F <decoded_data.root>           root file to write that stores pedestal data of all modules" << endl;
    cout << "  -f <ped_data.root>               root file to read that stores pedestal data of all modules" << endl;
    cout << "  -m                               show pedestal mean map of 1600 channels" << endl;
    cout << "  -o <ped_vector.root>             root file to write that stores vectors of pedestal mean and sigma" << endl;
    cout << endl;
    cout << "  --version                        print version and author information" << endl;
    cout << endl;
}

void OptionsManager::print_version() {
    cout << endl;
    cout << "    " << SW_NAME << " - POLAR Data Analysis Tools - ped_calc" << endl;
    cout << "    " << SW_VERSION << " (" << RELEASE_DATE << ", compiled " << __DATE__ << " " << __TIME__ << ")" << endl;
    cout << endl;
    cout << " Copyright (C) 2015-2016 Zhengheng Li <lizhengheng@ihep.ac.cn>" << endl;
    cout << " Source Code: https://github.com/ZhenghengLi/POLAR_RawData_Decode_Project" << endl;
    cout << endl;
}

void OptionsManager::print_options() {
    cout << "begin_gps: " << begin_gps << endl;
    cout << "end_gps: " << end_gps << endl;
    cout << "decoded_data_filename: " << decoded_data_filename << endl;
    cout << "ped_data_filename: " << ped_data_filename << endl;
    cout << "ped_vector_filename: " << ped_vector_filename << endl;
    cout << "show_flag: " << show_flag << endl;
    cout << "rw_mode: " << rw_mode << endl;
    cout << "action: " << action << endl;
}

void OptionsManager::init() {
    decoded_data_filename.Clear();
    ped_data_filename.Clear();
    ped_vector_filename.Clear();
    show_flag = false;
    rw_mode = '0';
    action = 0;

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
