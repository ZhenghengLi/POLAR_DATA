#include <iostream>
#include <fstream>
#include <cstdlib>
#include "SciFrame.hpp"
#include "FileList.hpp"
#include "Processor.hpp"
#include "SciDataFile.hpp"
#include "OptionsManager.hpp"

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

    bool have_started = false;

    filelist.set_start();
    while (filelist.next_file()) {
        while (filelist.next_frame()) {
            frame.updated();
            if (pro.process_frame(frame)) {
                if (!have_started) {
                    if (pro.process_start(frame, root_datafile)) {
                        have_started = true;
                    }
                } else if (pro.interruption_occurred(frame)) {
                    cout << "FRAME INTERRUPTION OCCURRED, RESTART." << endl;
                    if (!pro.process_restart(frame, root_datafile)) {
                        have_started = false;
                    }
                } else {
                    while (frame.next_packet()) {
                        pro.process_packet(frame, root_datafile);
                    }
                }
            }       
        }
    }

    // === End Process Data ====================================
    
    pro.do_the_last_work(root_datafile);
    root_datafile.write_after_decoding();
    pro.write_meta_info(filelist, root_datafile);
    root_datafile.close();
    if (LOG_FLAG) {
        pro.logfile_close();
    }

    pro.cnt.print();

    return 0;
}
