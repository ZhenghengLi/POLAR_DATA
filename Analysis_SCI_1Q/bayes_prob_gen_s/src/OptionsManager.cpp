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
            case 'M':
                if (idx < argc_par - 1) {
                    modcur_array_filename = argv_par[++idx];
                    if (modcur_array_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'A':
                if (idx < argc_par - 1) {
                    TString tmp_str = argv_par[++idx];
                    if (tmp_str[0] == '-') {
                        return false;
                    } else {
                        pa_idx = tmp_str.Atoi();
                    }
                } else {
                    return false;
                }
                break;
            case 'D':
                if (idx < argc_par - 1) {
                    TString tmp_str = argv_par[++idx];
                    if (tmp_str[0] == '-') {
                        return false;
                    } else {
                        pd_idx = tmp_str.Atoi();
                    }
                } else {
                    return false;
                }
                break;
            case 'o':
                if (idx < argc_par - 1) {
                    output_filename = argv_par[++idx];
                    if (output_filename[0] == '-') {
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
            if (modcur_filename.IsNull()) {
                modcur_filename = cur_par_str;
            } else {
                return false;
            }
        }
    }

    if (modcur_filename.IsNull()) return false;
    if (modcur_array_filename.IsNull()) return false;
    if (output_filename.IsNull()) output_filename = "contour_output.root";
    if (pa_idx < 0 || pa_idx > 89) {
        cout << "pa_idx is out of range [0-89]: " << pa_idx << endl;
        return false;
    }
    if (pd_idx < 0 || pd_idx > 49) {
        cout << "pd_idx is out of range [0-49]: " << pd_idx << endl;
        return false;
    }

    return true;

}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " <modcur.root> -M <modcur_array.root> -A <pa_idx> -D <pd_idx> -o <contour.root>" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -M <modcur_array.root>           modcur_array file" << endl;
    cout << "  -A <pa_idx>                      pa_idx: 0, 1, 2, ..., 89" << endl;
    cout << "  -D <pd_idx>                      pd_idx: 0, 1, 2, ..., 49" << endl;
    cout << "  -o <contour.root>                contour file" << endl;
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
    modcur_filename.Clear();
    modcur_array_filename.Clear();
    output_filename.Clear();
    version_flag_ = false;
    pa_idx = 0;
    pd_idx = 0;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
