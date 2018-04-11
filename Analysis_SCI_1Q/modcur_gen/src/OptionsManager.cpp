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
            case 'a':
                if (idx < argc_par - 1) {
                    TString tmp_str = argv_par[++idx];
                    if (tmp_str[0] == '-') {
                        return false;
                    } else {
                        bkg_before_start = tmp_str.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'b':
                if (idx < argc_par - 1) {
                    TString tmp_str = argv_par[++idx];
                    if (tmp_str[0] == '-') {
                        return false;
                    } else {
                        bkg_before_stop = tmp_str.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'c':
                if (idx < argc_par - 1) {
                    TString tmp_str = argv_par[++idx];
                    if (tmp_str[0] == '-') {
                        return false;
                    } else {
                        bkg_after_start = tmp_str.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'd':
                if (idx < argc_par - 1) {
                    TString tmp_str = argv_par[++idx];
                    if (tmp_str[0] == '-') {
                        return false;
                    } else {
                        bkg_after_stop = tmp_str.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'x':
                if (idx < argc_par - 1) {
                    TString tmp_str = argv_par[++idx];
                    if (tmp_str[0] == '-') {
                        return false;
                    } else {
                        grb_start = tmp_str.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'y':
                if (idx < argc_par - 1) {
                    TString tmp_str = argv_par[++idx];
                    if (tmp_str[0] == '-') {
                        return false;
                    } else {
                        grb_stop = tmp_str.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'n':
                if (idx < argc_par - 1) {
                    TString tmp_str = argv_par[++idx];
                    if (tmp_str[0] == '-') {
                        return false;
                    } else {
                        nbins = tmp_str.Atoi();
                    }
                } else {
                    return false;
                }
                break;
            case 't':
                if (idx < argc_par - 1) {
                    TString tmp_str = argv_par[++idx];
                    if (tmp_str[0] == '-') {
                        return false;
                    } else {
                        energy_thr = tmp_str.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'w':
                if (idx < argc_par - 1) {
                    TString tmp_str = argv_par[++idx];
                    if (tmp_str[0] == '-') {
                        return false;
                    } else {
                        rate_binw = tmp_str.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'S':
                subbkg_flag = true;
                break;
            default:
                return false;
            }
        } else {
            if (angle_filename.IsNull()) {
                angle_filename = cur_par_str;
            } else {
                return false;
            }
        }
    }

    if (angle_filename.IsNull()) return false;
    if (subbkg_flag) {
        if (bkg_before_start < 0) return false;
        if (bkg_before_stop < 0) return false;
        if (bkg_after_start < 0) return false;
        if (bkg_after_stop < 0) return false;
        if (grb_start < 0) return false;
        if (grb_stop < 0) return false;
        if (!(bkg_before_start < bkg_before_stop
                && bkg_before_stop < grb_start
                && grb_start < grb_stop
                && grb_stop < bkg_after_start
                && bkg_after_start < bkg_after_stop)
            ) {
            return false;
        }
    }
    if (output_filename.IsNull()) {
        output_filename = "output_modcur.root";
    }
    if (nbins < 0) nbins = 30;
    return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << "<angle_file.root> -o <out.root> -t <thr> -n <nbins> -S -x <grb1> -y <grb2> -a <bkg1> -b <bkg2> -c <bkg3> -d <bkg4> -w <binw>" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -o <output.root>                 output data" << endl;
    cout << "  -t <energy_thr>                  energy_thr, default is 15" << endl;
    cout << "  -n <nbins>                       nbins, default is 30" << endl;
    cout << "  -S                               subbkg_flag" << endl;
    cout << "  -x <grb1>                        grb_start" << endl;
    cout << "  -y <grb2>                        grb_stop" << endl;
    cout << "  -a <bkg1>                        bkg_before_start" << endl;
    cout << "  -b <bkg2>                        bkg_before_stop" << endl;
    cout << "  -c <bkg3>                        bkg_after_start" << endl;
    cout << "  -d <bkg4>                        bkg_after_stop" << endl;
    cout << "  -w <binw>                        binw for angle rate" << endl;
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
    cout << "angle_filename = " << angle_filename << endl;
    cout << "output_filename = " << output_filename << endl;
    cout << "bkg_before_start = " << bkg_before_start << endl;
    cout << "bkg_before_stop = " << bkg_before_stop << endl;
    cout << "bkg_after_start = " << bkg_after_start << endl;
    cout << "bkg_after_stop = " << bkg_after_stop << endl;
    cout << "grb_start = " << grb_start << endl;
    cout << "grb_stop = " << grb_stop << endl;

}

void OptionsManager::init() {
    angle_filename.Clear();
    output_filename.Clear();
    subbkg_flag = false;
    bkg_before_start = -1;
    bkg_before_stop = -1;
    bkg_after_start = -1;
    bkg_after_stop = -1;
    grb_start = -1;
    grb_stop = -1;
    nbins = -1;
    energy_thr = 15.0;
    rate_binw = 0.2;

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
