#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <boost/crc.hpp>
#include "FileList.hpp"

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

    FileList filelist;
    filelist.add_back(argv[1]);

    filelist.set_start();
    if (!filelist.next_file()) {
        cout << "raw data file open failed" << endl;
        return 1;
    }

    crc_optimal<32, 0x04C11DB7> crc_32_;

    ofstream outfile;
    char filename_buffer[80];

    bool start_flag = true;
    double pre_gps_time = 0;
    double cur_gps_time = 0;
    uint32_t expected, result;
    int file_id = -1;
    while (filelist.next_frame()) {
        // check crc
        expected = 0;
        for (int i = 0; i < 4; i++) {
            expected <<= 8;
            expected += static_cast<uint8_t>(filelist.data_buffer[2048 + i]);
        }
        crc_32_.reset();
        crc_32_.process_bytes(filelist.data_buffer, 2048 );
        result = crc_32_.checksum();
        if (result != expected) continue;

        cur_gps_time = read_gps_time(filelist.data_buffer);

        if (start_flag || cur_gps_time < pre_gps_time) {
            start_flag = false;
            file_id++;
            sprintf(filename_buffer, "sci_segment_%02d.dat", file_id);
            outfile.close();
            cout << "open file: " << filename_buffer << " ... " << endl;
            outfile.open(filename_buffer, ios::out|ios::binary);
            pre_gps_time = cur_gps_time;
            outfile.write(filelist.data_buffer, 2052);
        } else {
            pre_gps_time = cur_gps_time;
            outfile.write(filelist.data_buffer, 2052);
        }

    }

    return 0;
}
