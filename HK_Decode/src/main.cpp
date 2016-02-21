#include <iostream>
#include <cstdlib>
#include "Processor.hpp"
#include "FileList.hpp"
#include "HkDataFile.hpp"

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

    HkDataFile root_datafile;
    if (!root_datafile.open("output/hk_test.root")) {
        cerr << "root file open failed. " << endl;
        exit(1);
    }
    
    Processor pro(filelist.data_buffer);
    if (LOG_FLAG) {
        if (!pro.logfile_open("output/hk_test.log")) {
            cerr << "log file open failed." << endl;
            exit(1);
        }
    }

    // === Start Process Data =======================================
    
    while (filelist.next_file()) {
        while (filelist.next_frame()) {
            pro.frame.updated();
            pro.process(root_datafile);
            pro.frame.processed();
        }
    }

    // === End Process Data =========================================

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
