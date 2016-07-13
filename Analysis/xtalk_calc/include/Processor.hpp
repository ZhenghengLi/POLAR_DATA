#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <cstdlib>
#include "OptionsManager.hpp"
#include "SciIterator.hpp"
#include "XtalkDataFile.hpp"
#include "XtalkMatrixCalc.hpp"
#include "XtalkMatrixShow.hpp"
#include "Constants.hpp"

using namespace std;

class Processor {
private:
    OptionsManager* cur_options_mgr_;
    TApplication*   cur_rootapp_;

    SciIterator     sciIter_;
    XtalkDataFile   xtalk_data_file_;
    XtalkMatrixCalc xtalk_matrix_calc_;
    XtalkMatrixShow xtalk_matrix_show_;

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
