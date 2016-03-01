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
            case 'l':
                if (idx < argc_par - 1) {
                    list_file = argv_par[++idx];
                    if (list_file[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'o':
                if (idx < argc_par - 1) {
                    out_file = argv_par[++idx];
                    if (out_file[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'g':
                if (idx < argc_par - 1) {
                    log_file = argv_par[++idx];
                    if (log_file[0] == '-') {
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
            raw_file_queue.push(cur_par_str);
        }
    }
    if (list_file.IsNull() && raw_file_queue.empty())
        return false;
    else
        return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " " << "[-l <listfile.txt>] [<POL_SCI_data_001.dat> <POL_SCI_data_002.dat> ...]" << endl;
    cout << "  ";
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << " " << "[-o <POL_SCI_decoded_data.root>] [-g <POL_SCI_decoding_error.log>]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -l <listfile.txt>                text file that contains raw data file list" << endl;
    cout << "  -o <decoded_data.root>           root file that stores decoded data" << endl;
    cout << "  -g <decoding_error.log>          text file that records decoding error log info" << endl;
    cout << endl;
    cout << "  --version                        print version and author information" << endl;
    cout << endl;
}

void OptionsManager::print_version() {
    cout << SW_NAME << " ";
    cout << SW_VERSION << " ";
    cout << "- POLAR SCI raw data decoder" << endl;
    cout << "Copyright (C) 2015-2016 Zhengheng Li <lizhengheng@ihep.ac.cn>" << endl;
    cout << "Source Code: https://github.com/ZhenghengLi/POLAR_RawData_Decode_Project" << endl;
}

void OptionsManager::init() {
    list_file.Clear();
    out_file.Clear();
    log_file.Clear();
    while (!raw_file_queue.empty()) {
        raw_file_queue.pop();
    }
    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
