#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <cstdlib>
#include "OptionsManager.hpp"
#include "PedDataFile.hpp"
#include "PedMeanCalc.hpp"
#include "PedMeanShow.hpp"

using namespace std;

class Processor {
private:
    OptionsManager* cur_options_mgr_;

    PedDataFile ped_data_file_;
    PedMeanCalc ped_mean_calc_;
    PedMeanShow ped_mean_show_;

private:
    int do_action_1_();
    int do_action_2_();
    int do_action_3_();
    
public:
    Processor(OptionsManager* my_options_mgr);
    ~Processor();

    int start_process();
    
};

#endif
