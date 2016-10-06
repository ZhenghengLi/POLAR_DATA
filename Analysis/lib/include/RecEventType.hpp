#ifndef RECEVENTTYPE_H
#define RECEVENTTYPE_H

#include "RootInc.hpp"

class RecEventType {
public:
    struct RecEvent_T {
        Int_t    abs_gps_week;
        Double_t abs_gps_second;
        Bool_t   abs_gps_valid;
        Double_t abs_ship_second;
        Int_t    type;
        Bool_t   trig_accepted[25];
        Bool_t   trigger_bit[1600];
        Int_t    trigger_n;
        Int_t    multiplicity[25];
        Float_t  energy_dep[1600];
    };
    
public:
    RecEventType();
    void bind_rec_event_tree(TTree* t_rec_event_tree, RecEvent_T& t_rec_event);

};

#endif
