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
	cout << "USAGE: " << "PN_Subtract <infile_name.root> -F <pedfile_name.root>" << endl;
	cout << "       " << "PN_Subtract <infile_name.root> -f <pedfile_name.root> -o <outfile_name.root>" << endl;
	cout << "       " << "PN_Subtract -f <pedfile_name.root> [-m] [-p] [-s]" << endl;
	cout << endl;
	cout << " -F <pedfile_name.root>           open <pedfile_name.root> in UPDATE mode" << endl;
	cout << " -f <pedfile_name.root>           open <pedfile_name.root> in READ mode" << endl;
	cout << " -o <outfile_name.root>           output data to <outfile_name.root>" << endl;
	cout << " -m                               show mean pedestal map of all channels" << endl;
	cout << " -p                               print mean pedestal of all channels" << endl;
	cout << " -s                               print sigma along with mean" << endl;
	cout << endl;
}

int main(int argc, char** argv) {

	TApplication* rootapp = new TApplication("POLAR", &argc, argv);

	// == process command line parameters =================================

	TString infile_name;
	TString pedfile_name;      // -F | -f
	TString outfile_name;      // -o
	bool show_map = false;     // -m
	bool print_ped = false;    // -p
	bool print_sigma = false;  // -s
	char mode = 'r';                 // 'w' | 'r'
	
	TString cur_par_str;	
	int argv_idx = 0;
	while (argv_idx < rootapp->Argc() - 1) {
		cur_par_str = rootapp->Argv(++argv_idx);
		if (cur_par_str == "-F") {
			pedfile_name = rootapp->Argv(++argv_idx);
			mode = 'w';
		} else if (cur_par_str == "-f") {
			pedfile_name = rootapp->Argv(++argv_idx);
			mode = 'r';
		} else if (cur_par_str == "-o") {
			outfile_name = rootapp->Argv(++argv_idx);
		} else if (cur_par_str == "-m") {
			show_map = true;
		} else if (cur_par_str == "-p") {
			print_ped = true;
		} else if (cur_par_str == "-s") {
			print_sigma = true;
		} else {
			infile_name = cur_par_str;
		}
	}
	
	if (pedfile_name.IsNull()) {
		print_help();
		exit(1);
	}

	// ====================================================================
	
	EventIterator eventIter;
	if (!infile_name.IsNull()) {
		if (!eventIter.open(infile_name.Data())) {
			cerr << "root file open failed: " << infile_name.Data() << endl;
			exit(1);
		}
	}

	PedMeanCalc pedMeanCalc;
	if (!pedMeanCalc.open(pedfile_name.Data(), mode)) {
		cerr << "root file open faild: " << pedfile_name.Data() << endl;
		eventIter.close();
		exit(1);
	}
	
	if (mode == 'w' && !infile_name.IsNull()) {
		cout << "Filling the data of pedestal ..." << endl;
		pedMeanCalc.do_fill(eventIter);
		pedMeanCalc.close();
		cout << "[ DONE ]" << endl;
	}
	
	if (mode == 'r') {
		cout << "Do fitting ..." << endl;
		pedMeanCalc.do_fit();
		cout << "[ DONE ]" << endl;
	}

	
	if (mode == 'r' && print_ped) {
		if (print_sigma)
			cout << "Printing sigma along with mean pedestal of all channels ... " << endl;
		else
			cout << "Printing mean pedestal of all channels ..." << endl;
		pedMeanCalc.print(print_sigma);
		cout << "[ DONE ]" << endl;
	}

	if (mode == 'r' && !print_ped && print_sigma) {
		cout << "Printing sigma along with mean pedestal of all channels ... " << endl;
		pedMeanCalc.print(true);
		cout << "[ DONE ]" << endl;
	}
	
	if (mode == 'r' && show_map) {
		cout << "Showing mean pedestal map of all channels ..." << endl;
		pedMeanCalc.show_mean();
		cout << "[ DONE ]" << endl;
	}
	
	if (mode == 'r' && !infile_name.IsNull() && !outfile_name.IsNull()) {
		int pre_percent = 0;
		int cur_percent = 0;
		cout << "Writing subtructed data to " << outfile_name.Data() << " ... " << endl;
		cout << "[ #" << flush;
		PhyEventFile phyEventFile;
		if (!phyEventFile.open(outfile_name.Data(), 'w')) {
			cerr << "root file open failed: " << outfile_name.Data() << endl;
			eventIter.close();
			pedMeanCalc.close();
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
	
	if (mode == 'r') {
		if (show_map)
			rootapp->Run();
		else
			pedMeanCalc.close();
	}

	return 0;
}
