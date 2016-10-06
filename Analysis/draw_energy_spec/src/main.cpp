#include <iostream>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "RecEventIterator.hpp"
#include "CommonCanvas.hpp"

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

	cout << options_mgr.event_data_filename << endl;
	cout << options_mgr.begin_gps << endl;
	cout << options_mgr.end_gps << endl;
	cout << options_mgr.na22_flag << endl;
	cout << options_mgr.min_bars << endl;
	cout << options_mgr.max_bars << endl;

//    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);

    return 0;
}
