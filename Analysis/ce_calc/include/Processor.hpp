#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <cstdlib>
#include "OptionsManager.hpp"
#include "EventIterator.hpp"
#include "SourceDataFile.hpp"
#include "ComptonEdgeCalc.hpp"
#include "SpectrumShow.hpp"
#include "Constants.hpp"

using namespace std;

class Processor {
private:
    OptionsManager* cur_options_mgr_;
    TApplication*   cur_rootapp_;

    EventIterator   eventIter_;
    SourceDataFile  source_data_file_;
    ComptonEdgeCalc compton_edge_calc_;
    SpectrumShow    spectrum_show_;

private:
    int do_action_1_();
    int do_action_2_();
    int do_action_3_();
    int do_action_4_();
    
public:
    Processor(OptionsManager* my_options_mgr,
              TApplication* my_rootapp);
    ~Processor();

    int start_process();
    
};

#endif
