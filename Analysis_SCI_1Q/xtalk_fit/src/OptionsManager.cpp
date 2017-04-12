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
                    xtalk_result_filename = argv_par[++idx];
                    if (xtalk_result_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'n':
                if (idx < argc_par - 1) {
                    TString tmp_arg = argv_par[++idx];
                    if (tmp_arg[0] == '-') {
                        return false;
                    } else {
                        ct_num = tmp_arg.Atoi();
                    }
                } else {
                    return false;
                }
                break;
            case 'a':
                if (idx < argc_par - 1) {
                    TString tmp_arg = argv_par[++idx];
                    if (tmp_arg[0] == '-') {
                        return false;
                    } else {
                        low_temp = tmp_arg.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'b':
                if (idx < argc_par - 1) {
                    TString tmp_arg = argv_par[++idx];
                    if (tmp_arg[0] == '-') {
                        return false;
                    } else {
                        high_temp = tmp_arg.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'x':
                if (idx < argc_par - 1) {
                    TString tmp_arg = argv_par[++idx];
                    if (tmp_arg[0] == '-') {
                        return false;
                    } else {
                        low_hv = tmp_arg.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'y':
                if (idx < argc_par - 1) {
                    TString tmp_arg = argv_par[++idx];
                    if (tmp_arg[0] == '-') {
                        return false;
                    } else {
                        high_hv = tmp_arg.Atof();
                    }
                } else {
                    return false;
                }
                break;
            default:
                return false;
            }
        } else {
            if (xtalk_data_filename.IsNull()) {
                xtalk_data_filename = cur_par_str;
            } else {
                return false;
            }
        }
    }
    if (xtalk_data_filename.IsNull()) return false;
    if (ct_num < 1 || ct_num > 25) return false;
    if (low_temp > high_temp) return false;
    if (xtalk_result_filename.IsNull()) {
        xtalk_result_filename = "xtalk_result.root";
    }
    return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << "<xtalk_data.root> -n <ct_num> [-o <xtalk_res.root>]" << endl;
    cout << "  ";
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << " [-a <low_temp>] [-b <high_temp>] [-x <low_hv>] [-y <high_hv>]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -n <ct_num>                      select which module to process" << endl;
    cout << "  -a <low_temp>                    low temperature" << endl;
    cout << "  -b <high_temp>                   high temperature" << endl;
    cout << "  -x <low_hv>                      low HV" << endl;
    cout << "  -y <high_hv>                     high HV" << endl;
    cout << "  -o <xtalk_res.root>              root file to store xtalk matrix" << endl;
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
    xtalk_data_filename.Clear();
    ct_num = -1;
    low_temp = -128;
    high_temp = 128;
    low_hv = -128;
    high_hv = 1280;
    xtalk_result_filename.Clear();

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
