#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cstdio>
#include "EventIterator.hpp"
#include "PhyEventFile.hpp"
#include "PedMeanCalc.hpp"

using namespace std;

void print_help() {
	cout << endl;
	cout << "USAGE: " << "PN_Subtruct [-p [-s]] [-m] [-v] [-o <outfile_name.root>] <infile_name.root>"  << endl;
	cout << endl;
	cout << " -o <outfile_name.root>           output data to <outfile_name.root>" << endl;
	cout << " -m                               show mean pedestal map of all channels" << endl;
	cout << " -p                               print mean pedestal of all channels" << endl;
	cout << " -s                               print sigma along with mean" << endl;
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
		else if (cur_par_str == "-s")
			print_sigma = true;
		else
			infile_name = cur_par_str;
	}
	if (infile_name.IsNull()) {
		print_help();
		exit(1);
	}
	if (outfile_name.IsNull() && !show_map && !print_ped && !print_sigma) {
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
	cout << " [ DONE ]" << endl;

	if (print_ped) {
		if (print_sigma)
			cout << "Printing sigma along with mean pedestal of all channels ... " << endl;
		else
			cout << "Printing mean pedestal of all channels ..." << endl;
		pedMeanCalc.print(print_sigma);
	}

	if (!print_ped && print_sigma) {
		cout << "Printing sigma along with mean pedestal of all channels ... " << endl;
		pedMeanCalc.print(true);
	}
	
	if (show_map) {
		cout << "Showing mean pedestal map of all channels ..." << endl;
		pedMeanCalc.show_mean();
	}
	
	if (!outfile_name.IsNull()) {
		int pre_percent = 0;
		int cur_percent = 0;
		cout << "Writing subtructed data to " << outfile_name.Data() << " ... [ #" << flush;
		PhyEventFile phyEventFile;
		if (!phyEventFile.open(outfile_name.Data(), 'w')) {
			cerr << "root file to write open failed" << endl;
			exit(1);
		}
		eventIter.trigg_restart();
		while (eventIter.trigg_next()) {
			cur_percent = static_cast<int>(100 * eventIter.current_index() / eventIter.total_entries());
			if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
				pre_percent = cur_percent;
				cout << "#" << flush;
			}
			pedMeanCalc.do_move_trigg(phyEventFile, eventIter);
			phyEventFile.write_trigg();
			while (eventIter.event_next()) {
				pedMeanCalc.do_subtruct(phyEventFile, eventIter);
				phyEventFile.write_event();
			}
		}
		eventIter.trigg_restart();
		phyEventFile.close();
		cout << " DONE ] " << endl;
	}

	eventIter.close();

	cout << "All Finished." << endl;
 
	rootapp->Run();
	delete rootapp;
	return 0;
}
