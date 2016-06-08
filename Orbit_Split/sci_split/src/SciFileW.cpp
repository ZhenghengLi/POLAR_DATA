#include "SciFileW.hpp"

SciFileW::SciFileW() {
    t_file_out_ = NULL;
    t_modules_tree_ = NULL;
    t_trigger_tree_ = NULL;
    t_ped_modules_tree_ = NULL;
    t_ped_trigger_tree_ = NULL;

    cur_scifile_r = NULL;
}

SciFileW::~SciFileW() {
    close();
}


bool SciFileW::open(const char* filename) {
    if (t_file_out_ != NULL)
        return false;
    
    t_file_out_ = new TFile(filename, "RECREATE");
    if (t_file_out_->IsZombie())
        return false;

    // t_modules
    t_modules_tree_ = new TTree("t_modules", "physical modules packets");
    build_modules_tree(t_modules_tree_, t_modules);

    // t_trigger
    t_trigger_tree_ = new TTree("t_trigger", "physical trigger packets");
    build_trigger_tree(t_trigger_tree_, t_trigger);

    // t_ped_modules
    t_ped_modules_tree_ = new TTree("t_ped_modules", "pedestal modules packets");
    build_modules_tree(t_ped_modules_tree_, t_ped_modules);

    // t_ped_trigger
    t_ped_trigger_tree_ = new TTree("t_ped_trigger", "pedestal trigger packets");
    build_trigger_tree(t_ped_trigger_tree_, t_ped_trigger);

    return true;
}

void SciFileW::close() {
    if (t_file_out_ == NULL)
        return;

    delete t_modules_tree_;
    t_modules_tree_ = NULL;

    delete t_trigger_tree_;
    t_trigger_tree_ = NULL;

    delete t_ped_modules_tree_;
    t_ped_modules_tree_ = NULL;

    delete t_ped_trigger_tree_;
    t_ped_trigger_tree_ = NULL;

    t_file_out_->Close();
    delete t_file_out_;
    t_file_out_ = NULL;
}
