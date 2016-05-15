#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <iterator>
#include <stdint.h>
#include <stdio.h>
#include "SciFrame.hpp"
#include "SciEvent.hpp"
#include "SciTrigger.hpp"
#include "Counter.hpp"
#include "Constants.hpp"
#include "EventMerger.hpp"
#include "SciDataFile.hpp"
#include "FileList.hpp"

#define SAA_LEN 5
#define SAA_MAX 10

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
    SciEvent   sci_event_;
    SciTrigger sci_trigger_;

    // EventMerger
    EventMerger evtMgr_;

    // packet extra info for trigger
    int64_t    cur_trigg_num_g_;
    int32_t    cur_trigg_pre_is_bad_;
    uint32_t   cur_trigg_pre_time_stamp_;
    uint32_t   cur_trigg_time_period_;
    int32_t    cur_trigg_pre_raw_dead_;
    bool       cur_trigg_is_first_;
    // packet extra info for event
    int64_t    cur_event_num_g_[25];
    int32_t    cur_event_pre_is_bad_[25];
    uint32_t   cur_event_pre_time_stamp_[25];
    uint32_t   cur_event_time_period_[25];
    int32_t    cur_event_pre_raw_dead_[25];
    bool       cur_event_is_first_[25];

    // saa detection
    bool   noped_start_flag_;
    int    noped_trigg_counter_[SAA_LEN];
    int    noped_current_counter_;
    
public:
    Counter cnt;

private:
    void sci_trigger_before_sync_();
    void sci_event_before_sync_(int idx);
    void count_ped_trigger_();
    void save_ped_event_(SciDataFile& datafile);
    
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
    void write_meta_info(FileList& filelist, SciDataFile& datafile);
};

#endif
