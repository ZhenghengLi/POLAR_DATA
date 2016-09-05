#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdint.h>
#include <stdio.h>
#include "HkFrame.hpp"
#include "HkOdd.hpp"
#include "HkEven.hpp"
#include "Counter.hpp"
#include "Constants.hpp"
#include "HkDataFile.hpp"
#include "FileList.hpp"
#include "RootInc.hpp"

using namespace std;

class Processor {
private:
    // log file
    bool      log_flag_;
    ofstream  os_logfile_;
    // two kind of packet
    HkOdd     ready_odd_packet_;
    HkOdd     odd_packet_;
    HkEven    even_packet_;

    bool odd_is_ready;

public:
    HkFrame   frame;
    Counter   cnt;

private:
    void set_frame_ptr_(const char* frame_ptr);
    
public:
    Processor();
    Processor(const char* frame_ptr);
    ~Processor();
    void initialize();
    void initialize(const char* frame_ptr);
    void process(HkDataFile& datafile);
    // for log
    void set_log(bool flag);
    bool can_log();
    bool logfile_open(const char* filename);
    void logfile_close();
    void do_the_last_work(HkDataFile& datafile);
    void write_meta_info(FileList& filelist, HkDataFile& datafile);
    
};

#endif
