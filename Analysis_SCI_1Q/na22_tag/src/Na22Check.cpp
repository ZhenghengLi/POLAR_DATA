#include "Na22Check.hpp"

Na22Check::Na22Check() {
    angle_min_ = (150.0 / 180.0 * 3.1415926);
    distance_max_ = (1.3 * BarD);
}

Na22Check::~Na22Check() {

}

double Na22Check::angle_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2) {
    double vec1[2] = {x1 - x0, y1 - y0};
    double vec2[2] = {x2 - x0, y2 - y0};
    double norm_vec1 = TMath::Sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1]);
    double norm_vec2 = TMath::Sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1]);
    double product12 = vec1[0] * vec2[0] + vec1[1] * vec2[1];
    return TMath::ACos(product12 / (norm_vec1 * norm_vec2));
}

double Na22Check::distance_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2) {
    double vec01[2] = {x1 - x0, y1 - y0};
    double vec12[2] = {x2 - x1, y2 - y1};
    double norm_vec01 = TMath::Sqrt(vec01[0] * vec01[0] + vec01[1] * vec01[1]);
    double norm_vec12 = TMath::Sqrt(vec12[0] * vec12[0] + vec12[1] * vec12[1]);
    double product_01_12 = vec01[0] * vec12[0] + vec01[1] * vec12[1];
    double angle_01_12 = TMath::ACos(product_01_12 / (norm_vec01 * norm_vec12));
    return (norm_vec01 * TMath::Sin(angle_01_12));
}

bool Na22Check::check_na22_event(const POLEvent& t_pol_event) {

    bool is_na22 = true;
    bool found_not_adjacent = false;
    bool is_bad_event = false;

    Bar first_bar;
    Pos first_pos;
    Bar second_bar;
    Pos second_pos;
    priority_queue<Bar> bar_queue;
    for (int i = 0; i < 25; i++) {
        if (!t_pol_event.time_aligned[i]) continue;
        for (int j = 0; j < 64; j++) {
            if (t_pol_event.trigger_bit[i][j] && (t_pol_event.channel_status[i][j] & POLEvent::ADC_NOT_READOUT)) {
                is_bad_event = true;
            }
            if (t_pol_event.trigger_bit[i][j] && (t_pol_event.channel_status[i][j] & POLEvent::ADC_OVERFLOW)) {
                is_bad_event = true;
            }
            if (is_bad_event) break;
            if (t_pol_event.trigger_bit[i][j]) {
                bar_queue.push(Bar(t_pol_event.energy_value[i][j], i, j));
            }
        }
        if (is_bad_event) break;
    }
    if (!is_bad_event && !bar_queue.empty()) {
        first_bar = bar_queue.top();
        bar_queue.pop();
        first_pos.randomize(first_bar.i, first_bar.j);
        if (!bar_queue.empty()) {
            second_bar = bar_queue.top();
            bar_queue.pop();
            second_pos.randomize(second_bar.i, second_bar.j);
            found_not_adjacent = !first_pos.is_adjacent_to(second_pos);
        }
    }

    if (found_not_adjacent) {
        cur_is_valid = true;
        cur_first_ij[0] = first_bar.i;
        cur_first_ij[1] = first_bar.j;
        cur_second_ij[0] = second_bar.i;
        cur_second_ij[1] = second_bar.j;
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

        cur_source_id = largest_index;
        cur_angle = largest_angle / 3.1415927 * 180.0;
        cur_distance = src_distance[largest_index];

        if (largest_angle < angle_min_) {
            is_na22 = false;
        }
        if (src_distance[largest_index] > distance_max_) {
            is_na22 = false;
        }
    } else {
        cur_is_valid = false;
        is_na22 = false;
        cur_first_ij[0] = -1;
        cur_first_ij[1] = -1;
        cur_second_ij[0] = -1;
        cur_second_ij[1] = -1;
        cur_source_id = -1;
        cur_angle = -1;
        cur_distance = -1;
    }

    return is_na22;
}
