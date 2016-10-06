#ifndef RECEVENTITERATOR_H
#define RECEVENTITERATOR_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <boost/regex.hpp>
#include "RootInc.hpp"
#include "RecEventType.hpp"
#include "Na22Info.hpp"
#include "BarPos.hpp"

#define GPS_SPAN_MIN 10
#define GPS_DIFF_MIN 10

using namespace std;
using boost::regex;
using boost::cmatch;
using boost::regex_match;

using namespace std;

class RecEventIterator: private RecEventType {
private:
    regex  re_gps_;
    regex  re_gps_span_;
    string gps_str_begin_;
    string gps_str_end_;
    double gps_value_begin_;
    double gps_value_end_;

    string  name_str_file_in_;
    TFile*  t_file_in_;
    TTree*  t_rec_event_tree_;
    TNamed* m_gps_span_;

    string  gps_str_first_;
    string  gps_str_last_;
    double  gps_value_first_;
    double  gps_value_last_;

    Long64_t first_entry_;
    Long64_t last_entry_;

private:
    double value_of_gps_str_(const string gps_str);
    Long64_t find_entry_(TTree* t_tree, RecEvent_T& t_branch, double gps_value);

    double angle_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2);
    double distance_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2);

public:
    RecEvent_T t_rec_event;

    bool is_first_two_ready;
    Bar  first_bar;
    Pos  first_pos;
    Bar  second_bar;
    Pos  second_pos;

private:
    Long64_t cur_entry_;
    bool     reach_end_;

public:
    RecEventIterator();
    ~RecEventIterator();

    bool open(const char* filename);
    bool open(const char* filename,
              const char* gps_begin, const char* gps_end);
    void close();
    void print_file_info();    
    
    void     set_start();
    Long64_t get_total_entries();
    Long64_t get_cur_entry();
    bool     next_event();

    bool     find_first_two_bars();
    bool     cur_is_na22();

};

#endif
