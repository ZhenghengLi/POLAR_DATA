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

    EventIterator    eventIter_;
    RecEventDataFile rec_event_data_file_;

private:
    TVectorF ped_mean_vector_[25];
    TMatrixF xtalk_matrix_inv_[25];
    TVectorF adc_per_kev_vector_[25];
    TVectorF energy_vector_;

private:
    bool read_ped_mean_vector_(const char* filename);
    bool read_xtalk_matrix_inv_(const char* filename);
    bool read_adc_per_kev_vector_(const char* filename);
    bool gen_energy_vector_(EventIterator& eventIter);
    void reconstruct_all_events_(EventIterator& eventIter, RecEventDataFile& rec_event_data_file);
    
public:
    Processor(OptionsManager* my_options_mgr,
              TApplication* my_rootapp);
    ~Processor();

    int start_process();
};

#endif
