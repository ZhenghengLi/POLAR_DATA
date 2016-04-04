#include <iostream>
#include <cstdlib>
#include "HkGPSIterator.hpp"

using namespace std;

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


    hkGPSIter.close();
    return 0;
}
