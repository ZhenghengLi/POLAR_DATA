#include <iostream>
#include <fstream>
#include <cstddef>
#include <cstdlib>
#include <string.h>
#include <stdint.h>

using namespace std;

bool check_sci(const char* buffer) {
	uint16_t frame_header = 0;	
	for (int i = 0; i < 2; i++) {
		frame_header <<= 8;
		frame_header += static_cast<uint8_t>(buffer[0 + i]);
	}
	if (frame_header != 0x009F)
		return false;
	uint16_t frame_length = 0;
	for (int i = 0; i < 2; i++) {
		frame_length <<= 8;
		frame_length += static_cast<uint8_t>(buffer[4 + i]);
	}
	if (frame_length != 0x07F9)
		return false;
	uint32_t sci_starter = 0;
	for (int i = 0; i < 4; i++) {
		sci_starter <<= 8;
		sci_starter += static_cast<uint8_t>(buffer[6 + i]);
	}
	if (sci_starter != 0x706F6C61)
		return false;

	return true;
}

bool check_hk(const char* buffer) {
	uint16_t frame_header = 0;
	for (int i = 0; i < 2; i++) {
		frame_header <<= 8;
		frame_header += static_cast<uint8_t>(buffer[0 + i]);
	}
	if (frame_header != 0x0749)
		return false;
	uint16_t frame_length = 0;
	for (int i = 0; i < 2; i++) {
		frame_length <<= 8;
		frame_length += static_cast<uint8_t>(buffer[4 + i]);
	}
	if (frame_length != 0x00F9)
		return false;
	uint32_t hk_starter = 0;
	for (int i = 0; i < 4; i++) {
		hk_starter <<= 8;
		hk_starter += static_cast<uint8_t>(buffer[6 + i]);
	}
	if (hk_starter != 0xD9A4C2EA)
		return false;

	return true;
}

int check_valid(const char* buffer) {
	if (check_sci(buffer))
		return 1;
	else if (check_hk(buffer))
		return 2;
	else
		return 0;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "Input the file name." << endl;
		exit(1);
	}

	ifstream infile;
	infile.open(argv[1], ios::in|ios::binary);
	if (!infile.is_open()) {
		cerr << "infile open failed." << endl;
		exit(1);
	}
	ofstream scifile;
	scifile.open("POL_SCI_data.dat", ios::out|ios::binary);
	if (!scifile.is_open()) {
		cerr << "scifile open failed." << endl;
		exit(1);
	}
	ofstream hkfile;
	hkfile.open("POL_HK_data.dat", ios::out|ios::binary);
	if (!hkfile.is_open()) {
		cerr << "hkfile open failed." << endl;
		exit(1);
	}

	// == start process ==

	char buffer[2052];
	bool reach_end = false;
	while (true) {
		infile.read(buffer, 10);
		if (infile.eof())
			break;
		while (check_valid(buffer) == 0) {
			memmove(buffer, buffer + 1, 9);
			infile.read(buffer + 9, 1);
			if (infile.eof()) {
				reach_end = true;
				break;
			}
		}
		if (reach_end)
			break;
		if (check_valid(buffer) == 1) {
			infile.read(buffer + 10, 2042);
			scifile.write(buffer, 2052);
		} else {
			infile.read(buffer + 10, 250);
			hkfile.write(buffer, 260);
		}
		
	}

	// == end process ==

	infile.close();
	scifile.close();
	hkfile.close();
	return 0;
}
