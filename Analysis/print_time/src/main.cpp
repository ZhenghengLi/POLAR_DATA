#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "EventIterator.hpp"
#include "Constants.hpp"

using boost::lexical_cast;
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

	ofstream outfile;
	outfile.open(options_mgr.output_filename.Data());

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
        if (!eventIter.t_trigger.abs_gps_valid) {
            continue;
        }
		double cur_seconds = (eventIter.t_trigger.abs_gps_week - GPSStartWeek) * 604800 + (eventIter.t_trigger.abs_gps_second - GPSStartSecond);
		outfile << lexical_cast<string>(cur_seconds) << ", " << eventIter.t_trigger.type << ", " << eventIter.t_trigger.trigger_n << endl;
    }
    cout << " DONE ]" << endl;
    eventIter.close();

	outfile.close();

    return 0;
}
