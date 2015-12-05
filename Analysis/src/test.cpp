#include <iostream>
#include <cstdlib>
#include "EventIterator.hpp"

using namespace std;

int main(int argc, char** argv) {
	if (argc < 2) {
		cerr << "USAGE: " << argv[0] << " <infile.root>" << endl;
		exit(1);
	}
	cout << argv[1] << endl;
	EventIterator eventIter;
	if (!eventIter.open(argv[1])) {
		cerr << "root file open failed." << endl;
		exit(1);
	}
	eventIter.close();
	cout << "Done." << endl;
  
	return 0;
}
