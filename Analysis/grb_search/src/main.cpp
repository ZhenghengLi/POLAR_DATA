#include <iostream>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "EventIterator.hpp"

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

    cout << "begin_gps: " << options_mgr.begin_gps << endl;
    cout << "end_gps: " << options_mgr.end_gps << endl;
    cout << "filename: " << options_mgr.decoded_data_filename << endl;
    cout << "bw_start: " << options_mgr.bw_start * 1 << endl;
    cout << "bw_stop: " << options_mgr.bw_stop * 1 << endl;
    cout << "bw_len: " << options_mgr.bw_len * 1 << endl;
    cout << "min_prob: " << options_mgr.min_prob * 1 << endl;
    cout << "bkg_distance: " << options_mgr.bkg_distance * 1 << endl;
    cout << "bkg_nbins: " << options_mgr.bkg_nbins * 1 << endl;

    return 0;

//    argc = 1;
//    TApplication* rootapp = new TApplication("POLAR", &argc, argv);

    EventIterator eventIter;
    if (!eventIter.open(options_mgr.decoded_data_filename.Data(),
                      options_mgr.begin_gps.Data(), options_mgr.end_gps.Data())) {
        cerr << "root file open failed: " << options_mgr.decoded_data_filename.Data() << endl;
        return 1;
    }

    if (!eventIter.file_is_1P()) {
        cout << "error: The opened file may be not 1P/1R SCI data file." << endl;
        eventIter.close();
        return 1;
    }
    
    eventIter.print_file_info();

    Double_t gps_time_length = (eventIter.phy_end_trigger.abs_gps_week - eventIter.ped_begin_trigger.abs_gps_week) * 604800 +
        (eventIter.phy_end_trigger.abs_gps_second - eventIter.phy_begin_trigger.abs_gps_second);

    // prepare histogram
    
    return 0;
}
