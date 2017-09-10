#ifndef NA22CHECK_H
#define NA22CHECK_H

#include <iostream>
#include <queue>
#include "DetInfo.hpp"
#include "BarPos.hpp"
#include "POLEvent.hpp"

using namespace std;

class Na22Check {
private:
    double angle_min_;
    double distance_max_;

private:
    double angle_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2);
    double distance_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2);

public:
    bool cur_is_valid;
    int cur_first_ij[2];
    int cur_second_ij[2];
    int cur_source_id;
    int cur_angle;
    int cur_distance;

public:
    Na22Check();
    ~Na22Check();

    void set_angle_min(double angle) { angle_min_ = angle; }
    void set_distance_max(double dist) { distance_max_ = dist; }
    bool check_na22_event(const POLEvent& t_pol_event);

};

#endif
