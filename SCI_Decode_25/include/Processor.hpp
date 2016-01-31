#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <iterator>
#include "SciFrame.hpp"
#include "SciEvent.hpp"
#include "SciTrigger.hpp"
#include "Counter.hpp"
#include "Constants.hpp"
#include "RootInc.hpp"
#include "EventMerger.hpp"
#include "SciDataFile.hpp"

using namespace std;

class Processor {
private:
    // trigger time
    int pre_ped_trigg_time_;
    bool start_flag_;
    // log file
    bool log_flag_;
    ofstream os_logfile_;
    // two type of packet
    SciEvent sci_event;
    SciTrigger sci_trigger;
    // EventMerger
    EventMerger evtMgr_;

    // packet extra info for trigger
    Long64_t cur_trigg_num_g_;
    Int_t    cur_trigg_pre_is_bad_;
    UInt_t   cur_trigg_pre_time_stamp_;
    UInt_t   cur_trigg_time_period_;
    UInt_t   cur_trigg_pre_raw_dead_;
    bool     cur_trigg_is_first_;
    // packet extra info for event
    Long64_t cur_event_num_g_[25];
    Int_t    cur_event_pre_is_bad_[25];
    UInt_t   cur_event_pre_time_stamp_[25];
    UInt_t   cur_event_time_period_[25];
    UInt_t   cur_event_pre_raw_dead_[25];
    bool     cur_event_is_first_[25];
    
public:
    Counter cnt;
    
public:
    Processor();
    ~Processor();
    void initialize();
    bool process_frame(SciFrame& frame);
    bool process_start(SciFrame& frame, SciDataFile& datafile);
    bool process_restart(SciFrame& frame, SciDataFile& datafile);
    bool interruption_occurred(SciFrame& frame);
    void process_packet(SciFrame& frame, SciDataFile& datafile);
    // for log
    void set_log(bool flag);
    bool can_log();
    bool logfile_open(const char* filename);
    void logfile_close();
    void do_the_last_work(SciDataFile& datafile);
};

#endif
