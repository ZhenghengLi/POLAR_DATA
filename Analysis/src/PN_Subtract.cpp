#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "EventIterator.hpp"
#include "PhyEventFile.hpp"
#include "PedMeanCalc.hpp"

using namespace std;

void print_help() {
	cout << endl;
	cout << "USAGE: " << "PN_Subtruct [-p [-s]] [-m] [-v] [-o <outfile_name.root>] <infile_name.root>"  << endl;
	cout << endl;
	cout << " -o <outfile_name.root>           output data to <outfile_name.root>" << endl;
	cout << " -p                               print mean pedestal of all channels" << endl;
	cout << " -s                               print sigma along with mean" << endl;
	cout << " -m                               show mean pedestal map of all channels" << endl;
	cout << " -v                               show all fited pedestal graph" << endl;
	cout << endl;
}

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
		print_help();
		exit(1);
	}
	if (outfile_name.IsNull() && !show_map && !show_fit && !print_ped && !print_sigma) {
		cout << endl;
		cout << "Please use some options to do something." << endl;
		print_help();
		exit(0);
	}
	// ====================================================================
	
	EventIterator eventIter;
	if (!eventIter.open(infile_name.Data())) {
		cerr << "root file open failed." << endl;
		exit(1);
	}

	cout << "Calculating the pedestal of each channel ... " << flush;
	PedMeanCalc pedMeanCalc;
	pedMeanCalc.init(&eventIter);
	pedMeanCalc.do_calc();
	cout << " [Done]" << endl;

	if (print_ped)
		pedMeanCalc.print(print_sigma);

	if (!print_ped && print_sigma)
		pedMeanCalc.print(true);
	
	if (show_map)
		pedMeanCalc.show_mean();
	
	if (show_fit)
		pedMeanCalc.show_all();
	
	if (!outfile_name.IsNull()) {
		cout << "Writing subtructed data to ";
		cout << outfile_name.Data() << " ... " << flush;
		PhyEventFile phyEventFile;
		if (!phyEventFile.open(outfile_name.Data(), 'w')) {
			cerr << "root file to write open failed" << endl;
			exit(1);
		}
		eventIter.trigg_restart();
		while (eventIter.trigg_next()) {
			pedMeanCalc.do_move_trigg(phyEventFile, eventIter);
			phyEventFile.write_trigg();
			while (eventIter.event_next()) {
				pedMeanCalc.do_subtruct(phyEventFile, eventIter);
				phyEventFile.write_event();
			}
		}
		eventIter.trigg_restart();
		phyEventFile.close();
		cout << " [Done]" << endl;
	}

	eventIter.close();
 
	rootapp->Run();
	delete rootapp;
	return 0;
}
