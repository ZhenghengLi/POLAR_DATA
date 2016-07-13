#include "EventFilter.hpp"

using namespace std;

EventFilter::EventFilter() {
    low_energy_thr_ = 0;
}

EventFilter::~EventFilter() {

}

double EventFilter::angle_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2) {
    double vec1[2] = {x1 - x0, y1 - y0};
    double vec2[2] = {x2 - x0, y2 - y0};
    double norm_vec1 = TMath::Sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1]);
    double norm_vec2 = TMath::Sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1]);
    double product12 = vec1[0] * vec2[0] + vec1[1] * vec2[1];
    return TMath::ACos(product12 / (norm_vec1 * norm_vec2));
}

double EventFilter::distance_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2) {
    double vec01[2] = {x1 - x0, y1 - y0};
    double vec12[2] = {x2 - x1, y2 - y1};
    double norm_vec01 = TMath::Sqrt(vec01[0] * vec01[0] + vec01[1] * vec01[1]);
    double norm_vec12 = TMath::Sqrt(vec12[0] * vec12[0] + vec12[1] * vec12[1]);
    double product_01_12 = vec01[0] * vec12[0] + vec01[1] * vec12[1];
    double angle_01_12 = TMath::ACos(product_01_12 / (norm_vec01 * norm_vec12));
    return (norm_vec01 * TMath::Sin(angle_01_12));
}

bool EventFilter::find_first_two_bars(const RecEventDataFile::RecEvent_T& rec_event) {
    is_first_two_ready = false;
    priority_queue<Bar> bar_queue;
    for (int i = 0; i < 25; i++) {
        if (rec_event.trig_accepted[i]) {
            for (int j = 0; j < 64; j++) {
                if (rec_event.trigger_bit[i * 64 + j] && rec_event.energy_dep[i * 64 + j] > 0) {
                    bar_queue.push(Bar(rec_event.energy_dep[i * 64 + j], i, j));
                }
            }
        }
    }
    if (bar_queue.empty())
        return false;
    first_bar = bar_queue.top();
    bar_queue.pop();
    first_pos.randomize(first_bar.i, first_bar.j);
    bool found_not_adjacent = false;
    while (!bar_queue.empty()) {
        second_bar = bar_queue.top();
        bar_queue.pop();
        second_pos.randomize(second_bar.i, second_bar.j);
        if (!first_pos.is_adjacent_to(second_pos)) {
            found_not_adjacent = true;
            break;
        }
    }
    if (!found_not_adjacent) {
        return false;
    } else {
        is_first_two_ready = true;
        return true;
    }
}

bool EventFilter::check_na22_event_() {
    if (!is_first_two_ready) {
        return false;
    }
    // calculate angle and distance to the 4 Na22 sources
    double src_angle[4];
    double src_distance[4];
    for (int i = 0; i < 4; i++) {
        src_angle[i] = angle_of_3_points_(SourcePos[i][0],  SourcePos[i][1],
                                          first_pos.abs_x,  first_pos.abs_y,
                                          second_pos.abs_x, second_pos.abs_y);
        src_distance[i] = distance_of_3_points_(SourcePos[i][0],  SourcePos[i][1],
                                                first_pos.abs_x,  first_pos.abs_y,
                                                second_pos.abs_x, second_pos.abs_y);
    }
    // find the largest angle
    double largest_angle = 0;
    int    largest_index = 0;
    for (int i = 0; i < 4; i++) {
        if (src_angle[i] > largest_angle) {
            largest_angle = src_angle[i];
            largest_index = i;
        }
    }
    if (largest_angle < AngleMin) {
        return false;
    }
    if (src_distance[largest_index] > DistanceMax) {
        return false;
    } else {
        return true;
    }
}

bool EventFilter::check(const RecEventDataFile::RecEvent_T& rec_event) {
    if (!is_first_two_ready)
        return false;
    if (second_bar.dep < low_energy_thr_)
        return false;
    if (check_na22_event_())
        return false;
    
    return true;
}

void EventFilter::set_low_energy_thr(float energy) {
    low_energy_thr_ = energy;
}
