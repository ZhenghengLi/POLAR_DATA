#include <iostream>
#include <iomanip>
#include "OptionsManager.hpp"
#include "Processor.hpp"

using namespace std;

int main(int argc, char** argv) {
    OptionsManager options_mgr;
    if (!options_mgr.parse(argc, argv)) {
        if (options_mgr.get_version_flag()) {
            options_mgr.print_version();
        } else {
            options_mgr.print_help();
        }
        return 0;
    }

    Processor pro(&options_mgr);
    if (pro.open_and_check()) {
        cout << "OK" << endl;
    } else {
        cout << "not OK" << endl;
    }

    return 0;
}
