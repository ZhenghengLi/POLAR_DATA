#include <iostream>
#include <fstream>
#include <cstdlib>
#include "SciFrame.hpp"
#include "FileList.hpp"

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
	int counts[] = {0, 0, 0, 0, 0, 0, 0};
	
	ifstream infile;
	char buffer[2052];

	// process the first file
	filelist.next();
	cout << filelist.cur_file() << endl;
	infile.open(filelist.cur_file(), ios::in|ios::binary);
	infile.read(buffer, 2052);
	SciFrame frame(buffer);	
	if (frame.find_start_pos()) {
		frame.process(counts);
		frame_cnt++;
	} else {
		cout << "This file may be not a SCI data file." << endl;
		exit(1);
	}
	while (!infile.eof()) {
		infile.read(buffer, 2052);
		if (infile.gcount() < 2052)
			break;
		frame.updated();
		if (!frame.can_connect()) {
			cout << "frame connection error" << endl;
			if (!frame.find_start_pos()) {
				cout << " find_start_pos error" << endl;
				break;
			}
		}
		frame.process(counts);
		frame_cnt++;
	}
	infile.close();

	// if there are other files
	while (filelist.next()) {
		cout << filelist.cur_file() << endl;
		infile.open(filelist.cur_file(), ios::in|ios::binary);
		while (!infile.eof()) {
			infile.read(buffer, 2052);
			if (infile.gcount() < 2052)
				break;
			frame.updated();
			if (!frame.can_connect()) {
				cout << "frame connection error" << endl;
				if (!frame.find_start_pos()) {
					cout << " find_start_pos error" << endl;
					break;
				}
			}
			frame.process(counts);
			frame_cnt++;
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
	cout << "small length count: " << counts[6] << endl;
	return 0;
}
