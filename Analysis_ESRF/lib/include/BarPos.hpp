#ifndef BARPOS_H
#define BARPOS_H

#include "TMath.h"
#include "CooConv.hpp"

struct Bar {
    float dep;
    int   i;
    int   j;

    Bar(float my_dep = 0, int my_i = 0, int my_j = 0) {
        dep = my_dep;
        i   = my_i;
        j   = my_j;
    }
    bool operator<(const Bar& right_bar) const {
        return (dep < right_bar.dep);
    }
    bool operator<=(const Bar& right_bar) const {
        return (dep <= right_bar.dep);
    }
    bool operator>(const Bar& right_bar) const {
        return (dep > right_bar.dep);
    }
    bool operator>=(const Bar& right_bar) const {
        return (dep >= right_bar.dep);
    }
    bool operator==(const Bar& right_bar) const {
        return (dep == right_bar.dep);
    }
};

struct Pos {
    int   i;
    int   j;
    float abs_x;
    float abs_y;

    Pos(int my_i = 0, int my_j = 0) {
        randomize(my_i, my_j);
    }
    void randomize(int my_i, int my_j) {
        i = my_i;
        j = my_j;
        abs_x = ijtox(i, j) / 8 * ModD + ijtox(i, j) % 8 * BarD + gRandom->Rndm() * BarW;
        abs_y = ijtoy(i, j) / 8 * ModD + ijtoy(i, j) % 8 * BarD + gRandom->Rndm() * BarW;
    }
    bool is_adjacent_to(const Pos& my_pos) {
        if (i != my_pos.i) {
            return false;
        } else {
            int j_diff = abs(j - my_pos.j);
            if (j_diff == 1 || j_diff == 8 || j_diff == 7 || j_diff == 9) {
                return true;
            } else {
                return false;
            }
        }
    }
    float angle_to(const Pos& my_pos) {
        float angle = TMath::ATan2(my_pos.abs_y - abs_y, my_pos.abs_x - abs_x) / TMath::Pi() * 180.0;
        return (angle >= 0 ? angle : 360 + angle);
    }
    float distance_to(const Pos& my_pos) {
        return TMath::Sqrt((my_pos.abs_y - abs_y) * (my_pos.abs_y - abs_y) + (my_pos.abs_x - abs_x) * (my_pos.abs_x - abs_x));
    }
};

#endif
