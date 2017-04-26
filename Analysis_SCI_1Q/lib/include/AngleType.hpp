#ifndef ANGLETYPE_H
#define ANGLETYPE_H

#include <iostream>
#include "RootInc.hpp"
#include "POLEvent.hpp"

using namespace std;

class POLEvent {
public:
    Double_t  event_time;
    Int_t     first_ij[2];
    Int_t     second_ij[2];
    Float_t   rand_angle;
    Float_t   rand_distance;
    Float_t   first_energy;
    Float_t   second_energy;
    Bool_t    is_valid;
    Bool_t    is_na22;
    Bool_t    is_cosmic;
    Float_t   deadtime_weight;
    Float_t   efficiency_weight;

public:
    void clear();
    void build_pol_angle_tree(TTree* t_pol_angle_tree);

};

#endif
