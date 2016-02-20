#include <iostream>
#include <cstdlib>
#include "HkFrame.hpp"
#include "FileList.hpp"

#define LOG_FLAG true

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " " << "<list-file-name>" << endl;
        return 0;
    }

    FileList filelist;
    if (!filelist.initialize(argv[1])) {
        cerr << "filelist initialization failed!" << endl;
        exit(1);
    }

    HkFrame frame(filelist.data_buffer);
    int cnt_valid = 0;
    int cnt_invalid = 0;
    int cnt_crc_passed = 0;
    int cnt_crc_error = 0;
    int cnt_cannot_connect = 0;
    while (filelist.next_file()) {
        while (filelist.next_frame()) {
            frame.updated();
            if (frame.check_valid())
                cnt_valid++;
            else
                cnt_invalid++;
            if (frame.check_crc())
                cnt_crc_passed++;
            else
                cnt_crc_error++;
            if (!frame.can_connect())
                cnt_cannot_connect++;
            cout << frame.get_index() << " " << frame.get_pkt_tag() << endl;
            frame.processed();
        }
    }

    cout << "     valid: " << cnt_valid << endl;
    cout << "   invalid: " << cnt_invalid << endl;
    cout << "crc_passed: " << cnt_crc_passed << endl;
    cout << " crc_error: " << cnt_crc_error << endl;
    cout << "cannot_con: " << cnt_cannot_connect << endl;
    
    return 0;
}
