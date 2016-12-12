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

    cout << options_mgr.decoded_data_filename.Data() << endl;
    cout << options_mgr.begin_gps.Data() << endl;
    cout << options_mgr.end_gps.Data() << endl;
    cout << options_mgr.pdf_filename.Data() << endl;
    cout << options_mgr.min_signif << endl;
    cout << options_mgr.min_bars << endl;
    cout << options_mgr.max_bars << endl;
    cout << options_mgr.bar_mask_filename.Data() << endl;

    return 0;
}
