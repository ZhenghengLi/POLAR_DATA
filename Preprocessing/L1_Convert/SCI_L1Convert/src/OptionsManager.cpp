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
            default:
                return false;
            }
        } else {
            if (in_file.IsNull()) {
                in_file = cur_par_str;
            } else {
                return false;
            }
        }
    }
    if (in_file.IsNull()) {
        return false;
    } else {
        if (out_file.IsNull())
            out_file = "POL_SCI_decoded_data_L1.root";
        return true;
    }
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " " << "<POL_SCI_decoded_data_time.root> [-o <POL_SCI_decoded_data_L1.root>]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -o <splitted_data.root>          root file that stores the Level 1 decoded data" << endl;
    cout << endl;
    cout << "  --version                        print version and author information" << endl;
    cout << endl;
}

void OptionsManager::print_version() {
    cout << endl;
    cout << "    " << SW_NAME << " - POLAR SCI decoded data Level 1 converting" << endl;
    cout << "    " << SW_VERSION << " (" << RELEASE_DATE << ", compiled " << __DATE__ << " " << __TIME__ << ")" << endl;
    cout << endl;
    cout << " Copyright (C) 2015-2016 Zhengheng Li <lizhengheng@ihep.ac.cn>" << endl;
    cout << " Source Code: https://github.com/ZhenghengLi/POLAR_RawData_Decode_Project" << endl;
    cout << endl;
}

void OptionsManager::init() {
    in_file.Clear();
    out_file.Clear();
    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
