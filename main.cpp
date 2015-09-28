#include <iostream>
#include <fstream>
#include <cstdlib>
#include "SciFrame.hpp"
#include "FileList.hpp"
#include "process_packet.hpp"

#define TOTAL_FRAME 100000

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
	
	Counter cnt = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	
	ifstream infile;
	char buffer[2052];

	// process the first file
	filelist.next();
	cout << filelist.cur_file() << endl;
	infile.open(filelist.cur_file(), ios::in|ios::binary);
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
				process_packet(frame, cnt);
		cnt.frame++;
	} else {
		cout << "This file may be not a SCI data file." << endl;
		exit(1);
	}
	while (!infile.eof()) {
		if (cnt.frame > TOTAL_FRAME)
			break;
		infile.read(buffer, 2052);
		if (infile.gcount() < 2052)
			break;
		frame.updated();
		if (!frame.check_valid()) {
			cout << "This frame is invalid! " << frame.get_index() << endl;
			break;
		} else if (!frame.check_crc()) {
			cout << "frame CRC Error! " << frame.get_index() << endl;
			break;
		}
		if (!frame.can_connect()) {
			cout << "frame connection error" << endl;
			if (!frame.find_start_pos()) {
				cout << " find_start_pos error" << endl;
				break;
			}
		}
		while (frame.next_packet())
			process_packet(frame, cnt);
		cnt.frame++;
	}
	infile.close();

	// if there are other files
	while (filelist.next()) {
		cout << filelist.cur_file() << endl;
		infile.open(filelist.cur_file(), ios::in|ios::binary);
		while (!infile.eof()) {
			if (cnt.frame > TOTAL_FRAME)
				break;
			infile.read(buffer, 2052);
			if (infile.gcount() < 2052)
				break;
			frame.updated();
			if (!frame.check_valid()) {
				cout << "This frame is invalid! " << frame.get_index() << endl;
				break;
			} else if (!frame.check_crc()) {
				cout << "frame CRC Error! " << frame.get_index() << endl;
				break;
			}
			if (!frame.can_connect()) {
				cout << "frame connection error" << endl;
				if (!frame.find_start_pos()) {
					cout << " find_start_pos error" << endl;
					break;
				}
			}
			while (frame.next_packet())
				process_packet(frame, cnt);
			cnt.frame++;
		}
		infile.close();
	}

	cout << "***************************" << endl;
	cout << "frame count: " << cnt.frame << endl;
	cout << "packet count: " << cnt.packet << endl;
	cout << "trigger count: " << cnt.trigger << endl;
	cout << "event count: " << cnt.event << endl;
	cout << "packet valid count: " << cnt.pkt_valid << endl;
	cout << "packet invalid count: " << cnt.pkt_invalid << endl;
	cout << "packet crc pass count: " << cnt.pkt_crc_passed << endl;
	cout << "packet crc err count: " << cnt.pkt_crc_error << endl;
	cout << "small length count: " << cnt.pkt_too_short << endl;

	return 0;
}
