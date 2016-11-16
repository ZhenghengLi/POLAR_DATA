#include <iostream>
#include "OptionsManager.hpp"
#include "SCIIterator.hpp"

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
        cout << "root file open faild." << endl;
        return 1;
    }

    while (sciIter.next_event()) {
        cout << sciIter.cur_trigger.trigg_num_g << " " << sciIter.cur_trigger.type << " | " << flush;
        while(sciIter.next_packet()) {
            cout << "(" << sciIter.cur_modules.ct_num << ", " << sciIter.cur_modules.compress << ") " << flush;
        }
        cout << endl;
    }

    return 0;
}
