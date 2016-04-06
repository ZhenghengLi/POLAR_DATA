#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <stdint.h>
#include "GPSTime.hpp"
#include "HkGPSIterator.hpp"
#include "SciTransfer.hpp"
#include "OptionsManager.hpp"

using namespace std;

void print_time(const GPSTime gps, const uint32_t timestamp) {
    cout << left << setprecision(15)
         << setw(6) << "week:" << setw(10) << gps.week
         << setw(8) << "second:" << setw(20) << gps.second
         << setw(11) << "timestamp:" << setw(20) << timestamp
         << right
         << endl;
}

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

    SciTransfer scitran;
    if (!scitran.open_read(options_mgr.scifile)) {
        cerr << "file open failed: " << options_mgr.scifile << endl;
        return 1;
    }

    HkGPSIterator hkGPSIter;
    if (!hkGPSIter.open(options_mgr.hkfile)) {
        cerr << "file open falied: " << options_mgr.hkfile << endl;
        return 1;
    }

    cout << "ped_first ----" << endl;
    print_time(scitran.ped_first_gps, scitran.ped_first_timestamp);
    cout << "phy_first ----" << endl;
    print_time(scitran.phy_first_gps, scitran.phy_first_timestamp);

    cout << "all_hk_time ----" << endl;
    hkGPSIter.initialize();
    print_time(hkGPSIter.after_gps_sync.first, hkGPSIter.after_gps_sync.second);
    while (hkGPSIter.next_minute()) {
        print_time(hkGPSIter.after_gps_sync.first, hkGPSIter.after_gps_sync.second);
    }

    cout << "ped_last ----" << endl;
    print_time(scitran.ped_last_gps, scitran.ped_last_timestamp);
    cout << "phy_last ----" << endl;
    print_time(scitran.phy_last_gps, scitran.phy_last_timestamp);

    scitran.close_read();
    hkGPSIter.close();

    return 0;
}
