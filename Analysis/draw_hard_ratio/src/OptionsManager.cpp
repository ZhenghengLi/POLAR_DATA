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
                    begin_gps = argv_par[++idx];
                    if (begin_gps[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'E':
                if (idx < argc_par - 1) {
                    end_gps = argv_par[++idx];
                    if (end_gps[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'w':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        time_bin_width = tmp_string.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'z':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        max_bars = tmp_string.Atoi();
                    }
                } else {
                    return false;
                }
                break;
            case 'y':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        min_bars = tmp_string.Atoi();
                    }
                } else {
                    return false;
                }
                break;
            default:
                return false;
            }
        } else {
            if (event_data_filename.IsNull()) {
                event_data_filename = cur_par_str;
            } else {
                return false;
            }
        }
    }
    if (event_data_filename.IsNull())
        return false;
    if (begin_gps.IsNull())
        begin_gps = "begin";
    if (end_gps.IsNull())
        end_gps = "end";
    if (max_bars < min_bars)
        return false;
    return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
	cout << "  " << SW_NAME << " <event_data_file.root> [-B <week1:second1>] [-E <week2:second2>]" << endl;
    cout << "  ";
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << " [-w <time_bin_width>] [-y <min_bars>] [-z <max_bars>]" << endl;
    cout << "  ";
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -B week1:second1                 GPS string of beginning" << endl;
    cout << "  -E week2:second2                 GPS string of ending" << endl;
    cout << "  -w <time_bin_width>              time bin width" << endl;
    cout << "  -y <min_bars>                    minimum number of bars to cut, default is 0" << endl;
    cout << "  -z <max_bars>                    maximum number of bars to cut, defualt is 1600" << endl;
    cout << endl;
    cout << "  --version                        print version and author information" << endl;
    cout << endl;
}

void OptionsManager::print_version() {
    cout << endl;
    cout << "    " << SW_NAME << " - POLAR Data Analysis Tools" << endl;
    cout << "    " << SW_VERSION << " (" << RELEASE_DATE << ", compiled " << __DATE__ << " " << __TIME__ << ")" << endl;
    cout << endl;
    cout << " Copyright (C) 2015-2016 Zhengheng Li <lizhengheng@ihep.ac.cn>" << endl;
    cout << " Source Code: https://github.com/ZhenghengLi/POLAR_RawData_Decode_Project" << endl;
    cout << endl;
}

void OptionsManager::print_options() {

}

void OptionsManager::init() {
    event_data_filename.Clear();
    begin_gps.Clear();
    end_gps.Clear();
	time_bin_width = 5;
	min_bars = 0;
	max_bars = 1600;

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
