#include "SciIterator.hpp"

SciIterator::SciIterator() {
    re_gps_      = "^ *(\\d+) *: *(\\d+) *$";

    t_file_in_ = NULL;
    t_modules_tree_ = NULL;
    t_trigger_tree_ = NULL;
    t_ped_modules_tree_ = NULL;
    t_ped_trigger_tree_ = NULL;
    
    cur_is_1P_ = false;

}

SciIterator::~SciIterator() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    t_file_in_ = NULL;
    t_modules_tree_ = NULL;
    t_trigger_tree_ = NULL;
    t_ped_modules_tree_ = NULL;
    t_ped_trigger_tree_ = NULL;
}

double SciIterator::value_of_gps_str_(const string gps_str) {
    cmatch cm;
    if (regex_match(gps_str.c_str(), cm, re_gps_)) {
        return TString(cm[1]).Atof() * 604800 + TString(cm[2]).Atof();
    } else {
        return -1;
    }
}

