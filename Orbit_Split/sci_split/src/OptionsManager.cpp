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
            case 'B':
                if (idx < argc_par - 1) {
                    gps_begin = argv_par[++idx];
                    if (gps_begin[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'E':
                if (idx < argc_par - 1) {
                    gps_end = argv_par[++idx];
                    if (gps_end[0] == '-') {
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
            default:
                return false;
            }
        } else {
            raw_file_vector.push_back(cur_par_str);
        }
    }
    if (raw_file_vector.empty())
        return false;
    else
        return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " " << "POL_SCI_decoded_data_time_1.root POL_SCI_decoded_data_time_2.root ..." << endl;
    cout << "  ";
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << " " << "[-B <gps_begin_str>] [-E <gps_end_str>] [-o <POL_SCI_decoded_data_time_split.root]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -B <gps_begin_str>               string of GPS beginning time, the format is week:second or begin" << endl;
    cout << "  -E <gps_end_str>                 string of GPS ending time, the format is week:second or end" << endl;
    cout << "  -o <splitted_data.root>          root file that stores the splitted data" << endl;
    cout << endl;
    cout << "  --version                        print version and author information" << endl;
    cout << endl;
}

void OptionsManager::print_version() {
    cout << endl;
    cout << "    " << SW_NAME << " - POLAR SCI decoded data splitting" << endl;
    cout << "    " << SW_VERSION << " (" << RELEASE_DATE << ", compiled " << __DATE__ << " " << __TIME__ << ")" << endl;
    cout << endl;
    cout << " Copyright (C) 2015-2016 Zhengheng Li <lizhengheng@ihep.ac.cn>" << endl;
    cout << " Source Code: https://github.com/ZhenghengLi/POLAR_RawData_Decode_Project" << endl;
    cout << endl;
}

void OptionsManager::init() {
    gps_begin.Clear();
    gps_end.Clear();
    raw_file_vector.clear();
    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
