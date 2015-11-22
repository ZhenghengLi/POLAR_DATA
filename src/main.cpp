#include <iostream>
#include <fstream>
#include <cstdlib>
#include "SciFrame.hpp"
#include "FileList.hpp"
#include "Processor.hpp"

#define TOTAL_FRAME 100000000
#define LOG_FLAG true

using namespace std;

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "USAGE: " << argv[0] << " " << "<list-file-name>" << endl;
		return 0;
	}

	FileList filelist;
	if (!filelist.initialize(argv[1])) {
		cerr << "filelist initialization failed!" << endl;
		exit(1);
	}
	
	Processor pro;
	if (pro.rootfile_open("output/test.root")) {
	} else {
		cerr << "root file open failed" << endl;
		exit(1);
	}
	if (LOG_FLAG) {
		if (pro.logfile_open("output/test.log")) {
		} else {
			cerr << "log file open failed" << endl;
			exit(1);
		}
	}
	pro.set_log(LOG_FLAG);

	ifstream infile;
	char buffer[2052];
	SciFrame frame(buffer);	

	// process the first file
	filelist.next();
	cout << filelist.cur_file() << endl;
	infile.open(filelist.cur_file(), ios::in|ios::binary);
	if (!infile) {
		cerr << "data file open failed" << endl;
		exit(1);
	}
	
	// check file valid and process the first frame
	bool file_is_valid = false;
	for (int i = 0; i < 5; i++) {
		infile.read(buffer, 2052);
		frame.updated();
		if (pro.process_frame(frame)) {
			if (pro.process_start(frame)) {
				file_is_valid = true;
				break;
			} else {
				continue;
			}
		} else {
			continue;
		}
	}
	
	if (!file_is_valid) {
		cerr << "This file may be not a POLAR SCI raw data file." << endl;
		exit(1);
	}
	
	// process other frames in the first file
	while (!infile.eof()) {
		if (pro.cnt.frame > TOTAL_FRAME)
			break;
		infile.read(buffer, 2052);
		if (infile.gcount() < 2052)
			break;
		frame.updated();
		if (pro.process_frame(frame)) {
			if (pro.interruption_occurred(frame)) {
				cout << "interruption occurred, restart." << endl;
				pro.process_restart(frame);
			} else {
				while (frame.next_packet())
					pro.process_packet(frame);
			}
		} else {
			continue;
		}
	}
	infile.close();

	// if there are other files
	while (filelist.next()) {
		cout << filelist.cur_file() << endl;
		infile.open(filelist.cur_file(), ios::in|ios::binary);
		if (!infile) {
			cerr << "data file open failed" << endl;
			exit(1);
		}
		while (!infile.eof()) {
			if (pro.cnt.frame > TOTAL_FRAME)
				break;
			infile.read(buffer, 2052);
			if (infile.gcount() < 2052)
				break;
			frame.updated();
			if (pro.process_frame(frame)) {
				if (pro.interruption_occurred(frame)) {
					cout << "interruption occurred, restart." << endl;
					pro.process_restart(frame);
				} else {
					while (frame.next_packet())
						pro.process_frame(frame);
				}
			} else {
				continue;
			}
		}
		infile.close();
	}

	pro.do_the_last_work();
	pro.rootfile_close();
	if (LOG_FLAG) {
		pro.logfile_close();
	}

	pro.cnt.print();

	return 0;
}
