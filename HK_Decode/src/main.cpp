#include <iostream>
#include <cstdlib>
#include "Processor.hpp"
#include "FileList.hpp"
#include "HkDataFile.hpp"
#include "OptionsManager.hpp"

using namespace std;

int main(int argc, char** argv) {
    OptionsManager options_mgr;
    if (!options_mgr.parse(argc, argv)) {
        if (options_mgr.get_version_flag()) {
            options_mgr.print_version();
        } else {
            options_mgr.print_help();
        }
        exit(0);
    }

    FileList filelist;
    if (!options_mgr.list_file.IsNull()) {
        if (!filelist.initialize(options_mgr.list_file.Data())) {
            cerr << "filelist initialization failed: "
                 << options_mgr.list_file.Data() << endl;
            exit(1);
        }
    }
    while (!options_mgr.raw_file_queue.empty()) {
        filelist.add_back(options_mgr.raw_file_queue.front().Data());
        options_mgr.raw_file_queue.pop();
    }

    HkDataFile root_datafile;
    if (!options_mgr.out_file.IsNull()) {
        if (!root_datafile.open(options_mgr.out_file.Data())) {
            cerr << "root file open failed: "
                 << options_mgr.out_file.Data() << endl;
            exit(1);
        }
    } else {
        if (!root_datafile.open("POL_HK_decoded_data.root")) {
            cerr << "root file open failed: "
                 << "POL_HK_decoded_data.root" << endl;
            exit(1);
        }
    }

    Processor pro(filelist.data_buffer);
    bool log_flag = false;
    if (!options_mgr.log_file.IsNull()) {
        log_flag = true;
        pro.set_log(log_flag);
        if (!pro.logfile_open(options_mgr.log_file.Data())) {
            cerr << "log file open failed: "
                 << options_mgr.log_file.Data() << endl;
            root_datafile.close();
            exit(1);
        }
    } else {
        pro.set_log(log_flag);
    }

    // === Start Process Data =======================================

    filelist.set_start();
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
    if (log_flag) {
        pro.logfile_close();
    }
    
    pro.cnt.print();

    return 0;
}
