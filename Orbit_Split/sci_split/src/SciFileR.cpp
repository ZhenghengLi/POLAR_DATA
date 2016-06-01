#include "SciFileR.hpp"

SciFileR::SciFileR() {
    re_gps_ = "^ *(\\d+) *: *(\\d+) *$";

    t_file_in_ = NULL;
    t_modules_tree_ = NULL;
    t_trigger_tree_ = NULL;
    t_ped_modules_tree_ = NULL;
    t_ped_trigger_tree_ = NULL;

    gps_value_first_phy_ = -1;
    gps_value_first_ped_ = -1;
    gps_value_last_phy_  = -1;
    gps_value_last_ped_  = -1;
    
}

SciFileR::~SciFileR() {

}

bool SciFileR::open(const char* filename, const char* gps_begin, const char* gps_end) {
    // record gps time
    gps_str_begin_.assign(gps_begin);
    if (gps_str_begin_ == "begin") {
        gps_value_begin_ = 0;
    } else {
        gps_value_begin_ = value_of_gps_str_(gps_str_begin_);
        if (gps_value_begin_ < 0) {
            cerr << "GPS string: " << gps_str_begin_ << " is invalid, it should be week:second." << endl;
            return false;
        }
    }
    gps_str_end_.assign(gps_end);    
    if (gps_str_end_ == "end") {
        gps_value_end_ = numeric_limits<double>::max();
    } else {
        gps_value_end_ = value_of_gps_str_(gps_str_end_);
        if (gps_value_end_ < 0) {
            cerr << "GPS string: " << gps_str_end_ << " is invalid, it should be week:second." << endl;
            return false;
        }
    }
    
    // open file and check
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_trigger_tree_ = static_cast<TTree*>(t_file_in_->Get("t_trigger"));
    if (t_trigger_tree_ == NULL)
        return false;
    if (t_trigger_tree_->FindBranch("abs_gps_week") == NULL ||
        t_trigger_tree_->FindBranch("abs_gps_second") == NULL ||
        t_trigger_tree_->FindBranch("abs_gps_valid") == NULL) {
        cerr << "File: " << filename << " may be not a 1P level SCI data file." << endl;
        return false;
    }
    t_modules_tree_ = static_cast<TTree*>(t_file_in_->Get("t_modules"));
    if (t_modules_tree_ == NULL)
        return false;
    t_ped_trigger_tree_ = static_cast<TTree*>(t_file_in_->Get("t_ped_trigger"));
    if (t_ped_trigger_tree_ == NULL)
        return false;
    t_ped_modules_tree_ = static_cast<TTree*>(t_file_in_->Get("t_ped_modules"));
    if (t_ped_modules_tree_ == NULL)
        return false;
    if (t_trigger_tree_->GetEntries() < 1 ||
        t_modules_tree_->GetEntries() < 1 ||
        t_ped_trigger_tree_->GetEntries() < 1 ||
        t_ped_modules_tree_->GetEntries() < 1) {
        cerr << "File: " << filename << " may be empty.";
        return false;
    }
    
    // bind TTree
    bind_trigger_tree(t_trigger_tree_, t_trigger);
    bind_modules_tree(t_modules_tree_, t_modules);
    bind_trigger_tree(t_ped_trigger_tree_, t_ped_trigger);
    bind_modules_tree(t_ped_modules_tree_, t_ped_modules);
    
    // find the first and last gps
    char str_buffer[80];
    for (Long64_t i = 0; i < t_trigger_tree_->GetEntries(); i++) {
        t_trigger_tree_->GetEntry(i);
        if (t_trigger.abs_gps_valid) {
            gps_value_first_phy_ = t_trigger.abs_gps_week * 604800 + t_trigger.abs_gps_second;
            sprintf(str_buffer, "%d:%d", t_trigger.abs_gps_week, static_cast<int>(t_trigger.abs_gps_second));
            gps_str_first_phy_.assign(str_buffer);
        }
    }
    
    
    return true;
}

void SciFileR::close() {

}

double SciFileR::value_of_gps_str_(const string gps_str) {
    cmatch cm;
    if (regex_match(gps_str.c_str(), cm, re_gps_)) {
        return TString(cm[1]).Atof() * 604800 + TString(cm[2]).Atof();
    } else {
        return -1;
    }
}
