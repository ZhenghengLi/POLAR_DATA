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

	TH1F* h1 = new TH1F("name", "title", 50, 0, 500);
	delete h1;
   
//  rootapp->Run();
	return 0;
}
