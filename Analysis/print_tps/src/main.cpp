#include <iostream>
#include <iomanip>
#include <cstdio>
#include "GPSTime.hpp"
#include "HkGPSIterator.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <aux_1M.root>" << endl;
        return 1;
    }
    HkGPSIterator hkIter;
    if (!hkIter.open(argv[1])) {
        cout << "root file open failed" << endl;
        return 1;
    }
    hkIter.initialize();

    char buffer[100];
    cout << left;
    while (hkIter.next_minute()) {
        sprintf(buffer, "%30.10f %30.10f",
                static_cast<double>(hkIter.before_gps_sync.first.week * 604800 + hkIter.before_gps_sync.first.second),
                hkIter.cur_ticks_per_second);
        cout << buffer << endl;
    }

    hkIter.close();
    return 0;
}
