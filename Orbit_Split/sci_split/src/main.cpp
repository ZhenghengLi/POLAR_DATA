#include <iostream>
#include "SciFileR.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << " <filename.root> <week1:second1> <week2:second2>" << endl;
        return 0;
    }
    SciFileR scifileR;
    if (scifileR.open(argv[1], argv[2], argv[3])) {
        scifileR.print_file_info();
    } else {
        cout << "file open failed" << endl;
    }
    scifileR.close();

    return 0;
}
