#include <iostream>
#include <cstdio>
#include "Constants.hpp"
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

    // open files
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
        cout << "WARNING: no PPD data file input." << endl;
    } else {
        if (!ppdIter.open(options_mgr.ppdfile.Data())) {
            cout << "PPD root file open failed: " << options_mgr.ppdfile.Data() << endl;
            return 1;
        }
        if (ppdIter.get_first_ship_second() - sciIter.get_first_ship_second() > MAX_OFFSET
                || sciIter.get_last_ship_second() - ppdIter.get_last_ship_second() > MAX_OFFSET) {
            cout << "PPD does not match to SCI y ship time." << endl;
            return 1;
        }
    }

    SAPDataFile sapFile;
    if (!sapFile.open(options_mgr.outfile.Data())) {
        cout << "output root file open failed: " << options_mgr.outfile.Data() << endl;
        return 1;
    }

    // merge data
    int pre_percent = 0;
    int cur_percent = 0;
    double total_seconds = sciIter.get_last_ship_second() - sciIter.get_first_ship_second();
    double start_seconds = sciIter.get_first_ship_second();
    if (options_mgr.ppdfile.IsNull()) {
        cout << "Merging SCI and AUX data ..." << endl;
    } else {
        cout << "Merging SCI, AUX and PPD data ..." << endl;
    }
    cout << "[ " << flush;
    while (sciIter.next_event()) {
        cur_percent = static_cast<int>((sciIter.cur_trigger.abs_ship_second - start_seconds) / total_seconds);
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        //TODO
    }
    cout << " DONE ]" << endl;

    // write root


    // write meta


    // close files
    sapFile.close();
    sciIter.close();
    auxIter.close();
    ppdIter.close();

    return 0;
}
