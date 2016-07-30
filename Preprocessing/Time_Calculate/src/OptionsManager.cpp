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
            case 'k':
                if (idx < argc_par - 1) {
                    hkfile = argv_par[++idx];
                    if (hkfile[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'o':
                if (idx < argc_par - 1) {
                    outfile = argv_par[++idx];
                    if (outfile[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'g':
                if (idx < argc_par - 1) {
                    logfile = argv_par[++idx];
                    if (logfile[0] == '-') {
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
            scifile = cur_par_str;
        }
    }
    if (scifile.IsNull() || hkfile.IsNull())
        return false;
    else
        return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " " << "<POL_SCI_decoded_data.root> -k <POL_HK_decoded_data.root>" << endl;
    cout << "  ";
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << " " << "[-o <POL_SCI_decoded_data_time.root>] [-g <POL_SCI_time_error.log>]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -k <hk_decoded_data.root>        root file that stores hk decoded data" << endl;
    cout << "  -o <sci_decoded_data.root>       root file that stores sci decoded data after absolute time is added" << endl;
    cout << "  -g <time_error.log>              text file that records time calculating error log info" << endl;
    cout << endl;
    cout << "  --version                        print version and author information" << endl;
    cout << endl;
}

void OptionsManager::print_version() {
    cout << endl;
    cout << "    " << SW_NAME << " - Absolute Time Calculator for POLAR SCI event." << endl;
    cout << "    " << SW_VERSION << " (" << RELEASE_DATE << ", compiled " << __DATE__ << " " << __TIME__ << ")" << endl;
    cout << endl;
    cout << " Copyright (C) 2015-2016 Zhengheng Li <lizhengheng@ihep.ac.cn>" << endl;
    cout << " Source Code: https://github.com/ZhenghengLi/POLAR_RawData_Decode_Project" << endl;
    cout << endl;
}

void OptionsManager::init() {
    scifile.Clear();
    hkfile.Clear();
    outfile.Clear();
    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
