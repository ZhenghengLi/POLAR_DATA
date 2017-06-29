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
            case 'f':
                if (idx < argc_par - 1) {
                    event_type_filename = argv_par[++idx];
                    if (event_type_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'o':
                if (idx < argc_par - 1) {
                    output_good_filename = argv_par[++idx];
                    if (output_good_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'O':
                if (idx < argc_par - 1) {
                    output_bad_filename = argv_par[++idx];
                    if (output_bad_filename[0] == '-') {
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
    if (event_type_filename.IsNull()) return false;
    if (output_good_filename.IsNull()) {
        output_good_filename = "output_good.root";
    }
    if (output_bad_filename.IsNull()) {
        output_bad_filename = "output_bad.root";
    }
    return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << "<SCI_1Q.root> -f <event_type.root> [-o <output_good.root>] [-O <output_bad.root>]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -f <event_type.root>             event type file" << endl;
    cout << "  -o <output_good.root>            good event data file" << endl;
    cout << "  -O <output_bad.root>             bad event data file" << endl;
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
    event_type_filename.Clear();
    output_good_filename.Clear();
    output_bad_filename.Clear();

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
