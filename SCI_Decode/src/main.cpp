#include <iostream>
#include <fstream>
#include <cstdlib>
#include "SciFrame.hpp"
#include "FileList.hpp"
#include "Processor.hpp"
#include "SciDataFile.hpp"

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

    SciDataFile root_datafile;
    if (!root_datafile.open("output/sci_test.root")) {
        cerr << "root file open failed. " << endl;
        exit(1);
    }
    
    Processor pro;
    pro.set_log(LOG_FLAG);  
    if (LOG_FLAG) {
        if (!pro.logfile_open("output/sci_test.log")) {
            cerr << "log file open failed." << endl;
            exit(1);
        }
    }

    // === Start Process Data =======================================
    
    SciFrame frame(filelist.data_buffer);
    
    // process the first file
    // check file valid and process the first frame
    bool file_is_valid = false;
    if (filelist.next_file()) {
        cout << filelist.cur_file() << endl;
        for (int i = 0; i < 5; i++) {
            if (filelist.next_frame()) {
                frame.updated();
                if (pro.process_frame(frame)) {
                    if (pro.process_start(frame, root_datafile)) {
                        file_is_valid = true;
                        break;
                    }
                }
            } else {
                break;
            }
        }
    } 
    if (!file_is_valid) {
        cerr << "This file may be not a POLAR SCI raw data file." << endl;
        root_datafile.close();
        if (LOG_FLAG) {
            pro.logfile_close();
        }
        exit(1);
    }
    // process other frames in the first file
    while (filelist.next_frame()) {
        frame.updated();
        if (pro.process_frame(frame)) {
            if (pro.interruption_occurred(frame)) {
                cout << "FRAME INTERRUPTION OCCURRED, RESTART." << endl;
                pro.process_restart(frame, root_datafile);
            } else {
                while (frame.next_packet())
                    pro.process_packet(frame, root_datafile);
            }
        }
    }
    // if there are other files
    while (filelist.next_file()) {
        cout << filelist.cur_file() << endl;
        while (filelist.next_frame()) {
            frame.updated();
            if (pro.process_frame(frame)) {
                if (pro.interruption_occurred(frame)) {
                    cout << "FRAME INTERRUPTION OCCURRED, RESTART." << endl;
                    pro.process_restart(frame, root_datafile);
                } else {
                    while (frame.next_packet())
                        pro.process_packet(frame, root_datafile);
                }
            }       
        }
    }

    // === End Process Data ====================================
    
    pro.do_the_last_work(root_datafile);
    root_datafile.close();
    if (LOG_FLAG) {
        pro.logfile_close();
    }

    pro.cnt.print();

    return 0;
}
