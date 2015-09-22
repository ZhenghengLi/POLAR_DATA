#include <iostream>
#include <fstream>
#include <cstdlib>
#include "SciFrame.hpp"
#include "FileList.hpp"

#define BUFFERSIZE (2052 * 1)

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
	
	int frame_cnt = 0;
	int counts[] = {0, 0, 0, 0, 0, 0};
	
	ifstream infile;
	char buffer[BUFFERSIZE];

	// process the first file.
	filelist.next();
	infile.open(filelist.cur_file(), ios::in);
	infile.read(buffer, 2052);
	SciFrame frame(buffer);
	infile.read(buffer, 2052);
	if (frame.find_start_pos()) {
		frame.process(counts);
		frame_cnt++;
	} else {
		cout << "This file may be not a SCI data file." << endl;
		exit(1);
	}
	while (!infile.eof()) {
		infile.read(buffer, BUFFERSIZE);
		int bf_frame_cnt = infile.gcount() / 2052;
		if (bf_frame_cnt < 1)
			break;
		for (int i = 0; i < bf_frame_cnt; i++) {
			frame.setdata(buffer + 2052 * i);
			frame.updated();
			frame.process(counts);
			frame_cnt++;
		}
	}
	infile.close();

	// if there are other files
	while (filelist.next()) {
		cout << "new_file_start_flag" << endl;
		infile.open(filelist.cur_file(), ios::in);
		while (!infile.eof()) {
			infile.read(buffer, BUFFERSIZE);
			int bf_frame_cnt = infile.gcount() / 2052;
			if (bf_frame_cnt < 1)
				break;
			for (int i = 0; i < bf_frame_cnt; i++) {
				frame.setdata(buffer + 2052 * i);
				frame.updated();
				frame.process(counts);
				frame_cnt++;
			}
		}
		infile.close();
	}
	
	cout << "***************************" << endl;
	cout << "frame count: " << frame_cnt << endl;
	cout << "trigger count: " << counts[0] << endl;
	cout << "event count: " << counts[1] << endl;
	cout << "packet valid count: " << counts[2] << endl;
	cout << "packet invalid count: " << counts[3] << endl;
	cout << "packet crc pass count: " << counts[4] << endl;
	cout << "packet crc err count: " << counts[5] << endl;
	return 0;
}
