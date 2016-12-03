#ifndef SAPFILEW_H
#define SAPFILEW_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "RootInc.hpp"
#include "SAPType.hpp"

using namespace std;

class SAPFileW: private SAPType {
private:
    TFile* t_file_out_;
    TTree* t_pol_event_tree_;

    bool first_flag_;
    int event_time_first_;
    int event_time_last_;

public:
    POLEvent_T t_pol_event;

public:
    SAPFileW();
    ~SAPFileW();

    bool open(const char* filename);
    void close();

    void fill_entry();
    void write_tree();
    void write_meta(const char* key, const char* value);

    string get_time_span();
};


#endif
