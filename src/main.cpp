#include <iostream>
#include <fstream>
#include <cstdlib>
#include "SciFrame.hpp"
#include "FileList.hpp"
#include "Processor.hpp"

#define TOTAL_FRAME 100000000

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

	cout << "***************************" << endl;
	cout << "frame count: " << pro.cnt.frame << endl;
	cout << "packet count: " << pro.cnt.packet << endl;
	cout << "trigger count: " << pro.cnt.trigger << endl;
	cout << "event count: " << pro.cnt.event << endl;
	cout << "packet valid count: " << pro.cnt.pkt_valid << endl;
	cout << "packet invalid count: " << pro.cnt.pkt_invalid << endl;
	cout << "packet crc pass count: " << pro.cnt.pkt_crc_passed << endl;
	cout << "packet crc err count: " << pro.cnt.pkt_crc_error << endl;
	cout << "small length count: " << pro.cnt.pkt_too_short << endl;
	cout << "frame connection error count: " << pro.cnt.frm_con_error << endl;
	cout << "---------------------------" << endl;
	cout << "ped lost:" << endl;
	int64_t trigg_sum = 0;
	int64_t event_sum = 0;
	cout << setfill(' ');
	for (int i = 0; i < 25; i++) {
		cout << setw(7) << pro.ped_trig[i];
		trigg_sum += pro.ped_trig[i];
	}
	cout << endl;
	for (int i = 0; i < 25; i++) {
		cout << setw(7) << pro.ped_event[i];
		event_sum += pro.ped_event[i];
	}
	cout << endl;
	for (int i = 0; i < 25; i++) {
		cout << setw(7) << pro.ped_trig[i] - pro.ped_event[i];
	}
	cout << endl;
	cout << "noped lost:" << endl;
	for (int i = 0; i < 25; i++) {
		cout << setw(7) << pro.noped_trig[i];
		trigg_sum += pro.noped_trig[i];
	}
	cout << endl;
	for (int i = 0; i < 25; i++) {
		cout << setw(7) <<  pro.noped_event[i];
		event_sum += pro.noped_event[i];
	}
	cout << endl;
	for (int i = 0; i < 25; i++) {
		cout << setw(7) << pro.noped_trig[i] - pro.noped_event[i];
	}
	cout << endl;

	cout << "trigg_sum: " << trigg_sum << endl;
	cout << "event_sum: " << event_sum << endl;

	return 0;
}
