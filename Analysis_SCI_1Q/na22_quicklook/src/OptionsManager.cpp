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
                    ce_result_filename = argv_par[++idx];
                    if (ce_result_filename[0] == '-') {
                        return false;
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
            if (na22_data_filename.IsNull()) {
                na22_data_filename = cur_par_str;
            } else {
                return false;
            }
        }
    }
    if (na22_data_filename.IsNull()) return false;
    if (ce_result_filename.IsNull() && !show_flag) return false;
    return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " <na22_data.root> [-m] [-o <ce_res.root>]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -o <ce_res.root>                 root file to store ADC/keV" << endl;
    cout << "  -m                               show compton edge fitting result" << endl;
    cout << endl;
    cout << "  --version                        print version and author information" << endl;
    cout << endl;
}

void OptionsManager::print_version() {
    cout << endl;
    cout << "    " << SW_NAME << " - POLAR 1Q Level Data Analysis Chain" << endl;
    cout << "    " << SW_VERSION << " (" << RELEASE_DATE << ", compiled " << __DATE__ << " " << __TIME__ << ")" << endl;
    cout << endl;
    cout << " Copyright (C) 2015-2016 Zhengheng Li <lizhengheng@ihep.ac.cn>" << endl;
    cout << endl;
}

void OptionsManager::print_options() {

}

void OptionsManager::init() {
    na22_data_filename.Clear();
    ce_result_filename.Clear();
    show_flag = false;

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
