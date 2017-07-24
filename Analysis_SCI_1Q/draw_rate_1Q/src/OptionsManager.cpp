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
            case 'R':
                if (idx < argc_par - 1) {
                    TString tmp_arg = argv_par[++idx];
                    grb_ra = tmp_arg.Atof();
                } else {
                    return false;
                }
                break;
            case 'D':
                if (idx < argc_par - 1) {
                    TString tmp_arg = argv_par[++idx];
                    grb_dec = tmp_arg.Atof();
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
                        binwidth = tmp_string.Atof();
                    }
                } else {
                    return false;
                }
                break;
            case 'j':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        phase = tmp_string.Atoi();
                    }
                } else {
                    return false;
                }
                break;
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
            case 'W':
                if (idx < argc_par - 1) {
                    weight_filename = argv_par[++idx];
                    if (weight_filename[0] == '-') {
                        return false;
                    }
                } else {
                    return false;
                }
                break;
            case 'm':
                tout1_flag = true;
                break;
            case 'A':
                align_hist = false;
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
            case 'n':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        niter = tmp_string.Atoi();
                    }
                } else {
                    return false;
                }
                break;
            case 's':
                if (idx < argc_par - 1) {
                    TString tmp_string = argv_par[++idx];
                    if (tmp_string[0] == '-') {
                        return false;
                    } else {
                        min_signif = tmp_string.Atof();
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
    if (max_bars < 0)
        max_bars = 1600;
    if (min_bars < 0)
        min_bars = 0;
    if (max_bars < min_bars)
        return false;
    if (max_bars > 1600)
        return false;
    if (grb_ra < 0 || grb_ra >= 24 || grb_dec < -90 || grb_dec >= 90) {
        cout << "radec of GRB is out of range: RA(0 ~ 24), DEC(-90 ~ 90)" << endl;
        return false;
    }
    if (binwidth <= 0)
        binwidth = 1.0;
    if (phase != 0 && phase != 1 && phase != 2 && phase != 3) {
        phase = 0;
    }
    if (niter < 0)
        niter = 20;
    if (min_signif < 0)
        min_signif = 4.5;
    return true;
}

void OptionsManager::print_help() {
    cout << "Usage:" << endl;
	cout << "  " << SW_NAME << " <SCI_1Q.root> [-B <met_second1>] [-E <met_second2>]" << endl;
    cout << "  ";
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << " [-w <binwidth>] [-j <phase>] [-o <filename.root>] -y [min_bars] [-z <max_bars>]" << endl;
    cout << "  ";
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << "[-W <weight.root>] [-k <bar_mask.cfg>] [-n <niter>] [-s <min_signif>]" << endl;
    for (size_t i = 0; i < SW_NAME.length(); i++)
        cout << " ";
    cout << "[-R <grb_ra>] [-D <grb_dec>] [-A]" << endl;
    cout << "  ";
    cout << endl;
    cout << "Options:" << endl;
    cout << "  -B met_second1                   MET time of beginning" << endl;
    cout << "  -E met_second2                   MET time of ending" << endl;
    cout << "  -w binwidth                      bin width of seconds" << endl;
    cout << "  -j phase                         phase shift, must be 0, 1, 2 or 3" << endl;
    cout << "  -m                               draw tout1 rate of 25 modules" << endl;
    cout << "  -o <filename.root>               output rate hist to a root file" << endl;
    cout << "  -y <min_bars>                    minimum number of bars to cut, default is 0" << endl;
    cout << "  -z <max_bars>                    maximum number of bars to cut, default is 1600" << endl;
    cout << "  -W weight.root                   weight file" << endl;
    cout << "  -k <bar_mask.cfg>                file that contains a list of hot bars" << endl;
    cout << "  -n <niter>                       numberIterations for background calculation" << endl;
    cout << "  -s <min_signif>                  min significance level for T0 calculation" << endl;
    cout << "  -R <grb_ra>                      RA(hour) of GRB" << endl;
    cout << "  -D <grb_dec>                     DEC(degree) of GRB" << endl;
    cout << "  -A                               Disable histogram alignment." << endl;
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
    begin_met_time = -1;
    end_met_time = -1;
    binwidth = -1;
    phase = -1;
    tout1_flag = false;
    max_bars = -1;
    min_bars = -1;
    bar_mask_filename.Clear();
    niter = -1;
    min_signif = -1;
    weight_filename.Clear();
    grb_ra = 5.575547;     // RA  of Crab Pulsar
    grb_dec = 22.0144722;  // DEC of Crab Pulsar
    align_hist = true;

    version_flag_ = false;
}

bool OptionsManager::get_version_flag() {
    return version_flag_;
}
