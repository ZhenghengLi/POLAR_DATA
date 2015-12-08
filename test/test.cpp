#include <iostream>
#include <fstream>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <stdint.h>
#include <stdlib.h>
#include "RootInc.hpp"
#include "EventIterator.hpp"

using namespace std;

int main(int argc, char** argv) {
	if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <infile.root>" << endl;
        exit(1);
    }   

//  TApplication* rootapp = new TApplication("POLAR", &argc, argv);

    EventIterator eventIter;
    eventIter.open(argv[1]);

	int lost_0 = 0;
	int lost_1 = 0;
	int tot_cnt = 0;
    while (eventIter.trigg_next()) {
		tot_cnt += eventIter.trigg.pkt_count + eventIter.trigg.lost_count;
		if (eventIter.trigg.pkt_count == 0) {
			lost_0 += eventIter.trigg.lost_count;
		} else {
			lost_1 += eventIter.trigg.lost_count;
		}
    }   
    
    eventIter.close();
   
 	cout << lost_0 << endl;
	cout << lost_1 << endl;
	cout << tot_cnt << endl;
   
//  rootapp->Run();
	return 0;
}
