#include <iostream>
#include <cstdio>
#include "OptionsManager.hpp"
#include "AUXIterator.hpp"

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

    AUXIterator auxIter;
    if (!auxIter.open(options_mgr.auxfile.Data())) {
        cout << "root file open faild." << endl;
        return 1;
    }

    char str_buffer[200];
    while (auxIter.next_obox()) {
        sprintf(str_buffer, 
                "%10d  %10d  %20.8f  %20.8f %20.8f %20.8f",
                auxIter.hk_obox_before.packet_num,
                auxIter.hk_obox_after.packet_num,
                auxIter.hk_obox_before.abs_ship_second,
                auxIter.hk_obox_after.abs_ship_second,
                auxIter.fe_thr_ship_second_current,
                auxIter.fe_thr_ship_second_next);
        cout << str_buffer << endl;
    }

    return 0;
}
