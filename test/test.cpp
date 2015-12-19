#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "RootInc.hpp"
#include "EventIterator.hpp"

using namespace std;

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "USAGE: " << argv[0] << " <trigger_index>" << endl;
		exit(1);
	}
	int target_index = atoi(argv[1]);
//  TApplication* rootapp = new TApplication("POLAR", &argc, argv);

	EventIterator eventIter;
	eventIter.open("output/sci_test.root");
	while (eventIter.trigg_next()) {
		if (eventIter.trigg.trigg_index < target_index)
			continue;
		while (eventIter.event_next()) {
			cout << setw(10) << eventIter.event.trigg_index
				 << setw(4) << eventIter.event.ct_num 
				 << setw(10) <<  eventIter.event.time_align << endl;
		}
		break;
	}
	eventIter.close();
   
//  rootapp->Run();
	return 0;
}
