#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "EventIterator.hpp"
#include "PhyEventFile.hpp"
#include "PedMeanCalc.hpp"

using namespace std;

int main(int argc, char** argv) {
	if (argc < 2) {
		cerr << "USAGE: " << argv[0] << " <infile.root>" << endl;
		exit(1);
	}
	char filename[80];
	strcpy(filename, argv[1]);
	TApplication* rootapp = new TApplication("POLAR", &argc, argv);
	EventIterator eventIter;
	if (!eventIter.open(filename)) {
		cerr << "root file open failed." << endl;
		exit(1);
	}

	PedMeanCalc pedMeanCalc;
	pedMeanCalc.init(&eventIter);
	pedMeanCalc.do_calc();
	for (int i = 0; i < 25; i++)
		pedMeanCalc.show(i + 1);
	cout << setw(3) << " " << " | ";
	for (int i = 0; i < 25; i++)
		cout << setw(5) << i + 1;
	cout << endl;
	cout << endl;
	for (int j = 0; j < 64; j++) {
		cout << setw(3) << j + 1 << " | ";
		for (int i = 0; i < 25; i++) {
			cout << setw(5) << pedMeanCalc.mean[i][j];
		}
		cout << endl;
		cout << setw(3) << " " << " | ";
		for (int i = 0; i < 25; i++) {
			cout << setw(5) << pedMeanCalc.sigma[i][j];
		}
		cout << endl;
		cout << endl;
	}

	eventIter.close();
 
   rootapp->Run();	
	return 0;
}
