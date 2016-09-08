#include <iostream>
#include "OptionsManager.hpp"
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

    argc = 1;
//    TApplication* rootapp = new TApplication("POLAR", &argc, argv);

    EventIterator eventIter;
    if (!eventIter.open(options_mgr.decoded_data_filename.Data(),
                      options_mgr.begin_gps.Data(), options_mgr.end_gps.Data())) {
        cerr << "root file open failed: " << options_mgr.decoded_data_filename.Data() << endl;
        return 1;
    }

    eventIter.print_file_info();
    cout << "----------------------------------------------------------" << endl;
    cout << eventIter.phy_begin_trigger.abs_gps_week << endl;
    cout << eventIter.phy_begin_trigger.abs_gps_second << endl;
    cout << eventIter.phy_end_trigger.abs_gps_week << endl;
    cout << eventIter.phy_end_trigger.abs_gps_second << endl;
    cout << eventIter.ped_begin_trigger.abs_gps_week << endl;
    cout << eventIter.ped_begin_trigger.abs_gps_second << endl;
    cout << eventIter.ped_end_trigger.abs_gps_week << endl;
    cout << eventIter.ped_end_trigger.abs_gps_second << endl;
    cout << "----------------------------------------------------------" << endl;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading trigger data ... " << endl;
    cout << "[ " << flush;
    eventIter.phy_trigger_set_start();
    while (eventIter.phy_trigger_next_event()) {
        cur_percent = static_cast<int>(100 * eventIter.phy_trigger_get_cur_entry() / eventIter.phy_trigger_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
    }
    cout << " DONE ]" << endl;

    eventIter.close();
    
    return 0;
}
