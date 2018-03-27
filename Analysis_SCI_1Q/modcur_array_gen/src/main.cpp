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

int extract_pol_direction(const char* filename) {
    string filename_str = filename;
    size_t start_pos = filename_str.length() - 12;
    string pol_direction_str = filename_str.substr(start_pos, 3);
    cout << pol_direction_str << endl;

    return 0;
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



    for (size_t i = 0; i < options_mgr.pol_modcur_filelist.size(); i++) {
        cout << options_mgr.pol_modcur_filelist[i] << endl;
        extract_pol_direction(options_mgr.pol_modcur_filelist[i].Data());
    }


    return 0;
}
