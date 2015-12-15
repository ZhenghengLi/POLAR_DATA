#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "EventIterator.hpp"
#include "PhyEventFile.hpp"
#include "PedMeanCalc.hpp"

using namespace std;

int main(int argc, char** argv) {

	TApplication* rootapp = new TApplication("POLAR", &argc, argv);

	// == process command line parameters =================================
	TString cur_par_str;
	TString infile_name;
	TString outfile_name;      // -o
	bool show_map = false;     // -m
	bool print_ped = false;    // -p
	bool show_fit = false;     // -v
	bool print_sigma = false;  // -s
	int argv_idx = 0;
	while (argv_idx < rootapp->Argc() - 1) {
		cur_par_str = rootapp->Argv(++argv_idx);
		if (cur_par_str == "-o")
			outfile_name = rootapp->Argv(++argv_idx);
		else if (cur_par_str == "-m")
			show_map = true;
		else if (cur_par_str == "-p")
			print_ped = true;
		else if (cur_par_str == "-v")
			show_fit = true;
		else if (cur_par_str == "-s")
			print_sigma = true;
		else
			infile_name = cur_par_str;
	}
	if (infile_name.IsNull()) {
		cout << "USAGE: " << rootapp->Argv(0) << " [-p [-s]] [-m] [-v] [-o <outfile_name.root>] <infile_name.root>" << endl;
		exit(1);
	}
	if (outfile_name.IsNull() && !show_map && !show_fit && !print_ped) {
		cout << "use parameters to do something." << endl;
		exit(0);
	}
	// ====================================================================
	
	EventIterator eventIter;
	if (!eventIter.open(infile_name.Data())) {
		cerr << "root file open failed." << endl;
		exit(1);
	}

	PedMeanCalc pedMeanCalc;
	pedMeanCalc.init(&eventIter);
	pedMeanCalc.do_calc();
	if (print_ped)
		pedMeanCalc.print(print_sigma);
	if (show_map)
		pedMeanCalc.show_mean();		
	if (show_fit)
		pedMeanCalc.show_all();
	if (!outfile_name.IsNull())
		cout << outfile_name.Data() << endl;

	eventIter.close();
 
	rootapp->Run();
	delete rootapp;
	return 0;
}
