#include <iostream>
#include <cstdio>
#include "OptionsManager.hpp"
#include "SCIIterator.hpp"
#include "AUXIterator.hpp"
#include "PPDIterator.hpp"
#include "SAPDataFile.hpp"

#define MAX_OFFSET 10

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

    SCIIterator sciIter;
    if (!sciIter.open(options_mgr.scifile.Data())) {
        cout << "SCI root file open failed: " << options_mgr.scifile.Data() << endl;
        return 1;
    }

    AUXIterator auxIter;
    if (!auxIter.open(options_mgr.auxfile.Data())) {
        cout << "AUX root file open failed: " << options_mgr.auxfile.Data() << endl;
        return 1;
    }
    if (auxIter.get_first_ship_second() - sciIter.get_first_ship_second() > MAX_OFFSET
            || sciIter.get_last_ship_second() - auxIter.get_last_ship_second() > MAX_OFFSET) {
        cout << "AUX does not match to SCI by ship time." << endl;
        return 1;

    }

    PPDIterator ppdIter;
    if (options_mgr.ppdfile.IsNull()) {

    }

    SAPDataFile sapFile;

    return 0;
}
