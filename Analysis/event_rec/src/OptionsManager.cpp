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
            case 'x':
                if (idx < argc_par - 1) {
                    xtalk_matrix_filename = argv_par[++idx];
                    if (xtalk_matrix_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'c':
                if (idx < argc_par - 1) {
                    adc_per_kev_filename = argv_par[++idx];
                    if (adc_per_kev_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'F':
                if (idx < argc_par - 1) {
                    rec_event_data_filename = argv_par[++idx];
                    if (rec_event_data_filename[0] == '-') {
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
    if (ped_vector_filename.IsNull() || xtalk_matrix_filename.IsNull() || adc_per_kev_filename.IsNull() ||
        decoded_data_filename.IsNull() || rec_event_data_filename.IsNull())
        return false;
    else
        return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " -p <ped_vector_file.root> -x <xtalk_matrix_file.root> -c <adc_per_kev_file.root> " << endl;
    cout << "  ";
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << " <decoded_data_file.root> [-B <week1:second1>] [-E <week2:second2>]" << endl;
    cout << "  ";
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << " -F <rec_event_data_file.root>" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -B <week1:second1>               GPS string of beginning" << endl;
    cout << "  -E <week2:second2>               GPS string of ending" << endl;
    cout << "  -p <ped_vec.root>                root file to read that stores pedestal vectors of all modules" << endl;
    cout << "  -x <xtalk_matrix.root>           root file to read that stores crosstalk matrices of all modules" << endl;
    cout << "  -c <adc_per_kev.root>            root file to read that stores ADC/KeV vector of all modules" << endl;
    cout << "  -F <rec_event.root>              root file to write that stores reconstructed event data" << endl;
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
    ped_vector_filename.Clear();
    xtalk_matrix_filename.Clear();
    adc_per_kev_filename.Clear();
    decoded_data_filename.Clear();
    rec_event_data_filename.Clear();

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
