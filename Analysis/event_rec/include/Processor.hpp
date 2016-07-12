#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <cstdlib>
#include "RootInc.hpp"
#include "EventIterator.hpp"
#include "RecEventDataFile.hpp"
#include "OptionsManager.hpp"

using namespace std;

class Processor {
private:
    OptionsManager* cur_options_mgr_;
    TApplication*   cur_rootapp_;

    EventIterator   eventIter_;
    
public:
    Processor(OptionsManager* my_options_mgr,
              TApplication* my_rootapp);
    ~Processor();

    int start_process();
};

#endif
