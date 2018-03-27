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
                    modcur_array_filename = argv_par[++idx];
                    if (modcur_array_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'N':
                if (idx < argc_par - 1) {
                    nopol_modcur_filename = argv_par[++idx];
                    if (nopol_modcur_filename[0] == '-') {
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
            pol_modcur_filelist.push_back(cur_par_str);
        }
    }

    if (pol_modcur_filelist.empty()) return false;
    if (nopol_modcur_filename.IsNull()) return false;
    if (modcur_array_filename.IsNull()) modcur_array_filename = "modcur_array.root";
    return true;

}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " <pol_modcur_1.root> <pol_modcur_2.root> <...> -N <nopol_modcur.root> -o <modcur_array.root>" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -N <nopol_modcur.root>           nopol_modcur file" << endl;
    cout << "  -o <modcur_array.root>           modcur_array file" << endl;
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

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
