#include <iostream>
#include <cstdlib>
#include "HkGPSIterator.hpp"

using namespace std;

void print(const HkGPSIterator& iter) {
    cout << left
         << setw(6) << "week:"
         << setw(10) << iter.after_gps_sync.first.week
         << setw(8) << "second:"
         << setw(16) << iter.after_gps_sync.first.second
         << setw(11) << "timestamp:"
         << setw(16) << iter.after_gps_sync.second
         << setw(18) << "ticks_per_second:"
         << setw(16) << static_cast<int>(iter.cur_ticks_per_second)
         << right
         << endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE" << argv[0] << " <filename>" << endl;
        return 0;
    }

    HkGPSIterator hkGPSIter;
    if (!hkGPSIter.open(argv[1])) {
        cerr << "file open failed: " << argv[1] << endl;
        return 1;
    }

    hkGPSIter.initialize();
    print(hkGPSIter);

    while (hkGPSIter.next_minute()) {
        print(hkGPSIter);
    }

    hkGPSIter.close();
    return 0;
}
