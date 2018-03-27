#include <iostream>
#include <fstream>
#include <sstream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "BarPos.hpp"
#include "PosConv.hpp"

using namespace std;

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
    }


    return 0;
}
