#ifndef PEDDATAFILE_H
#define PEDDATAFILE_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "RootInc.hpp"

using namespace std;

class PedDataFile {
public:

private:
    TFile* t_ped_file_;
    TTree* t_ped_data_[25];
    
public:
    PedDataFile();
    ~PedDataFile();
    
};

#endif
