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
            case 't':
                if (idx < argc_par - 1) {
                    na22_flag_filename = argv_par[++idx];
                    if (na22_flag_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'F':
                if (idx < argc_par - 1) {
                    na22_data_filename = argv_par[++idx];
                    if (na22_data_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'B':
                if (idx < argc_par - 1) {
                    TString tmp_arg = argv_par[++idx];
                    if (tmp_arg[0] == '-') {
                        return false;
                    } else {
                        begin_met_time = tmp_arg.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'E':
                if (idx < argc_par - 1) {
                    TString tmp_arg = argv_par[++idx];
                    if (tmp_arg[0] == '-') {
                        return false;
                    } else {
                        end_met_time = tmp_arg.Atof();
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
    if (na22_flag_filename.IsNull()) return false;
    if (na22_data_filename.IsNull()) {
        na22_data_filename = "na22_data.root";
    }
    return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << "<SCI_1Q.root> -t <na22_flag.root> [-B <begin_met>] [-E <end_met>] [-F <na22_data.root>]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << " -t <na22_flag.root>               Na22 event flag file" << endl;
    cout << "  -B <begin_met>                   begin MET time" << endl;
    cout << "  -E <end_met>                     end MET time" << endl;
    cout << "  -F <na22_data.root>              root file to collect Na22 event data" << endl;
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
    na22_flag_filename.Clear();
    na22_data_filename.Clear();
    begin_met_time = -1;
    end_met_time = -1;

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
