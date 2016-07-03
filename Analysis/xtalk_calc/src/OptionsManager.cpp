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
            case 'p':
                if (idx < argc_par - 1) {
                    ped_vector_filename = argv_par[++idx];
                    if (ped_vector_filename[0] == '-') {
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
                    xtalk_data_filename = argv_par[++idx];
                    if (xtalk_data_filename[0] == '-') {
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
                    xtalk_data_filename = argv_par[++idx];
                    if (xtalk_data_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'n':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        ct_num = tmp_string.Atoi();
                    }
                } else {
                    return false;
                }
                break;
            case 'm':
                show_flag = true;
                break;
            case 'o':
                if (xtalk_matrix_read_flag)
                    return false;
                if (idx < argc_par - 1) {
                    xtalk_matrix_filename = argv_par[++idx];
                    if (xtalk_matrix_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'x':
                xtalk_matrix_read_flag = true;
                if (idx < argc_par - 1) {
                    xtalk_matrix_filename = argv_par[++idx];
                    if (xtalk_matrix_filename[0] == '-') {
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
    if (!ped_vector_filename.IsNull() && !decoded_data_filename.IsNull() && rw_mode == 'w') {
        action = 1;
        return true;
    } else if (rw_mode == 'r' && ct_num > 0 && ct_num <= 25 && show_flag) {
        action = 2;
        return true;
    } else if (rw_mode == 'r' && !xtalk_matrix_filename.IsNull() && !xtalk_matrix_read_flag) {
        action = 3;
        return true;
    } else if (xtalk_matrix_read_flag && !xtalk_matrix_filename.IsNull()) {
        action = 4;
        return true;
    } else {
        return false;
    }
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " <decoded_data_file.root> -p <ped_vector_file.root> -F <xtalk_data_file.root>" << endl;
    cout << "  ";
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << " [-B <week1:second1>] [-E <week2:second2]" << endl;
    cout << "  " << SW_NAME << " -f <xtalk_data_file.root> -n <ct_num> -m" << endl;
    cout << "  " << SW_NAME << " -f <xtalk_data_file.root> -o <xtalk_matrix_file.root>" << endl;
    cout << "  " << SW_NAME << " -x <xtalk_matrix_file.root>" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -B <week1:second1>               GPS string of beginning" << endl;
    cout << "  -E <week2:second2>               GPS string of ending" << endl;
    cout << "  -p <ped_vec.root>                root file to read that stores pedestal vectors of all modules" << endl;    
    cout << "  -F <xtalk_data.root>             root file to write that stores crosstalk data of all modules" << endl;
    cout << "  -f <xtalk_data.root>             root file to read that stores crosstalk data of all modules" << endl;
    cout << "  -n <ct_num>                      select one module by CT number" << endl;
    cout << "  -m                               show crosstalk matrix map of one module" << endl;
    cout << "  -o <xtalk_matrix.root>           root file to write that stores matrixes of all modules" << endl;
    cout << "  -x <xtalk_matrix.root>           show the crosstalk matrix map of all modules" << endl;
    cout << endl;
    cout << "  --version                        print version and author information" << endl;
    cout << endl;
}

void OptionsManager::print_version() {
    cout << endl;
    cout << "    " << SW_NAME << " - POLAR Data Analysis Tools - xtalk_calc" << endl;
    cout << "    " << SW_VERSION << " (" << RELEASE_DATE << ", compiled " << __DATE__ << " " << __TIME__ << ")" << endl;
    cout << endl;
    cout << " Copyright (C) 2015-2016 Zhengheng Li <lizhengheng@ihep.ac.cn>" << endl;
    cout << " Source Code: https://github.com/ZhenghengLi/POLAR_RawData_Decode_Project" << endl;
    cout << endl;
}

void OptionsManager::print_options() {

}

void OptionsManager::init() {
    ped_vector_filename.Clear();    
    decoded_data_filename.Clear();
    xtalk_data_filename.Clear();
    xtalk_matrix_filename.Clear();
    xtalk_matrix_read_flag = false;
    show_flag = false;
    rw_mode = '0';
    action = 0;
    ct_num = 0;

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
