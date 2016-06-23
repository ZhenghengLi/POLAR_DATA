#include "SciFile1RR.hpp"

using namespace std;

SciFile1RR::SciFile1RR() {
    t_file_in_ = NULL;
    t_triggerl1_tree_ = NULL;
    t_modulesl1_tree_ = NULL;
}

SciFile1RR::~SciFile1RR() {
    close();
}

bool SciFile1RR::open(const char* filename) {
    
}
