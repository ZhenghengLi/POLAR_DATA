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
                    auxfile = argv_par[++idx];
                    if (auxfile[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'p':
                if (idx < argc_par - 1) {
                    ppdfile = argv_par[++idx];
                    if (ppdfile[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'o':
                if (idx < argc_par - 1) {
                    outfile = argv_par[++idx];
                    if (outfile[0] == '-') {
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
            if (scifile.IsNull())
                scifile = cur_par_str;
            else
                return false;
        }
    }
    if (scifile.IsNull() || auxfile.IsNull())
        return false;
    else
        return true;
    if (outfile.IsNull()) {
        outfile = "POL_SCI_data_1Q.root";
    }
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " " << "<SCI_1P.root> -a <AUX_1M.root> [-p <PPD_1N.root>] [-o <SCI_1Q.root>]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -a <AUX_1M.root>                 root file of AUX_1M level data" << endl;
    cout << "  -p <PPD_1N.root>                 root file of PPD_1N level data" << endl;
    cout << "  -o <SCI_1Q.root>                 root file to store SCI_1Q level data" << endl;
    cout << endl;
    cout << "  --version                        print version and author information" << endl;
    cout << endl;
}

void OptionsManager::print_version() {
    cout << endl;
    cout << "    " << SW_NAME << " - Merge SCI/AUX/PPD data together" << endl;
    cout << "    " << SW_VERSION << " (" << RELEASE_DATE << ", compiled " << __DATE__ << " " << __TIME__ << ")" << endl;
    cout << endl;
    cout << " Copyright (C) 2015-2016 Zhengheng Li <lizhengheng@ihep.ac.cn>" << endl;
    cout << endl;
}

void OptionsManager::init() {
    scifile.Clear();
    auxfile.Clear();
    ppdfile.Clear();
    outfile.Clear();
    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
