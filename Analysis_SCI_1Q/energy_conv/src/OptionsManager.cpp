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
            // pedestal
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
            case 'P':
                ped_temp_flag = true;
                if (idx < argc_par - 1) {
                    ped_temp_par_filename = argv_par[++idx];
                    if (ped_temp_par_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            // nonlinearity
            case 'f':
                if (idx < argc_par - 1) {
                    nonlin_fun_filename = argv_par[++idx];
                    if (nonlin_fun_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            // crosstalk
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
            // gain
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
    if (output_filename.IsNull()) {
        output_filename = "output_energy.root";
    }

    // pedestal
    if (ped_vector_filename.IsNull() && ped_temp_par_filename.IsNull()) return false;
    // nonlinearity
    if (nonlin_fun_filename.IsNull()) return false;
    // crosstalk
    if (xtalk_matrix_filename.IsNull()) return false;
    // gain
    if (gain_vec_filename.IsNull() && gain_vs_hv_filename.IsNull()) return false;

    return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " <SCI_1Q.root> [-o <output_energy.root>]" << endl;
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << "  " << " [-p <ped_vec_file.root>]|[-P <ped_temp_par.root>]" << endl;
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << "  " << " -f <nonlin_fun.root> -x -x <xtalk_mat_file.root>" << endl;
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << "  " << " [-g <gain_vec_file.root>]|[-G <gain_vs_hv_file.root>]" << endl;

    cout << endl;
    cout << "Options:" << endl;
    cout << "  -p <ped_vector.root>             pedestal vector file" << endl;
    cout << "  -P <ped_temp_par.root>           pedestal and temperature dependence parameters" << endl;
    cout << "  -f <nonlin_fun.root>             normalized non-linearity function file" << endl;
    cout << "  -x <xtalk_matrix.root>           crosstalk matrix file" << endl;
    cout << "  -g <gain_vec.root>               gain vector file" << endl;
    cout << "  -G <gain_vs_hv.root>             gain vs HV parameters file" << endl;
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
    output_filename.Clear();

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
