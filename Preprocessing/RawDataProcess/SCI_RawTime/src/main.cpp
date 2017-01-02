#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <boost/crc.hpp>

#define BUFFER_SIZE 2052

using namespace std;
using boost::crc_optimal;

int week_of_gps6(const uint64_t raw_gps) {
    return static_cast<int>((raw_gps >> 32) & 0xFFFF);
}

double second_of_gps6(const uint64_t raw_gps) {
    return static_cast<double>((raw_gps >> 12) & 0xFFFFF) + static_cast<double>(raw_gps & 0xFFF) * 0.5 * 1.0E-3;
}

double read_gps_time(char* frame) {
    int64_t frm_gps_time_ = 0;
    for (int i = 0; i < 6; i++) {
        frm_gps_time_ <<= 8;
        frm_gps_time_ += static_cast<uint8_t>(frame[16 + i]);
    }
    return week_of_gps6(frm_gps_time_) * 604800 + second_of_gps6(frm_gps_time_);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <rawdata_filename.dat>" << endl;
        return 2;
    }

    ifstream infile;
    infile.open(argv[1], ios::in|ios::binary);
    if (!infile.is_open()) {
        cout << "raw data file open failed" << endl;
        return 1;
    }

    crc_optimal<32, 0x04C11DB7> crc_32_;
    uint32_t expected, result;

    char buffer[BUFFER_SIZE];
    int shift = -1;
    int try_num = 0;
    bool found = false;
    // read first frame
    while (try_num < BUFFER_SIZE * 5) {
        shift++;
        infile.read(buffer, BUFFER_SIZE);
        expected = 0;
        for (int i = 0; i < 4; i++) {
            expected <<= 8;
            expected += static_cast<uint8_t>(buffer[2048 + i]);
        }
        crc_32_.reset();
        crc_32_.process_bytes(buffer, 2048 );
        result = crc_32_.checksum();
        if (result == expected) {
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "cannot find the first frame" << endl;
        return 1;
    }

    int64_t first_gps_time = static_cast<int64_t>(read_gps_time(buffer));
    cout << "first_gps_time => " << first_gps_time / 604800 << ":" << first_gps_time % 604800 << endl;

    // read last frame
    try_num = 0;
    found = false;
    shift = BUFFER_SIZE - 1;
    while (try_num < BUFFER_SIZE * 5) {
        shift++;
        infile.seekg(-1 * shift, ios::end);
        infile.read(buffer, BUFFER_SIZE);
        expected = 0;
        for (int i = 0; i < 4; i++) {
            expected <<= 8;
            expected += static_cast<uint8_t>(buffer[2048 + i]);
        }
        crc_32_.reset();
        crc_32_.process_bytes(buffer, 2048 );
        result = crc_32_.checksum();
        if (result == expected) {
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "cannot find the last frame" << endl;
        return 1;
    }

    int64_t last_gps_time = static_cast<int64_t>(read_gps_time(buffer));
    cout << "last_gps_time => " << last_gps_time / 604800 << ":" << last_gps_time % 604800 << endl;

    infile.close();

    return 0;
}
