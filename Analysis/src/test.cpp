#include <iostream>
#include <cstdlib>
#include "EventIterator.hpp"
#include "PhyEventFile.hpp"

using namespace std;

int main(int argc, char** argv) {
	if (argc < 2) {
		cerr << "USAGE: " << argv[0] << " <infile.root>" << endl;
		exit(1);
	}
	cout << argv[1] << endl;
	PhyEventFile phyeventfile;
	EventIterator eventIter;
	if (!eventIter.open(argv[1])) {
		cerr << "root file open failed." << endl;
		exit(1);
	}
	while (eventIter.trigg_next()) {
		while (eventIter.event_next()) {

		}
	}

	while (eventIter.ped_trigg_next()) {
		while (eventIter.ped_event_next()) {

		}
	}

	eventIter.close();
	cout << "Done." << endl;
  
	return 0;
}
