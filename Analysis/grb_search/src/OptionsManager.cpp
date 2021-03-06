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
            case 'o':
                if (idx < argc_par - 1) {
                    pdf_filename = argv_par[++idx];
                    if (pdf_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'a':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        bw_start = tmp_string.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'b':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        bw_stop = tmp_string.Atof();
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
                        bw_len = tmp_string.Atoi();
                    }
                } else {
                    return false;
                }
                break;
            case 'p':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        min_prob = tmp_string.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'd':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        bkg_distance = tmp_string.Atoi();
                    }
                } else {
                    return false;
                }
                break;
            case 'm':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        bkg_nbins = tmp_string.Atoi();
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
            case 'k':
                if (idx < argc_par - 1) {
                    bar_mask_filename = argv_par[++idx];
                    if (bar_mask_filename[0] == '-') {
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
            if (decoded_data_filename.IsNull()) {
                decoded_data_filename = cur_par_str;
            } else {
                return false;
            }
        }
    }
    if (decoded_data_filename.IsNull())
        return false;
    if (begin_gps.IsNull())
        begin_gps = "begin";
    if (end_gps.IsNull())
        end_gps = "end";
    if (bw_start <= 0)
        bw_start = 0.016;
    if (bw_stop <= 0)
        bw_stop = 8.0;
    if (bw_stop <= bw_start)
        return false;
    if (bw_len <= 0)
        bw_len = 10;
    if (min_prob <= 0)
        min_prob = 1.0E-10;
    if (bkg_distance <= 0)
        bkg_distance = 5;
    if (bkg_nbins <= 0)
        bkg_nbins = 5;
    if (pdf_filename.IsNull()) {
        pdf_filename = "found_exceeding_event.pdf";
    }
    TString suffix_str;
    for (int i = pdf_filename.Length() - 1; i > 0 && i > pdf_filename.Length() - 5; i--) {
        suffix_str += pdf_filename[i];
    }
    if (suffix_str != "fdp.") {
        cout << "ERROR: output file must be a pdf file." << endl;
        cout << endl;
        return false;
    }
    return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
    cout << "  " << SW_NAME << " <decoded_data_file.root> [-B <week1:second1>] [-E <week2:second2>]" << endl;
    cout << "  ";
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << " [-a <bw_start>] [-b <bw_stop>] [-n <bw_len>] [-p <min_prob>] [-y <min_bars>] [-z <max_bars>]" << endl;
    cout << "  ";
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << " [-d <bkg_distance>] [-m <bkg_nbins>] -o [<filename.pdf>] [-k <bar_mask.cfg]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -B week1:second1                 GPS string of beginning" << endl;
    cout << "  -E week2:second2                 GPS string of ending" << endl;
    cout << "  -a bw_start                      start bin_width" << endl;
    cout << "  -b bw_stop                       stop bin_width" << endl;
    cout << "  -n bw_len                        number of kinds of bins" << endl;
    cout << "  -p min_prob                      minimum probability" << endl;
    cout << "  -d bkg_distance                  background distance to select" << endl;
    cout << "  -m bkg_nbins                     number of bins to select for background" << endl;
    cout << "  -o filename.pdf                  output pdf filename" << endl;
    cout << "  -y <min_bars>                    minimum number of bars to cut, default is 0" << endl;
    cout << "  -z <max_bars>                    maximum number of bars to cut, default is 4" << endl;
    cout << "  -k <bar_mask.cfg>                file that contains a list of hot bars" << endl;
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
    decoded_data_filename.Clear();
    begin_gps.Clear();
    end_gps.Clear();
    pdf_filename.Clear();
    bw_start = -1;
    bw_stop = -1;
    bw_len = -1;
    min_prob = -1;
    bkg_distance = -1;
    bkg_nbins = -1;
    min_bars = 0;
    max_bars = 4;
    bar_mask_filename.Clear();

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
