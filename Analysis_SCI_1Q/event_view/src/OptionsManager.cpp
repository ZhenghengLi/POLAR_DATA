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
            case 'a':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        start = tmp_string.Atoi();
                    }
                } else {
                    return false;
                }
                break;
            case 'n':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        step = tmp_string.Atoi();
                    }
                } else {
                    return false;
                }
                break;
            case 'l':
                if (idx < argc_par - 1) {
                    entry_list_filename = argv_par[++idx];
                    if (entry_list_filename[0] == '-') {
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
    if (pol_event_filename.IsNull())
        return false;
    if (start < 0) start = 0;
    if (step < 1)  step  = 1;
    return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
	cout << "  " << SW_NAME << " <SCI_1Q.root> [[-a <start>] [-n <step>]] | [-l <list_file.txt>]" << endl;
    cout << "  ";
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -a start                         start entry" << endl;
    cout << "  -n step                          iteration step" << endl;
    cout << "  -l list_file.txt                 entry list file" << endl;
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
    start = 0;
    step = 1;
    entry_list_filename.Clear();

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
