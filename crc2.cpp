#include <iostream>
#include <cstddef>
#include <boost/crc.hpp>

using namespace std;
using boost::crc_optimal;

int main(int argc, char** argv)
{
	unsigned char const  data[] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39 };
	size_t const data_len = sizeof( data ) / sizeof( data[0] );

	cout << data_len << endl;

	crc_optimal<16, 0x1021>  crc_ccitt;
	crc_ccitt.process_bytes( data, data_len );
	uint16_t res = crc_ccitt.checksum();
	cout << hex << res << endl;

	return 0;
}
