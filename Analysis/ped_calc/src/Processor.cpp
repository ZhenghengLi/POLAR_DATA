#include "Processor.hpp"

using namespace std;

Processor::Processor(OptionsManager* my_options_mgr) {
    cur_options_mgr_ = my_options_mgr;
}

Processor::~Processor() {
    
}

int Processor::start_process() {
    switch (cur_options_mgr_->action) {
    case 1:
        return do_action_1_();
    case 2:
        return do_action_2_();
    case 3:
        return do_action_3_();
    }
}

int Processor::do_action_1_() {
    return 0;
}

int Processor::do_action_2_() {
    return 0;
}

int Processor::do_action_3_() {
    return 0;
}
