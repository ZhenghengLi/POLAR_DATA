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
                    output_filename = argv_par[++idx];
                    if (output_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'g':
                if (idx < argc_par - 1) {
                    gain_vec_filename = argv_par[++idx];
                    gain_hv_flag = false;
                    if (gain_vec_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'G':
                if (idx < argc_par - 1) {
                    gain_vs_hv_filename = argv_par[++idx];
                    gain_hv_flag = true;
                    if (gain_vs_hv_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 't':
                if (idx < argc_par - 1) {
                    gain_temp_filename = argv_par[++idx];
                    gain_temp_flag = true;
                    if (gain_temp_filename[0] == '-') {
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
            if (pol_event_filename.IsNull()) {
                pol_event_filename = cur_par_str;
            } else {
                return false;
            }
        }
    }
    if (pol_event_filename.IsNull()) return false;
    if (gain_vec_filename.IsNull() && gain_vs_hv_filename.IsNull()) return false;
    if (output_filename.IsNull()) {
        output_filename = "output_energy.root";
    }
    return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " <SCI_1Q.root> [-g <gain_vec.root>]|[-G <gain_vs_hv.root>] [-t <gain_temp.root>] [-o <output_energy.root>]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -g <gain_vec.root>               gain vector file" << endl;
    cout << "  -G <gain_vs_hv.root>             gain vs HV parameters file" << endl;
    cout << "  -t <gain_temp.root>              gain temp slope file" << endl;
    cout << "  -o <output_energy.root>          energy reconstructed data" << endl;
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
    pol_event_filename.Clear();
    gain_vec_filename.Clear();
    gain_vs_hv_filename.Clear();
    gain_hv_flag = false;
    gain_temp_filename.Clear();
    gain_temp_flag = false;
    output_filename.Clear();

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
