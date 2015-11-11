#include <iostream>
#include <fstream>
#include <cstdlib>
#include "SciFrame.hpp"
#include "FileList.hpp"
#include "Processor.hpp"

#define TOTAL_FRAME 500000
#define LOG_FLAG true

using namespace std;

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "USAGE: " << argv[0] << " " << "<list-file-name>" << endl;
		return 0;
	}

	FileList filelist;
	if (!filelist.initialize(argv[1])) {
		cout << "filelist initialization failed!" << endl;
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

	// process the first file
	filelist.next();
	cout << filelist.cur_file() << endl;
	infile.open(filelist.cur_file(), ios::in|ios::binary);
	if (!infile) {
		cerr << "data file open failed" << endl;
		exit(1);
	}
	infile.read(buffer, 2052);
	SciFrame frame(buffer);
	
	// process the first frame
	if (frame.find_start_pos()) {
		if (!frame.check_valid())
			cout << "This frame is invalid! " << frame.get_index() << endl;
		else if (!frame.check_crc())
			cout << "frame CRC Error! " << frame.get_index() <<  endl;
		else
			while (frame.next_packet())
				pro.process_packet(frame);
		pro.cnt.frame++;
	} else {
		cout << "This file may be not a SCI data file." << endl;
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
		if (!pro.process_frame(frame))
			break;
		while (frame.next_packet())
			pro.process_packet(frame);
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
			if (!pro.process_frame(frame))
				break;
			while (frame.next_packet())
				pro.process_packet(frame);
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
