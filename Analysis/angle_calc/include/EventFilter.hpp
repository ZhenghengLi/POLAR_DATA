#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include <iostream>
#include <cstdlib>
#include <utility>
#include <queue>
#include "RootInc.hpp"
#include "RecEventDataFile.hpp"
#include "Constants.hpp"
#include "BarPos.hpp"

using namespace std;

class EventFilter {
private:
    float low_energy_thr_;
    
public:
    
    bool is_first_two_ready;
    Bar  first_bar;
    Pos  first_pos;
    Bar  second_bar;
    Pos  second_pos;

private:

    bool check_na22_event_();

    double angle_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2);
    double distance_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2);    
    
public:
    EventFilter();
    ~EventFilter();
    
    bool find_first_two_bars(const RecEventDataFile::RecEvent_T& rec_event);    
    bool check(const RecEventDataFile::RecEvent_T& rec_event);

    void set_low_energy_thr(float energy);
    
};

#endif
