#include <iostream>
#include <fstream>
#include "SciFrame.hpp"

using namespace std;

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "USAGE: " << argv[0] << " " << "<sci-file-name>" << endl;
		return 0;
	}

	ifstream infile(argv[1], ios::in|ios::binary);
	if (!infile) {
		cout << "Can not open file!" << endl;
		exit(1);
	}

	int frame_cnt = 0;
	int crc_error_cnt = 0;

	char buffer[2052];
	SciFrame frame(buffer);
	while (!infile.eof()) {
		infile.read(buffer, 2052);
		if (!infile.good())
			break;
		if (!frame.check_valid()) {
			cout << "This frame is not valid" << endl;
			continue;
		}
		cout << "bits: " << frame.get_bits() << endl;
		cout << "index: " << frame.get_index() << endl;
		if (frame.check_crc())
			cout << "CRC check passed" << endl;
		else {
			cout << "CRC error!" << endl;
			crc_error_cnt++;
		}
		cout << "-----" << endl;
		frame_cnt++;
	}
	infile.close();
	cout << "==============" << endl;
	cout << "frame count: " << frame_cnt << endl;
	cout << "crc error count: " << crc_error_cnt << endl;
	return 0;
}
