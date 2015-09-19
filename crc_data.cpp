#include <iostream>
#include <fstream>
#include <cstddef>
#include <boost/crc.hpp>

using namespace std;
using boost::crc_optimal;

bool frame_check_valid(const char* buffer) {

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

	uint32_t sci_stater = 0;
	for (int i = 0; i < 4; i++) {
		sci_stater <<= 8;
		sci_stater += static_cast<uint8_t>(buffer[6 + i]);
	}
	if (sci_stater != 0x706F6C61)
		return false;

	return true;

}

uint16_t frame_get_bits(const char* buffer) {
	uint8_t tmp = 0;
	tmp += static_cast<uint8_t>(buffer[2]);
	return static_cast<uint16_t>(tmp >> 6);
}

uint16_t frame_get_index(const char* buffer) {
	uint16_t sum = 0;
	sum += static_cast<uint8_t>(buffer[2] & 0x3F);
	sum <<= 8;
	sum += static_cast<uint8_t>(buffer[3]);
	return sum;
}

bool frame_check_crc(const char* buffer) {
	crc_optimal<32, 0x04C11DB7> crc_32;
	uint32_t expected, res;
	expected = 0;
	for (int i = 0; i < 4; i++) {
		expected <<= 8;
		expected += static_cast<uint8_t>(buffer[2048 + i]);
	}
	crc_32.reset();
	crc_32.process_bytes( buffer, 2048 );
	res = crc_32.checksum();
	if (res == expected)
		return true;
	else
		return false;
}

int main(int argc, char** argv) {
	ifstream infile("POL_SCI_data_20150827_094535_001.dat", ios::in|ios::binary);
	if (!infile) {
		cout << "Can not open file!" << endl;
		exit(1);
	}
	char buffer[2052];
	for (int j = 0; j < 10; j++) {
		infile.read(buffer, 2052);
		if (!frame_check_valid(buffer)) {
			cout << "This frame is not valid" << endl;
			continue;
		}
		cout << "bits: " << fram_get_bits(buffer) << endl;
		cout << "index: " << frame_get_index(buffer) << endl;
		if (frame_check_crc(buffer))
			cout << "CRC check passed" << endl;
		else
			cout << "CRC error!" << endl;
		cout << "-----" << endl;
	}
	return 0;
}
