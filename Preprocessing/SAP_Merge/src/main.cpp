#include <iostream>
#include "OptionsManager.hpp"

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

    return 0;
}
