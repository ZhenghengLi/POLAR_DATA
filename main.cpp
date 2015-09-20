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
	int counts[] = {0, 0, 0, 0};

	char buffer[2052];
	SciFrame frame(buffer);
	for(int i = 0; true; i++) {
		infile.read(buffer, 2052);
		if (infile.eof())
			break;
		frame.updated();
		frame.process(counts);
		frame_cnt++;
	}
	infile.close();
	cout << "***************************" << endl;
	cout << "frame count: " << frame_cnt << endl;
	cout << "trigger count: " << counts[0] << endl;
	cout << "event count: " << counts[1] << endl;
	cout << "packet crc pass count: " << counts[2] << endl;
	cout << "packet crc err count: " << counts[3] << endl;
	return 0;
}
