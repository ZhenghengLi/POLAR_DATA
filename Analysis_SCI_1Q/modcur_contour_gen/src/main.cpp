#include <iostream>
#include <fstream>
#include <sstream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "BarPos.hpp"
#include "PosConv.hpp"

/////////////////////////////////////////////////////////////
// MODULATION CURVE ARRAY
/////////////////////////////////////////////////////////////
//
// pol_direction: h1(45, 0, 180) => 2, 6, 10, ... , 174, 178
//
// pol_degree: h1(50, 0, 100) => 1, 3, 5, 7, 9, ... , 97, 99
//
/////////////////////////////////////////////////////////////

using namespace std;

// filename_XXX.mac.root
int extract_pol_direction(const char* filename) {
    string filename_str = filename;
    size_t start_pos = filename_str.length() - 12;
    string pol_direction_str = filename_str.substr(start_pos, 3);
    int pol_direction = atoi(pol_direction_str.c_str());
    if (pol_direction < 2 || pol_direction > 178) return -1;
    if ((pol_direction - 2) % 4 != 0) return -1;
    return pol_direction;
}

bool read_modcur(const char* filename, TH1D& modcur) {
    TFile* modcur_file = new TFile(filename, "read");
    if (modcur_file->IsZombie()) return false;
    TH1D* modcur_hist = static_cast<TH1D*>(modcur_file->Get("modcur_grb_sub_bkg"));
    if (modcur_hist == NULL) return false;
    modcur = *modcur_hist;
    modcur.SetDirectory(NULL);
    modcur_file->Close();
    delete modcur_file;
    return true;
}


int main(int argc, char** argv) {
    OptionsManager options_mgr;
    if (!options_mgr.parse(argc, argv)) {
        if (options_mgr.get_version_flag()) {
            options_mgr.print_version();
        } else {
            options_mgr.print_help();
        }
        return 2;
    }


    return 0;
}
