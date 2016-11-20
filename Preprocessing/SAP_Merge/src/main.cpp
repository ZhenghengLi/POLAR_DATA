#include <iostream>
#include <cstdio>
#include "OptionsManager.hpp"
#include "PPDIterator.hpp"

using namespace std;

int main(int argc, char** argv) {
    OptionsManager options_mgr;
    if (!options_mgr.parse(argc, argv)) {
        if (options_mgr.get_version_flag()) {
            options_mgr.print_version();
        } else {
            options_mgr.print_help();
        }
        return 1;
    }

    cout << options_mgr.scifile.Data() << endl;
    cout << options_mgr.auxfile.Data() << endl;
    cout << options_mgr.ppdfile.Data() << endl;
    cout << options_mgr.outfile.Data() << endl;

    PPDIterator ppdIter;
    if (!ppdIter.open(options_mgr.auxfile.Data())) {
        cout << "root file open faild." << endl;
        return 1;
    }

    cout << "first_ship_time: " << ppdIter.get_first_ship_second() << endl;
    cout << "last_ship_time:  " << ppdIter.get_last_ship_second() << endl;

    while (ppdIter.next_ppd()) {
        ppdIter.calc_ppd_interm(ppdIter.ppd_before.ship_time_sec + 0.5);
        cout << ppdIter.ppd_before.wgs84_x << " " << ppdIter.ppd_after.wgs84_x << ppdIter.ppd_interm.wgs84_x << endl;
    }

    ppdIter.close();

    return 0;
}
