#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <cstdlib>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "RecEventDataFile.hpp"
#include "AngleDataFile.hpp"
#include "EventFilter.hpp"
#include "CurveShow.hpp"
#include "CooConv.hpp"
#include "Constants.hpp"


using namespace std;

class Processor {
private:
    OptionsManager* cur_options_mgr_;
    TApplication*   cur_rootapp_;

    RecEventDataFile rec_event_data_file_;
    AngleDataFile    angle_data_file_;
    EventFilter      event_filter_;
    CurveShow        curve_show_;

private:
    int do_action_1_();
    int do_action_2_();
    
public:
    Processor(OptionsManager* my_options_mgr,
              TApplication* my_rootapp);
    ~Processor();

    int start_process();
    
};

#endif
